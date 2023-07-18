#pragma once

class BaseAllocator
{
public:
	static void* Allocate(unsigned int size);
	static void Release(void* ptr);
};

class StompAllocator
{
private:
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Allocate(unsigned int size);
	static void Release(void* ptr);
};

class PoolAllocator
{
public:
	static void* Allocate(unsigned int size);
	static void Release(void* ptr);
};