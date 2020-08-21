#pragma once

#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)
#define BITSTREAM_STACK_ALLOCATION_SIZE 256

class BitStream
{
public:
	BitStream();
	BitStream(int initialBytesToAllocate);
	BitStream(unsigned char* _data, unsigned int lengthInBytes, bool _copyData);
	~BitStream();
	void Reset(void);
	template <class templateType>
	bool Serialize(bool writeToBitstream, templateType &var);
	template <class templateType>
	bool SerializeDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue);
	template <class templateType>
	bool SerializeDelta(bool writeToBitstream, templateType &currentValue);
	template <class templateType>
	bool SerializeCompressed(bool writeToBitstream, templateType &var);
	template <class templateType>
	bool SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue);
	template <class templateType>
	bool SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue);
	bool Serialize(bool writeToBitstream, char* input, const int numberOfBytes);
	template <class templateType>
	bool SerializeNormVector(bool writeToBitstream, templateType &x, templateType &y, templateType &z);
	template <class templateType>
	bool SerializeVector(bool writeToBitstream, templateType &x, templateType &y, templateType &z);
	template <class templateType>
	bool SerializeNormQuat(bool writeToBitstream, templateType &w, templateType &x, templateType &y, templateType &z);
	template <class templateType>
	bool SerializeOrthMatrix(bool writeToBitstream, templateType &m00, templateType &m01, templateType &m02, templateType &m10, templateType &m11, templateType &m12, templateType &m20, templateType &m21, templateType &m22);

	bool SerializeBits(bool writeToBitstream, unsigned char* input, int numberOfBitsToSerialize, const bool rightAlignedBits = true);

	template <class templateType>
	void Write(templateType var);
	template <class templateType>
	void WriteDelta(templateType currentValue, templateType lastValue);
	template <class templateType>
	void WriteDelta(templateType currentValue);
	template <class templateType>
	void WriteCompressed(templateType var);
	template <class templateType>
	void WriteCompressedDelta(templateType currentValue, templateType lastValue);
	template <class templateType>
	void WriteCompressedDelta(templateType currentValue);
	template <class templateType>
	bool Read(templateType &var);
	template <class templateType>
	bool ReadDelta(templateType &var);
	template <class templateType>
	bool ReadCompressed(templateType &var);
	template <class templateType>
	bool ReadCompressedDelta(templateType &var);
	void Write(const char* input, const int numberOfBytes);
	void Write(BitStream *bitStream, int numberOfBits);
	void Write(BitStream *bitStream);
	template <class templateType>
	void WriteNormVector(templateType x, templateType y, templateType z);
	template <class templateType>
	void WriteVector(templateType x, templateType y, templateType z);
	template <class templateType>
	void WriteNormQuat(templateType w, templateType x, templateType y, templateType z);
	template <class templateType>
	void WriteOrthMatrix(templateType m00, templateType m01, templateType m02, templateType m10, templateType m11, templateType m12, templateType m20, templateType m21, templateType m22);
	bool Read(char* output, const int numberOfBytes);
	void ResetReadPointer(void);
	void ResetWritePointer(void);
	void AssertStreamEmpty(void);
	void IgnoreBits(const int numberOfBits);
	void SetWriteOffset(const int offset);
	inline int GetNumberOfBitsUsed(void) const { return GetWriteOffset(); }
	inline int GetWriteOffset(void) const { return numberOfBitsUsed; }
	inline int GetNumberOfBytesUsed(void) const { return BITS_TO_BYTES(numberOfBitsUsed); }
	inline int GetReadOffset(void) const { return readOffset; }
	inline void SetReadOffset(int newReadOffset) { readOffset = newReadOffset; }
	inline int GetNumberOfUnreadBits(void) const { return numberOfBitsUsed - readOffset; }
	int CopyData(unsigned char** _data) const;
	void SetData(unsigned char *input);
	inline unsigned char* GetData(void) const { return data; }
	void WriteBits(const unsigned char* input, int numberOfBitsToWrite, const bool rightAlignedBits = true);
	void WriteAlignedBytes(const unsigned char *input, const int numberOfBytesToWrite);
	bool ReadAlignedBytes(unsigned char *output, const int numberOfBytesToRead);
	void AlignWriteToByteBoundary(void);
	void AlignReadToByteBoundary(void);
	bool ReadBits(unsigned char *output, int numberOfBitsToRead, const bool alignBitsToRight = true);

	void Write0(void);
	void Write1(void);
	bool ReadBit(void);
	void AssertCopyData(void);
	void SetNumberOfBitsAllocated(const unsigned int lengthInBits);
	void AddBitsAndReallocate(const int numberOfBitsToWrite);

private:
	void WriteCompressed(const unsigned char* input, const int size, const bool unsignedData);
	bool ReadCompressed(unsigned char* output, const int size, const bool unsignedData);
	void ReverseBytes(unsigned char *input, unsigned char *output, int length);
	int numberOfBitsUsed;
	int numberOfBitsAllocated;
	int readOffset;
	unsigned char *data;
	bool copyData;
	unsigned char stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
};

