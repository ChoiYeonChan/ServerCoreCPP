#pragma once

class MemoryStream
{
protected:
	static const int DEFAULT_BUF_SIZE = 1024;

protected:
	char* buffer_;
	unsigned int front_;
	unsigned int rear_;
	unsigned int capacity_;
	unsigned int length_;
	unsigned int space_;

	bool is_allocated_;
	bool is_open_;

	BufferChunkRef owner_ = nullptr;

public:
	MemoryStream();
	MemoryStream(unsigned int buf_size, bool pooling = false);
	MemoryStream(char* in_buffer, unsigned int in_buf_size, unsigned int buf_size = DEFAULT_BUF_SIZE, bool copy = false);

	virtual ~MemoryStream();

	// Buffer Pooling
	void Open(int size);
	void Close();

	// Read & Write
	bool OnRead(const unsigned int out_byte_count);
	bool OnWrite(const unsigned int in_byte_count);

	void CleanUp();

	// Getter & Setter
	char* GetBufferFront() const { return buffer_ + front_; }
	char* GetBufferRear() const { return buffer_ + rear_; }
	unsigned int GetCapacity() const { return capacity_; }
	unsigned int GetLength() const { return length_; }
	unsigned int GetSpace() const { return space_; }

	char* GetDataSegment();

	// temp
	void Display();

private:
	bool AllocBuffer(const unsigned int buf_size);
	void SetBuffer(char* buffer, const unsigned int in_buf_size, const unsigned int buf_size);
};

/**********************
*     StreamReader
***********************/

class StreamReader
{
private:
	MemoryStream* stream_;

public:
	StreamReader(MemoryStream* stream) : stream_(stream) { }

	template <typename T>
	bool Read(T* dest) { return Read(dest, sizeof(T)); }
	bool Read(void* out_buffer, const unsigned int out_byte_count);
	bool Peek(void* out_buffer, const unsigned int out_byte_count);

	template <typename T>
	StreamReader& operator>>(T& dest);
};

template <typename T>
StreamReader& StreamReader::operator>>(T& dest)
{
	Read<T>(&dest);
	return *this;
}

/**********************
*     StreamWriter
***********************/

class StreamWriter
{
private:
	MemoryStream* stream_;

public:
	StreamWriter(MemoryStream* stream) : stream_(stream) { }

	template <typename T>
	bool Write(const T* source) { return Write(source, sizeof(T)); }
	bool Write(const void* in_buffer, const unsigned int in_byte_count);

	template <typename T>
	T* Reserve(int count);

	template <typename T>
	StreamWriter& operator<<(T&& source);
};

template <typename T>
T* StreamWriter::Reserve(int count)
{
	T* ptr = reinterpret_cast<T*>(stream_->GetBufferRear());
	if (stream_->OnWrite(sizeof(T) * count))
	{
		return ptr;
	}
	else
	{
		return nullptr;
	}
}

template <typename T>
StreamWriter& StreamWriter::operator<<(T&& source)
{
	Write<T>(&source);
	return *this;
}

/************************
*	  Buffer Pooling
*************************/

class BufferChunk : public enable_shared_from_this<BufferChunk>
{
	enum
	{
		BUFFER_CHUNK_SIZE = 819200
	};

private:
	std::array<char, BUFFER_CHUNK_SIZE> buffer_ = {};
	bool is_open_ = false;
	int used_size_ = 0;

public:
	BufferChunk() = default;
	~BufferChunk() = default;

	void Reset();
	char* Open(int alloc_size);
	void Close(int write_size);

	char* Buffer() { return &buffer_[used_size_]; }
	int FreeSize() { return buffer_.size() - used_size_; }
	bool IsOpen() { return is_open_; }
};

class BufferManager
{
private:
	CRITICAL_SECTION lock_;
	std::vector<BufferChunkRef> buffer_chunk_list_;

public:
	BufferManager();

	BufferChunkRef Pop();
	void Push(BufferChunkRef buffer);

	static void PushGlobal(BufferChunk* buffer);
};