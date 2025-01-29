#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>

// Lớp bitstream dùng để đọc và ghi dữ liệu bit
class BitStream
{
public:
	std::vector<uint8_t> data;  // Dữ liệu cần đọc hoặc ghi
	uint8_t bitPosition;  // Vị trí bit hiện tại từ 0 -> 7
	uint32_t bytePosition;
	uint32_t originalSize;

	BitStream(uint32_t ori) : bitPosition(8), bytePosition(0), originalSize(ori) {}
	void setData(const std::vector<uint8_t>& data);
	std::vector<uint8_t> getData();
	uint8_t getBits();
	void pushBit(const uint8_t& byte, const std::vector<uint8_t>& codes);
};

// Bật tắt bit tại vị trí bitPosition
void turnBitOn(uint8_t& byte, uint8_t bitPosition);
void turnBitOff(uint8_t& byte, uint8_t bitPosition);