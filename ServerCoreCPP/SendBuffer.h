#pragma once
#include "MemoryStream.h"

class SendBuffer : public MemoryStream
{
public:
	SendBuffer() = default;
	SendBuffer(int size) : MemoryStream(size, true) { }
	~SendBuffer() = default;	
};