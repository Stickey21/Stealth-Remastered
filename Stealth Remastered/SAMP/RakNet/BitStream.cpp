#include "main.h"

BitStream::BitStream()
{
	numberOfBitsUsed = 0;
	numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
	readOffset = 0;
	data = (unsigned char*) stackData;
	copyData = true;
}

BitStream::BitStream(int initialBytesToAllocate)
{
	numberOfBitsUsed = 0;
	readOffset = 0;
	if (initialBytesToAllocate <= BITSTREAM_STACK_ALLOCATION_SIZE)
	{
		data = (unsigned char*) stackData;
		numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
	}
	else
	{
		data = (unsigned char*) malloc(initialBytesToAllocate);
		numberOfBitsAllocated = initialBytesToAllocate << 3;
	}
	copyData = true;
}

BitStream::BitStream(unsigned char* _data, unsigned int lengthInBytes, bool _copyData)
{
	numberOfBitsUsed = lengthInBytes << 3;
	readOffset = 0;
	copyData = _copyData;
	numberOfBitsAllocated = lengthInBytes << 3;

	if (copyData)
	{
		if (lengthInBytes > 0)
		{
			if (lengthInBytes < BITSTREAM_STACK_ALLOCATION_SIZE)
			{
				data = (unsigned char*) stackData;
				numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE << 3;
			}
			else data = (unsigned char*) malloc(lengthInBytes);

			memcpy(data, _data, lengthInBytes);
		}
		else data = 0;
	}
	else data = (unsigned char*) _data;
}

void BitStream::SetNumberOfBitsAllocated(const unsigned int lengthInBits)
{
	numberOfBitsAllocated = lengthInBits;
}

BitStream::~BitStream()
{
	if (copyData && numberOfBitsAllocated > BITSTREAM_STACK_ALLOCATION_SIZE << 3)
		free(data);
}

void BitStream::Reset(void)
{
	numberOfBitsUsed = 0;
	readOffset = 0;
}

void BitStream::Write(const char* input, const int numberOfBytes)
{
	if (numberOfBytes == 0)
		return;

	if ((numberOfBitsUsed & 7) == 0)
	{
		AddBitsAndReallocate(BYTES_TO_BITS(numberOfBytes));
		memcpy(data + BITS_TO_BYTES(numberOfBitsUsed), input, numberOfBytes);
		numberOfBitsUsed += BYTES_TO_BITS(numberOfBytes);
	}
	else WriteBits((unsigned char*) input, numberOfBytes * 8, true);

}
void BitStream::Write(BitStream *bitStream)
{
	Write(bitStream, bitStream->GetNumberOfBitsUsed());
}
void BitStream::Write(BitStream *bitStream, int numberOfBits)
{
	AddBitsAndReallocate(numberOfBits);
	int numberOfBitsMod8;

	while (numberOfBits-- > 0 && bitStream->readOffset + 1 <= bitStream->numberOfBitsUsed)
	{
		numberOfBitsMod8 = numberOfBitsUsed & 7;
		if (numberOfBitsMod8 == 0)
		{
			if (bitStream->data[bitStream->readOffset >> 3] & (0x80 >> (bitStream->readOffset++ % 8)))
				data[numberOfBitsUsed >> 3] = 0x80;
			else data[numberOfBitsUsed >> 3] = 0;
		}
		else if (bitStream->data[bitStream->readOffset >> 3] & (0x80 >> (bitStream->readOffset++ % 8)))
			data[numberOfBitsUsed >> 3] |= 0x80 >> (numberOfBitsMod8);

		numberOfBitsUsed++;
	}
}

bool BitStream::Read(char* output, const int numberOfBytes)
{
	if ((readOffset & 7) == 0)
	{
		if (readOffset + (numberOfBytes << 3) > numberOfBitsUsed)
			return false;

		memcpy(output, data + (readOffset >> 3), numberOfBytes);

		readOffset += numberOfBytes << 3;
		return true;
	}
	else return ReadBits((unsigned char*) output, numberOfBytes * 8);
}

void BitStream::ResetReadPointer(void)
{
	readOffset = 0;
}

void BitStream::ResetWritePointer(void)
{
	numberOfBitsUsed = 0;
}

void BitStream::Write0(void)
{
	AddBitsAndReallocate(1);

	if ((numberOfBitsUsed & 7) == 0)
		data[numberOfBitsUsed >> 3] = 0;

	numberOfBitsUsed++;
}

