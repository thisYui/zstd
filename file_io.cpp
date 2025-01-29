#include "file_io.h"
#include <cstring>

void FileEntry::setHeader(const char* header)
{
	if (header == nullptr) 
	{
		std::cerr << "Header is null" << std::endl;
		exit(1);
	}

	// Sử dụng memcpy để sao chép dữ liệu vào từng thành viên
	std::memcpy(&lenghtName, header, sizeof(lenghtName));               // 1 byte đầu
	std::memcpy(&sizeOfTree, header + 1, sizeof(sizeOfTree));           // 2 byte tiếp theo
	std::memcpy(&isDirectory, header + 3, sizeof(isDirectory));         // Byte thứ 4
	std::memcpy(&sizeOfData, header + 4, sizeof(sizeOfData));           // 4 bytes tiếp theo
	std::memcpy(&bufferLz77, header + 8, sizeof(bufferLz77));           // 2 bytes tiếp theo
	std::memcpy(&origanalSize, header + 10, sizeof(origanalSize));      // 4 bytes cuối
}

void FileCompress::readHeader(std::ifstream& file)
{
	// Đọc thông tin Header
	file.read(reinterpret_cast<char *>(&lenghtName), sizeof(lenghtName));
	file.read(reinterpret_cast<char *>(&countFile), sizeof(countFile));
	file.read(reinterpret_cast<char *>(&isDirectory), sizeof(isDirectory));

	// Đọc tên file/folder
	char* name = new char[lenghtName + 1];
	file.read(name, lenghtName);
	name[lenghtName] = '\0';
	fileName = name;
	delete[] name;
}

FileEntry FileCompress::readData(std::ifstream& file)
{
	FileEntry fileEntry("", 0, false);

	// Đọc 14 bytes Header
	char header[HEADER_FILE_ENTRY];
	file.read(header, HEADER_FILE_ENTRY);

	fileEntry.setHeader(header);

	// Đọc tên file/folder
	char* name = new char[fileEntry.lenghtName + 1];
	file.read(name, fileEntry.lenghtName);
	name[fileEntry.lenghtName] = '\0';
	fileEntry.fileName = name;
	delete[] name;

	// Đọc cây mã hóa Huffman
	fileEntry.tree.resize(fileEntry.sizeOfTree);
	file.read(reinterpret_cast<char *>(fileEntry.tree.data()), fileEntry.sizeOfTree);

	// Đọc dữ liệu nén
	fileEntry.data.resize(fileEntry.sizeOfData);
	file.read(reinterpret_cast<char *>(fileEntry.data.data()), fileEntry.sizeOfData);

	return fileEntry;
}

bool FileCompress::read(const std::string& inputFile)
{
    std::ifstream file;
	file.open(inputFile, std::ios::binary);  // Đọc ở chế độ binary

    if (!file.is_open())
	{
		std::cout << "Cannot open file " << inputFile << std::endl;
		return false;
	}

	readHeader(file);

	// Đọc thông tin các file/folder con
    for (int i = 0; i < countFile; i++)
    {
		files.push_back(readData(file));
    }

    return true;
}

bool FileCompress::write()
{
	if (files.empty())
	{
		std::cout << "No file to write!" << std::endl;
		return false;
	}

	std::ofstream outFile(getFileName(), std::ios::binary);
	if (!outFile.is_open())
	{
		std::cout << "Cannot open file " << getFileName() << std::endl;
		return false;
	}

	// Ghi thông tin Header
	outFile.write(reinterpret_cast<char *>(&lenghtName), sizeof(lenghtName));
	outFile.write(reinterpret_cast<char *>(&countFile), sizeof(countFile));
	outFile.write(reinterpret_cast<char *>(&isDirectory), sizeof(isDirectory));
	outFile.write(fileName.c_str(), fileName.size());

	// Ghi thông tin các file/folder con
	for (const auto& file : files)
	{
		// Ghi 14 bytes Header
		char header[HEADER_FILE_ENTRY];
		std::memcpy(header, &file.lenghtName, sizeof(file.lenghtName));
		std::memcpy(header + 1, &file.sizeOfTree, sizeof(file.sizeOfTree));
		std::memcpy(header + 3, &file.isDirectory, sizeof(file.isDirectory));
		std::memcpy(header + 4, &file.sizeOfData, sizeof(file.sizeOfData));
		std::memcpy(header + 8, &file.bufferLz77, sizeof(file.bufferLz77));
		std::memcpy(header + 10, &file.origanalSize, sizeof(file.origanalSize));

		outFile.write(header, HEADER_FILE_ENTRY);

		// Ghi tên file/folder
		outFile.write(file.fileName.c_str(), file.fileName.size());

		// Ghi các mã hóa Huffman
		outFile.write((char*)file.tree.data(), file.tree.size());

		// Ghi dữ liệu nén
		outFile.write((char*)file.data.data(), file.data.size());
	}

	return true;
}

