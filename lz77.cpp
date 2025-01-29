#include "lz77.h"
#include <iostream>

// 4 bytes đánh dấu kết thúc 1 block tương đương 1 tuple
// Chọn 4 bytes 0xff đảm bảo không thể xảy ra trường hợp trùng với dữ liệu nén
const std::tuple<int, int, uint8_t> EOB = std::make_tuple(0xffff, 0xff, 0xff);  // End of block

std::vector<std::tuple<int, int, uint8_t>> lz77::compress(
    const std::vector<uint8_t>& input, 
    int searchBufferSize, 
    int lookaheadBufferSize)
{
	std::vector<std::tuple<int, int, uint8_t>> compressedData;

	// Chia dữ liệu thành các block nhỏ
	std::vector<std::vector<uint8_t>> blocks = splitIntoBlocks(input);

	// Duyệt qua từng block và nén dữ liệu
	for (const auto& block : blocks)
	{
		std::vector<uint8_t> searchBuffer;
		std::vector<uint8_t> lookaheadBuffer;
		int a[15] = { 0 };
		int z = 0;
		int max = 0;
		// Duyệt qua từng byte trong block
		for (size_t i = 0; i < block.size() || !lookaheadBuffer.empty();)
		{
			// Cập nhật lookaheadBuffer
			while (lookaheadBuffer.size() < lookaheadBufferSize
				&& i < block.size())
			{
				lookaheadBuffer.push_back(block[i]);
				i++;
			}

			// Tạo chuỗi trùng khớp
			auto match = createMatch(searchBuffer, lookaheadBuffer);

			// Thêm chuỗi trùng khớp vào compressedData
			compressedData.push_back(match);

			int offset = std::get<0>(match);
			int length = std::get<1>(match);		
			uint8_t nextByte = std::get<2>(match);

			if (length < 15) a[length]++;
			else z++;

			if (length > max) max = length;

			// Cập nhật searchBuffer
			for (int j = 0; j < length; j++)
			{
				searchBuffer.push_back(lookaheadBuffer[j]);
			}
			searchBuffer.push_back(nextByte);

			// Cập nhật lookaheadBuffer
			lookaheadBuffer.erase(lookaheadBuffer.begin(), lookaheadBuffer.begin() + 
				std::min(length + 1, static_cast<int>(lookaheadBuffer.size())));

		}

		for (int i = 0; i < 15; i++)
		{
			std::cout << i << ": " << a[i] << "   ";
		} std::cout << std::endl;
		std::cout << "more 15: " << z << "        ";
		std::cout << "max: " << max << std::endl;

		// Thêm EOT vào cuối block
		compressedData.push_back(EOB);
	}

	return compressedData;
}

std::vector<uint8_t> lz77::decompress(const std::vector<std::tuple<int, int,uint8_t>>& compressedData)
{
	std::vector<uint8_t> decompressedData;

	std::vector<std::vector<std::tuple<int, int, uint8_t>>> blocks = getBlockData(compressedData);

	for (const auto& block : blocks)
	{
		std::vector<uint8_t> searchBuffer;

		for (const auto& data : block)
		{
			int offset = std::get<0>(data);
			int length = std::get<1>(data);
			uint8_t nextByte = std::get<2>(data);
			for (int i = 0; i < length; i++)
			{
				searchBuffer.push_back(searchBuffer[offset + i]);
			}

			searchBuffer.push_back(nextByte);
		}

		for (const auto& byte : searchBuffer)
		{
			decompressedData.push_back(byte);
		}
	}

	return decompressedData;
}

std::vector<std::vector<uint8_t>> lz77::splitIntoBlocks(
    const std::vector<uint8_t>& input, 
    size_t blockSize)
{
	std::vector<std::vector<uint8_t>> blocks;

	for (size_t i = 0; i < input.size(); i += blockSize)
	{
		std::vector<uint8_t> block;
		for (size_t j = i; j < i + blockSize && j < input.size(); j++)
		{
			block.push_back(input[j]);
		}
		blocks.push_back(block);
	}

	return blocks;
}

std::vector<std::vector<std::tuple<int, int, uint8_t>>> lz77::getBlockData(const std::vector<std::tuple<int, int, uint8_t>>& lz77)
{
	std::vector<std::vector<std::tuple<int, int, uint8_t>>> result;

	size_t n = lz77.size();
	for (size_t i = 0; i < n; i++)
	{
		if (lz77[i] == EOB)
		{
			continue;
		}

		std::vector<std::tuple<int, int, uint8_t>> blockData;
		while (i < n && lz77[i] != EOB)
		{
			blockData.push_back(lz77[i]);
			i++;
		}

		result.push_back(blockData);
	}

	return result;
}