void BitStream::Write1(void)
{
	AddBitsAndReallocate(1);

	int numberOfBitsMod8 = numberOfBitsUsed & 7;

	if (numberOfBitsMod8 == 0)
		data[numberOfBitsUsed >> 3] = 0x80;
	else data[numberOfBitsUsed >> 3] |= 0x80 >> (numberOfBitsMod8);

	numberOfBitsUsed++;
}

#ifdef _MSC_VER
#pragma warning( disable : 4800 )
#endif

bool BitStream::ReadBit(void)
{
	return (bool) (data[readOffset >> 3] & (0x80 >> (readOffset++ & 7)));
}

void BitStream::WriteAlignedBytes(const unsigned char* input,
	const int numberOfBytesToWrite)
{
	AlignWriteToByteBoundary();
	Write((const char*) input, numberOfBytesToWrite);
}

bool BitStream::ReadAlignedBytes(unsigned char* output, const int numberOfBytesToRead)
{
	if (numberOfBytesToRead <= 0)
		return false;

	AlignReadToByteBoundary();

	if (readOffset + (numberOfBytesToRead << 3) > numberOfBitsUsed)
		return false;

	memcpy(output, data + (readOffset >> 3), numberOfBytesToRead);

	readOffset += numberOfBytesToRead << 3;

	return true;
}

void BitStream::AlignWriteToByteBoundary(void)
{
	if (numberOfBitsUsed)
		numberOfBitsUsed += 8 - (((numberOfBitsUsed - 1) & 7) + 1);
}

void BitStream::AlignReadToByteBoundary(void)
{
	if (readOffset)
		readOffset += 8 - (((readOffset - 1) & 7) + 1);
}

void BitStream::WriteBits(const unsigned char *input, int numberOfBitsToWrite, const bool rightAlignedBits)
{
	if (numberOfBitsToWrite <= 0)
		return;

	AddBitsAndReallocate(numberOfBitsToWrite);
	int offset = 0;
	unsigned char dataByte;
	int numberOfBitsUsedMod8;

	numberOfBitsUsedMod8 = numberOfBitsUsed & 7;

	while (numberOfBitsToWrite > 0)
	{
		dataByte = *(input + offset);

		if (numberOfBitsToWrite < 8 && rightAlignedBits)
			dataByte <<= 8 - numberOfBitsToWrite;

		if (numberOfBitsUsedMod8 == 0)
			* (data + (numberOfBitsUsed >> 3)) = dataByte;
		else
		{
			*(data + (numberOfBitsUsed >> 3)) |= dataByte >> (numberOfBitsUsedMod8);

			if (8 - (numberOfBitsUsedMod8) < 8 && 8 - (numberOfBitsUsedMod8) < numberOfBitsToWrite)
			{
				*(data + (numberOfBitsUsed >> 3) + 1) = (unsigned char) (dataByte << (8 - (numberOfBitsUsedMod8)));
			}
		}

		if (numberOfBitsToWrite >= 8)
			numberOfBitsUsed += 8;
		else numberOfBitsUsed += numberOfBitsToWrite;

		numberOfBitsToWrite -= 8;

		offset++;
	}
}

void BitStream::SetData(unsigned char *input)
{
	data = input;
	copyData = false;
}

void BitStream::WriteCompressed(const unsigned char* input,
	const int size, const bool unsignedData)
{
	int currentByte = (size >> 3) - 1;

	unsigned char byteMatch;

	if (unsignedData)
		byteMatch = 0;
	else byteMatch = 0xFF;

	while (currentByte > 0)
	{
		if (input[currentByte] == byteMatch)
		{
			bool b = true;
			Write(b);
		}
		else
		{
			bool b = false;
			Write(b);

			WriteBits(input, (currentByte + 1) << 3, true);
			return;
		}

		currentByte--;
	}

	if ((unsignedData && ((*(input + currentByte)) & 0xF0) == 0x00) ||
		(unsignedData == false && ((*(input + currentByte)) & 0xF0) == 0xF0))
	{
		bool b = true;
		Write(b);
		WriteBits(input + currentByte, 4, true);
	}

	else
	{
		bool b = false;
		Write(b);
		WriteBits(input + currentByte, 8, true);
	}
}

