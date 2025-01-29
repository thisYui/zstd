#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

constexpr int HEADER_FILE_ENTRY = 14;

class FileEntry 
{
public:
	// Header : 14 bytes
	uint8_t lenghtName;		// Độ dài tên file/folder
	uint16_t sizeOfTree;	// Số lượng mã hóa Huffman
	bool isDirectory;		// Kiểm tra xem có phải folder không
	uint32_t sizeOfData;	// Kích thước phần data
	uint16_t bufferLz77;	// Buffer cho LZ77 để các block được chia đều
	uint32_t origanalSize;  // Kích thước dữ liệu sau phép nén LZ77

	// Data
	std::string fileName;
	std::vector<uint8_t> tree;  // Mã hóa Huffman
	std::vector<uint8_t> data;  // Dữ liệu nén

    // Constructor
	FileEntry() : lenghtName(0), sizeOfTree(0), isDirectory(false), sizeOfData(0), bufferLz77(0), origanalSize(0) {}
	FileEntry(const std::string& p, uint64_t s, bool isDir)
		: fileName(p), origanalSize(s), isDirectory(isDir) {}
	void setHeader(const char* header);  // Thiết lập thông tin Header : 19 bytes
	void printInfomation() const
	{
		std::cout << "File name: " << fileName << std::endl;
		std::cout << "Size of tree: " << static_cast<int>(sizeOfTree) << std::endl;
		std::cout << "Size of data: " << static_cast<int>(sizeOfData) << std::endl;
		std::cout << "Buffer LZ77: " << static_cast<int>(bufferLz77) << std::endl;
		std::cout << "Original size: " << static_cast<int>(origanalSize) << std::endl;
	}

};

class FileCompress {
public:
	// Header : 3 bytes
	uint8_t lenghtName;
	uint8_t countFile;
    bool isDirectory;

	// Data
	std::string fileName;
	std::vector<FileEntry> files;

    // Constructor để dễ dàng tạo đối tượng
	FileCompress() : lenghtName(0), countFile(0), isDirectory(false) {}
	FileCompress(const std::string& p, bool isDir, std::vector<FileEntry> f)
		: fileName(p), isDirectory(isDir), files(f) {}
	std::vector<FileEntry> getFiles() { return files; }
	void readHeader(std::ifstream& file);
	FileEntry readData(std::ifstream& file);
	bool read(const std::string& inputFile);
	bool write();
	std::string getFileName();
	void printInfomation() const
	{
		std::cout << "File Name: " << fileName << std::endl;
		std::cout << "Is Directory: " << isDirectory << std::endl;
		std::cout << "Count File: " << (int)countFile << std::endl;
		for (const auto& file : files)
		{
			std::cout << "Name: " << file.fileName << "\t\t";
			std::cout << "Is Directory: " << file.isDirectory << "\t\t";
			std::cout << "Size: " << file.data.size() << std::endl;
		}
	}
};


class FileSource
{
public:
	std::string name;
	bool isDirectory;
	std::vector<uint8_t> data;

	FileSource() : name(""), isDirectory(false) {}
	FileSource(const std::string& n, bool isDir)
		: name(n), isDirectory(isDir) {}
	void printInfomation() const
	{
		std::cout << "File name: " << name << std::endl;
		std::cout << "Is directory: " << isDirectory << std::endl;
	}
};

class FileDecompress
{
public:
	std::string path;
	bool isDirectory;
	std::vector<FileSource> files;

	FileDecompress() : path(""), isDirectory(false) {}
	FileDecompress(const std::string& p) : path(p){}

	void scanFolder(const fs::path& path);
	void read();
	bool write();
	void printInfomation() const
	{
		std::cout << "Path: " << path << std::endl;
		std::cout << "Is Directory: " << isDirectory << std::endl;
		std::cout << "Files: " << std::endl;
		for (const auto& file : files)
		{
			std::cout << "Name: " << file.name << "\t\t";
			std::cout << "Is Directory: " << file.isDirectory << "\t\t";
			std::cout << "Size: " << file.data.size() << std::endl;
		}
	}
};

FileCompress transmission(const FileDecompress& fileDecompress);
FileDecompress transmission(const FileCompress& fileCompress);
FileEntry transmission(const FileSource& fileSource);
FileSource transmission(const FileEntry& fileEntry);