std::string FileCompress::getFileName()
{
	if (isDirectory)
	{
		return fileName + ".bin";
	}

	// Xóa đuôi file
	const size_t pos = fileName.find_last_of('.');
	return fileName.substr(0, pos) + ".bin";
}

void FileDecompress::scanFolder(const fs::path& path)
{
	try {
		// Kiểm tra xem thư mục có tồn tại không
		if (!fs::exists(path))
		{
			std::cout << "Directory does not exist!" << std::endl;
			return;
		}

		if (!fs::is_directory(path))
		{
			files.emplace_back(path.string(), false);
			return;
		}

		// Duyệt qua tất cả các phần tử trong thư mục và thư mục con
		for (const auto& entry : fs::recursive_directory_iterator(path))
		{
			bool isDir = fs::is_directory(entry);

			// Lưu thông tin vào vector entries
			files.emplace_back(entry.path().string(), isDir);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}

void FileDecompress::read()
{
	// Kiểm tra path có tồn tại không
	if (!fs::exists(path))
	{
		std::cout << "File does not exist!" << std::endl;
		return;
	}

	// Kiểm tra path là folder hay file
	isDirectory = fs::is_directory(path) ? true : false;

	// Lấy danh sách file/folder trong thư mục
	scanFolder(path);

	// Đọc dữ liệu từ file
	for (auto& file : files)
	{
		if (!file.isDirectory)
		{
			std::ifstream inFile(file.name, std::ios::binary);
			if (!inFile.is_open())
			{
				std::cout << "Cannot open file " << file.name << std::endl;
				return;
			}

			// Đọc dữ liệu từ file
			inFile.seekg(0, std::ios::end);  // Đưa con trỏ đọc về cuối file
			file.data.resize(inFile.tellg());  // Lấy kích thước file
			inFile.seekg(0, std::ios::beg);  // Đưa con trỏ đọc về đầu file
			inFile.read(reinterpret_cast<char *>(file.data.data()), file.data.size());
			inFile.close();
		}
	}
}

bool FileDecompress::write()
{
	if (files.empty())
	{
		std::cout << "No file to write!" << std::endl;
		return false;
	}

	try 
	{
		if (fs::exists(path))
		{
			std::string input;
			std::cout << "Folder already exists. Do you want to overwrite it? (y/n): ";
			std::cin >> input;

			if (input != "y")
			{
				return false;
			}
			
			fs::remove_all(path);	
		}
		// Tạo thư mục mới
		if (isDirectory)
		{
			fs::create_directory(path);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return false;
	}


	for (const auto& file : files)
	{
		// Nếu là Folder
		if (file.isDirectory)
		{
			fs::create_directory(file.name);
		}
		// Nếu là File
		else
		{
			std::ofstream outFile(file.name, std::ios::binary);
			if (!outFile.is_open())
			{
				std::cout << "Cannot open file " << file.name << std::endl;
				return false;
			}

			outFile.write((char*)file.data.data(), file.data.size());
			outFile.close();
		}
	}

	return true;
}

FileCompress transmission(const FileDecompress& fileDecompress)
{
	FileCompress fileCompress;

	fileCompress.fileName = fileDecompress.path;
	fileCompress.isDirectory = fileDecompress.isDirectory;
	fileCompress.countFile = fileDecompress.files.size();
	fileCompress.lenghtName = fileDecompress.path.size();

	return fileCompress;
}

FileDecompress transmission(const FileCompress& fileCompress)
{
	FileDecompress fileDecompress;

	fileDecompress.path = fileCompress.fileName;
	fileDecompress.isDirectory = fileCompress.isDirectory;

	return fileDecompress;
}

FileEntry transmission(const FileSource& fileSource)
{
	FileEntry fileEntry;

	fileEntry.fileName = fileSource.name;
	fileEntry.lenghtName = fileSource.name.size();
	fileEntry.isDirectory = fileSource.isDirectory;

	return fileEntry;
}

FileSource transmission(const FileEntry& fileEntry)
{
	FileSource fileSource;

	fileSource.name = fileEntry.fileName;
	fileSource.isDirectory = fileEntry.isDirectory;

	return fileSource;
}