bool BitStream::ReadBits(unsigned char* output, int numberOfBitsToRead, const bool alignBitsToRight)
{
	if (numberOfBitsToRead <= 0)
		return false;

	if (readOffset + numberOfBitsToRead > numberOfBitsUsed)
		return false;

	int readOffsetMod8;

	int offset = 0;

	memset(output, 0, BITS_TO_BYTES(numberOfBitsToRead));

	readOffsetMod8 = readOffset & 7;

	while (numberOfBitsToRead > 0)
	{
		*(output + offset) |= *(data + (readOffset >> 3)) << (readOffsetMod8);

		if (readOffsetMod8 > 0 && numberOfBitsToRead > 8 - (readOffsetMod8))
			*(output + offset) |= *(data + (readOffset >> 3) + 1) >> (8 - (readOffsetMod8));

		numberOfBitsToRead -= 8;

		if (numberOfBitsToRead < 0)
		{

			if (alignBitsToRight)
				* (output + offset) >>= -numberOfBitsToRead;

			readOffset += 8 + numberOfBitsToRead;
		}
		else readOffset += 8;

		offset++;

	}

	return true;
}

bool BitStream::ReadCompressed(unsigned char* output, const int size, const bool unsignedData)
{
	int currentByte = (size >> 3) - 1;

	unsigned char byteMatch, halfByteMatch;

	if (unsignedData)
	{
		byteMatch = 0;
		halfByteMatch = 0;
	}
	else
	{
		byteMatch = 0xFF;
		halfByteMatch = 0xF0;
	}

	while (currentByte > 0)
	{
		bool b;

		if (Read(b) == false)
			return false;

		if (b)
		{
			output[currentByte] = byteMatch;
			currentByte--;
		}
		else
		{
			if (ReadBits(output, (currentByte + 1) << 3) == false)
				return false;

			return true;
		}
	}

	if (readOffset + 1 > numberOfBitsUsed)
		return false;

	bool b;

	if (Read(b) == false)
		return false;

	if (b)
	{
		if (ReadBits(output + currentByte, 4) == false)
			return false;

		output[currentByte] |= halfByteMatch;
	}
	else
	{
		if (ReadBits(output + currentByte, 8) == false)
			return false;
	}

	return true;
}

void BitStream::AddBitsAndReallocate(const int numberOfBitsToWrite)
{
	if (numberOfBitsToWrite <= 0)
		return;

	int newNumberOfBitsAllocated = numberOfBitsToWrite + numberOfBitsUsed;

	if (numberOfBitsToWrite + numberOfBitsUsed > 0 && ((numberOfBitsAllocated - 1) >> 3) < ((newNumberOfBitsAllocated - 1) >> 3))
	{
		newNumberOfBitsAllocated = (numberOfBitsToWrite + numberOfBitsUsed) * 2;

		int amountToAllocate = BITS_TO_BYTES(newNumberOfBitsAllocated);
		if (data == (unsigned char*) stackData)
		{
			if (amountToAllocate > BITSTREAM_STACK_ALLOCATION_SIZE)
			{
				data = (unsigned char*) malloc(amountToAllocate);
				memcpy((void *) data, (void *) stackData, BITS_TO_BYTES(numberOfBitsAllocated));
			}
		}
		else
		{
			data = (unsigned char*) realloc(data, amountToAllocate);
		}
	}

	if (newNumberOfBitsAllocated > numberOfBitsAllocated)
		numberOfBitsAllocated = newNumberOfBitsAllocated;
}

int BitStream::CopyData(unsigned char** _data) const
{
	*_data = new unsigned char[BITS_TO_BYTES(numberOfBitsUsed)];
	memcpy(*_data, data, sizeof(unsigned char) * (BITS_TO_BYTES(numberOfBitsUsed)));
	return numberOfBitsUsed;
}

void BitStream::IgnoreBits(const int numberOfBits)
{
	readOffset += numberOfBits;
}

void BitStream::SetWriteOffset(const int offset)
{
	numberOfBitsUsed = offset;
}

void BitStream::AssertCopyData(void)
{
	if (copyData == false)
	{
		copyData = true;

		if (numberOfBitsAllocated > 0)
		{
			unsigned char * newdata = (unsigned char*) malloc(BITS_TO_BYTES(numberOfBitsAllocated));

			memcpy(newdata, data, BITS_TO_BYTES(numberOfBitsAllocated));
			data = newdata;
		}

		else data = 0;
	}
}
void BitStream::ReverseBytes(unsigned char *input, unsigned char *output, int length)
{
	for (int i = 0; i < length; i++)
		output[i] = input[length - i - 1];
}