//std::tuple<int, int, uint8_t> lz77::createMatch(
//    const std::vector<uint8_t>& searchBuffer,
//    const std::vector<uint8_t>& lookaheadBuffer)
//{
//    int bestOffset = 0;  // Vị trí bắt đầu của chuỗi con trùng khớp tốt nhất
//    int bestLength = 0;   // Độ dài chuỗi con trùng khớp tốt nhất
//    uint8_t nextByte = 0; // Ký tự tiếp theo trong lookaheadBuffer
//
//    // Duyệt qua searchBuffer và so sánh với lookaheadBuffer
//	for (size_t i = 0; i < searchBuffer.size(); i++)
//	{
//		int offset = i;
//		int length = 0;
//
//		// So sánh chuỗi con từ vị trí i trong searchBuffer với chuỗi con từ vị trí 0 trong lookaheadBuffer
//		while (i < searchBuffer.size() && 
//			length < lookaheadBuffer.size() && 
//			searchBuffer[i] == lookaheadBuffer[length])
//		{
//			i++;
//			length++;
//		}
//
//		// Cập nhật offset và length tốt nhất
//		if (length > bestLength)
//		{
//			bestOffset = offset;
//			bestLength = length;
//		}
//
//		// Đặt lại vị trí i
//		i = offset;
//	}
//
//	if (bestLength < lookaheadBuffer.size())
//	{
//		nextByte = lookaheadBuffer[bestLength];
//	}
//	else
//	{
//		bestLength = lookaheadBuffer.size() - 1;
//		nextByte = lookaheadBuffer.back();
//	}
//
//    // Trả về tuple (offset, length, nextByte)
//    return std::make_tuple(bestOffset, bestLength, nextByte);
//}

std::tuple<int, int, uint8_t> lz77::createMatch(
	const std::vector<uint8_t>& searchBuffer,
	const std::vector<uint8_t>& lookaheadBuffer)
{
	int bestOffset = 0;  // Vị trí bắt đầu của chuỗi con trùng khớp tốt nhất
	int bestLength = 0;  // Độ dài chuỗi con trùng khớp tốt nhất
	uint8_t nextByte = lookaheadBuffer.empty() ? 0 : lookaheadBuffer[0];

	// Sử dụng sliding window để tìm kiếm chuỗi trùng khớp
	for (size_t i = 0; i < searchBuffer.size(); ++i)
	{
		int currentLength = 0;

		// So khớp chuỗi trong phạm vi của lookaheadBuffer
		while (currentLength < static_cast<int>(lookaheadBuffer.size()) &&
			i + currentLength < searchBuffer.size() &&
			searchBuffer[i + currentLength] == lookaheadBuffer[currentLength])
		{
			++currentLength;
		}

		// Cập nhật chuỗi con tốt nhất nếu cần
		if (currentLength > bestLength)
		{
			bestOffset = static_cast<int>(i);
			bestLength = currentLength;

			// Cập nhật ký tự tiếp theo nếu còn trong lookaheadBuffer
			if (bestLength < static_cast<int>(lookaheadBuffer.size()))
			{
				nextByte = lookaheadBuffer[bestLength];
			}
		}
	}

	if (bestLength < lookaheadBuffer.size())
	{
		nextByte = lookaheadBuffer[bestLength];
	}
	else
	{
		bestLength = lookaheadBuffer.size() - 1;
		nextByte = lookaheadBuffer.back();
	}

	// Trả về tuple (offset, length, nextByte)
	return std::make_tuple(bestOffset, bestLength, nextByte);
}

bool lz77::validateInput(const std::vector<uint8_t>& input)
{
	// Kiểm tra xem input có rỗng không
	if (input.empty())
	{
		return false;
	}
	// Kiểm tra xem input có quá nhỏ không
	if (input.size() < SEARCH_BUFFER_SIZE)
	{
		return false;
	}
	return true;
}

std::vector<uint8_t> lz77::concatTuple(const std::vector<std::tuple<int, int, uint8_t>>& data)
{
	/*
	* 1. Mỗi tuple gồm 3 phần tử: offset, length, nextByte
	* 2. offset: 2 bytes, length: 1 bytes, nextByte: 1 byte
	* 3. Chuyển 3 phần tử của tuple thành 4 bytes
	*
	* Ta thấy rõ block dài 32*1024 tức ko quá 2 bytes => offset không quá 2 bytes
	* length không quá 1 byte vì LOOKAHEAD_BUFFER_SIZE = 16
	* nextByte không quá 1 byte
	*/
	std::vector<uint8_t> result;

	for (const auto& tuple : data)
	{
		int offset = std::get<0>(tuple);
		int length = std::get<1>(tuple);
		uint8_t nextByte = std::get<2>(tuple);

		// Chuyển offset thành 2 bytes
		result.push_back(static_cast<uint8_t>(offset & 0xFF));
		result.push_back(static_cast<uint8_t>((offset >> 8) & 0xFF));
		// Chuyển length thành 1 byte
		result.push_back(static_cast<uint8_t>(length));
		// Chuyển nextByte thành 1 byte
		result.push_back(nextByte);
	}

	return result;
}

std::vector<std::tuple<int, int, uint8_t>> lz77::unconcatTuple(const std::vector<uint8_t>& data)
{
	std::vector<std::tuple<int, int, uint8_t>> result;

	for (size_t i = 0; i < data.size(); i += 4)
	{
		int offset = data[i] | (data[i + 1] << 8);
		int length = data[i + 2];
		uint8_t nextByte = data[i + 3];

		result.push_back(std::make_tuple(offset, length, nextByte));
	}

	return result;
}