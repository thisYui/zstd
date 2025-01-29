#pragma once
#include "file_io.h"
#include "lz77.h"
#include "huffman.h"

namespace zstd
{
	std::vector<uint8_t> dataToLz77(
		const std::vector<uint8_t>& data, 
		uint16_t& bufferLz77
	);
	std::vector<uint8_t> lz77ToData(
		const std::vector<uint8_t>& data, 
		uint16_t& bufferLz77
	);

	std::vector<uint8_t> lz77ToHuffman(
		const std::vector<uint8_t>& data, 
		uint32_t& originalSize,
		std::vector<uint8_t>& trans
	);
	std::vector<uint8_t> huffmanToLz77(
		const std::vector<uint8_t>& data,
		HuffmanNode* root,
		uint32_t& originalSize
	);

	bool compress(const std::string& inputFileName);
	bool decompress(const std::string& inputFileName);
}