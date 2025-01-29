#pragma once
#include <vector>
#include <tuple>
#include <cstdint> // uint8_t

namespace lz77
{
    const int SEARCH_BUFFER_SIZE = 32 * 1024;
    const int LOOKAHEAD_BUFFER_SIZE = 32;
    const int BLOCK_SIZE = 32 * 1024;

    // Nén dữ liệu
    std::vector<std::tuple<int, int, uint8_t>> compress(
        const std::vector<uint8_t>& input,
        int searchBufferSize = SEARCH_BUFFER_SIZE,
		int lookaheadBufferSize = LOOKAHEAD_BUFFER_SIZE);

    // Giải nén dữ liệu
    std::vector<uint8_t> decompress(
        const std::vector<std::tuple<int, int, uint8_t>>& compressedData);

    // Chia dữ liệu thành các block nhỏ
    std::vector<std::vector<uint8_t>> splitIntoBlocks(
        const std::vector<uint8_t>& input,
		size_t blockSize = BLOCK_SIZE);

    // Lấy lại kết quả của từng block dựa trên vector tổng
    std::vector<std::vector<std::tuple<int, int, uint8_t>>> getBlockData(
        const std::vector<std::tuple<int, int, uint8_t>>& lz77);

    // Tìm kiếm chuỗi trùng lặp và tạo tuple (offset, length, nextByte)
    std::tuple<int, int, uint8_t> createMatch(
        const std::vector<uint8_t>& searchBuffer,
        const std::vector<uint8_t>& lookaheadBuffer);

    // Kiểm tra tính hợp lệ của đầu vào
    bool validateInput(const std::vector<uint8_t>& input);

	// Nối các tuple thành một vector<uint8_t>
    std::vector<uint8_t> concatTuple(const std::vector<std::tuple<int, int, uint8_t>>& data);

	// Tách vector<uint8_t> thành các tuple
    std::vector<std::tuple<int, int, uint8_t>> unconcatTuple(const std::vector<uint8_t>& data);
}
