#include "zstd.h"

std::vector<uint8_t> zstd::dataToLz77(
	const std::vector<uint8_t>& data, 
	uint16_t& bufferLz77)
{
	std::vector<uint8_t> copyData = data;
	std::vector<std::tuple<int, int, uint8_t>> compressedData;

	// Kiểm tra dữ liệu đầu vào
	if (data.empty() || !lz77::validateInput(data))
	{
		return data;
	}

	// Kiểm tra kích thước dữ liệu có chia hết cho BLOCK_SIZE không
	if (data.size() % lz77::BLOCK_SIZE != 0)
	{
		// Ghi lại buffer
		bufferLz77 = lz77::BLOCK_SIZE - data.size() % lz77::BLOCK_SIZE;
		for (size_t i = 0; i < bufferLz77; i++)
		{
			copyData.push_back(0x00);
		}
	}
	else
	{
		bufferLz77 = 0;
	}

	// Nén dữ liệu với lz77
	compressedData = lz77::compress(copyData); 

	return lz77::concatTuple(compressedData);
}

std::vector<uint8_t> zstd::lz77ToData(
	const std::vector<uint8_t>& data, 
	uint16_t& bufferLz77)
{
	// Tách dữ liệu thành các tuple
	std::vector<std::tuple<int, int, uint8_t>> compressedData = lz77::unconcatTuple(data);

	// Giải nén dữ liệu với lz77
	std::vector<uint8_t> decompressedData = lz77::decompress(compressedData); 

	// Xóa buffer
	if (bufferLz77 != 0)
	{
		decompressedData.erase(decompressedData.end() - bufferLz77, decompressedData.end());
	}

	return decompressedData;
}

std::vector<uint8_t> zstd::lz77ToHuffman(
	const std::vector<uint8_t>& data,
	uint32_t& originalSize,
	std::vector<uint8_t>& trans
)
{
	originalSize = data.size();
	return huffman::encode(data,trans);
}

std::vector<uint8_t> zstd::huffmanToLz77(
	const std::vector<uint8_t>& data,
	HuffmanNode* root,
	uint32_t& originalSize)
{
	return huffman::decode(data, root, originalSize);
}

bool zstd::compress(const std::string& inputFileName)
{
	// Đọc dữ liệu
	FileDecompress fileDecompress(inputFileName);
	// Đưa thông tin vào biến
	fileDecompress.read();  

	// Nén dữ liệu
	FileCompress fileCompress = transmission(fileDecompress);  // Truyền thông tin
	
	// Xử lí từng phần
	for (auto& file : fileDecompress.files)
	{
		FileEntry entry = transmission(file);  // Truyền thông tin

		if (!entry.isDirectory)
		{
			// Nén dữ liệu
			std::vector<uint8_t> lz77Data = dataToLz77(file.data, entry.bufferLz77);
			std::vector<uint8_t> huffmanData = lz77ToHuffman(lz77Data, entry.origanalSize, entry.tree);

			// Đưa các phần dữ liệu vào fileEntry
			entry.sizeOfTree = entry.tree.size();
			entry.sizeOfData = huffmanData.size();
			entry.data = huffmanData;
		}

		// Thêm vào fileCompress
		fileCompress.files.push_back(entry);

		// Xóa dữ liệu để giải phóng bớt bộ nhớ
		file.data.clear();
		
	}

	// Ghi dữ liệu ra file

	return fileCompress.write();
}

bool zstd::decompress(const std::string& inputFileName)
{
	// Đọc dữ liệu
	FileCompress fileCompress;
	fileCompress.read(inputFileName);

	// Tạo giải nén
	FileDecompress fileDecompress = transmission(fileCompress);  // Truyền thông tin

	// Xử lí từng phần
	for (auto& file : fileCompress.files)
	{
		FileSource source = transmission(file);  // Truyền thông tin

		if (!source.isDirectory)
		{
			// Giải nén dữ liệu
			HuffmanTree tree;
			tree.root = huffman::readTree(file.tree);
			std::vector<uint8_t> lz77Data = huffmanToLz77(file.data, tree.root, file.origanalSize);
			std::vector<uint8_t> data = lz77ToData(lz77Data, file.bufferLz77);

			// Đưa dữ liệu vào fileSource
			source.data = data;
		}

		// Thêm vào fileDecompress
		fileDecompress.files.push_back(source);

		// Xóa dữ liệu để giải phóng bớt bộ nhớ
		file.data.clear();
	}

	// Ghi dữ liệu ra file
	return fileDecompress.write();
}