template <class templateType>
inline bool BitStream::Serialize(bool writeToBitstream, templateType &var)
{
	if (writeToBitstream)
		Write(var);
	else return Read(var);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue)
{
	if (writeToBitstream)
		WriteDelta(currentValue, lastValue);
	else return ReadDelta(currentValue);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType &currentValue)
{
	if (writeToBitstream)
		WriteDelta(currentValue);
	else return ReadDelta(currentValue);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeCompressed(bool writeToBitstream, templateType &var)
{
	if (writeToBitstream)
		WriteCompressed(var);
	else return ReadCompressed(var);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue)
{
	if (writeToBitstream)
		WriteCompressedDelta(currentValue, lastValue);
	else return ReadCompressedDelta(currentValue);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue)
{
	if (writeToBitstream)
		WriteCompressedDelta(currentValue);
	else return ReadCompressedDelta(currentValue);
	return true;
}

inline bool BitStream::Serialize(bool writeToBitstream, char* input, const int numberOfBytes)
{
	if (writeToBitstream)
		Write(input, numberOfBytes);
	else return Read(input, numberOfBytes);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeNormVector(bool writeToBitstream, templateType &x, templateType &y, templateType &z)
{
	if (writeToBitstream)
		WriteNormVector(x, y, z);
	else return ReadNormVector(x, y, z);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeVector(bool writeToBitstream, templateType &x, templateType &y, templateType &z)
{
	if (writeToBitstream)
		WriteVector(x, y, z);
	else return ReadVector(x, y, z);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeNormQuat(bool writeToBitstream, templateType &w, templateType &x, templateType &y, templateType &z)
{
	if (writeToBitstream)
		WriteNormQuat(w, x, y, z);
	else return ReadNormQuat(w, x, y, z);
	return true;
}

template <class templateType>
inline bool BitStream::SerializeOrthMatrix(
	bool writeToBitstream,
	templateType &m00, templateType &m01, templateType &m02,
	templateType &m10, templateType &m11, templateType &m12,
	templateType &m20, templateType &m21, templateType &m22)
{
	if (writeToBitstream)
		WriteOrthMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
	else return ReadOrthMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
	return true;
}

inline bool BitStream::SerializeBits(bool writeToBitstream, unsigned char* input, int numberOfBitsToSerialize, const bool rightAlignedBits)
{
	if (writeToBitstream)
		WriteBits(input, numberOfBitsToSerialize, rightAlignedBits);
	else return ReadBits(input, numberOfBitsToSerialize, rightAlignedBits);
	return true;
}

template <class templateType>
inline void BitStream::Write(templateType var)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
	WriteBits((unsigned char*) & var, sizeof(templateType) * 8, true);
}

template <>
inline void BitStream::Write(bool var)
{
	if (var)
		Write1();
	else Write0();
}

template <class templateType>
inline void BitStream::WriteCompressed(templateType var)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
	WriteCompressed((unsigned char*) &var, sizeof(templateType) * 8, true);
}

template <>
inline void BitStream::WriteCompressed(bool var)
{
	Write(var);
}

template <>
inline void BitStream::WriteCompressed(float var)
{
	if (var < -1.0f)
		var = -1.0f;

	if (var > 1.0f)
		var = 1.0f;

	Write((unsigned short) ((var + 1.0f)*32767.5f));
}

template <>
inline void BitStream::WriteCompressed(double var)
{
	if (var < -1.0f)
		var = -1.0f;

	if (var > 1.0f)
		var = 1.0f;

	Write((unsigned long) ((var + 1.0)*2147483648.0));
}

template <class templateType>
inline void BitStream::WriteCompressedDelta(templateType currentValue, templateType lastValue)
{
	if (currentValue == lastValue)
	{
		Write(false);
	}
	else
	{
		Write(true);
		WriteCompressed(currentValue);
	}
}

template <>
inline void BitStream::WriteCompressedDelta(bool currentValue, bool lastValue)
{
#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif
	Write(currentValue);
}

template <class templateType>
inline void BitStream::WriteCompressedDelta(templateType currentValue)
{
	Write(true);
	WriteCompressed(currentValue);
}

template <>
inline void BitStream::WriteCompressedDelta(bool currentValue)
{
	Write(currentValue);
}

template <class templateType>
inline bool BitStream::Read(templateType &var)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
	return ReadBits((unsigned char*) &var, sizeof(templateType) * 8, true);
}

template <>
inline bool BitStream::Read(bool &var)
{
	if (readOffset + 1 > numberOfBitsUsed)
		return false;

	if (data[readOffset >> 3] & (0x80 >> (readOffset++ % 8)))
		var = true;
	else var = false;

	return true;
}

template <class templateType>
inline bool BitStream::ReadDelta(templateType &var)
{
	bool dataWritten;
	bool success = Read(dataWritten);
	if (dataWritten)
		success = Read(var);
	return success;
}

template <>
inline bool BitStream::ReadDelta(bool &var)
{
	return Read(var);
}

template <class templateType>
inline bool BitStream::ReadCompressed(templateType &var)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif
	return ReadCompressed((unsigned char*) &var, sizeof(templateType) * 8, true);
}

template <>
inline bool BitStream::ReadCompressed(bool &var)
{
	return Read(var);
}

template <>
inline bool BitStream::ReadCompressed(float &var)
{
	unsigned short compressedFloat;
	if (Read(compressedFloat))
	{
		var = ((float) compressedFloat / 32767.5f - 1.0f);
		return true;
	}
	return false;
}

template <>
inline bool BitStream::ReadCompressed(double &var)
{
	unsigned long compressedFloat;
	if (Read(compressedFloat))
	{
		var = ((double) compressedFloat / 2147483648.0 - 1.0);
		return true;
	}
	return false;
}

template <class templateType>
inline bool BitStream::ReadCompressedDelta(templateType &var)
{
	bool dataWritten;
	bool success = Read(dataWritten);
	if (dataWritten)
		success = ReadCompressed(var);
	return success;
}

template <>
inline bool BitStream::ReadCompressedDelta(bool &var)
{
	return Read(var);
}