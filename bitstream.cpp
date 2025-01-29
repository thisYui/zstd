#include "bitstream.h"

void turnBitOn(uint8_t& byte, uint8_t bitPosition)
{
	byte |= (1 << bitPosition);
}

void turnBitOff(uint8_t& byte, uint8_t bitPosition)
{
	byte &= ~(1 << bitPosition);
}

void BitStream::setData(const std::vector<uint8_t>& data)
{
	this->data = data;
}

std::vector<uint8_t> BitStream::getData()
{
	return this->data;
}

uint8_t BitStream::getBits()
{
	
	if (bitPosition == 8)
	{
		bitPosition = 0;
		bytePosition++;
	}

	uint8_t bit = data[bytePosition - 1] & (1 << bitPosition);  // Lấy bit tại vị trí bitPosition
	bitPosition++;  // bitPosition lên 1
	return (bit != 0x0) ? 0x1 : 0x0;
}

void BitStream::pushBit(const uint8_t& byte, const std::vector<uint8_t>& codes)
{
	for (uint8_t bit : codes)
	{
		
		if (bitPosition == 8)
		{
			bitPosition = 0;
			bytePosition++;
			data.push_back(0x0);  // Thêm một byte mới vào data (mặc định là 0)
		}

		if (bit == 1)
		{
			turnBitOn(data.back(), bitPosition);  // Bật bit tại vị trí bitPosition và tăng bitPosition lên 1
		}
		// Các bit còn lại mặt định là 0 không cần tắt
		bitPosition++;
	}
}
