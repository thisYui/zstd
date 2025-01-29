#pragma once
#include "bitstream.h"
#include <queue>

// Cấu trúc của một nút trong cây Huffman
struct HuffmanNode 
{
	uint8_t byte;  // Dữ liệu của nút
	bool isLeaf;  // Kiểm tra xem nút có phải là lá không
	int frequency;  // Tần suất xuất hiện của dữ liệu
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(uint8_t b, bool isL, int freq) : 
        byte(b), isLeaf(isL), frequency(freq), left(nullptr), right(nullptr) {}

    bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs) const
    {
        return lhs->frequency > rhs->frequency; // Min-heap dựa trên tần suất
    }
};

// Cấu trúc để quản lý cây Huffman
struct HuffmanTree
{
	HuffmanNode* root;  // Nút gốc của cây
	std::unordered_map<uint8_t, std::vector<uint8_t>> codes;  // Bảng mã Huffman

    HuffmanTree() : root(nullptr) {}
    ~HuffmanTree();
};

namespace huffman
{
    const uint8_t TOLEFT = 0;
	const uint8_t TORIGHT = 1;

    // Tạo cây Huffman từ bảng tần suất
    HuffmanNode* buildHuffmanTree(const std::unordered_map<uint8_t, int>& frequencyTable);

    // Tạo bảng mã Huffman cho các ký tự
    void generateCodes(
        HuffmanNode* root, 
        const std::vector<uint8_t>& code, 
        std::unordered_map<uint8_t, 
        std::vector<uint8_t>>& codes
    );

    // Mã hóa dữ liệu
    std::vector<uint8_t> encode(
        const std::vector<uint8_t>& data,
        std::vector<uint8_t>& trans
    );

    // Giải mã dữ liệu
    std::vector<uint8_t> decode(
        const std::vector<uint8_t>& encodedData, 
        HuffmanNode* root, 
        const uint32_t origanalSize
    );

    // Tạo bảng tần suất từ một chuỗi
    std::unordered_map<uint8_t, int> buildFrequencyTable(const std::vector<uint8_t>& data);

	// Ghi cây dưới dạng vector<uint8_t>
	std::vector<uint8_t> writeTree(HuffmanNode* root);

	// Đọc cây từ vector<uint8_t>
	HuffmanNode* readTree(const std::vector<uint8_t>& treeData);
}