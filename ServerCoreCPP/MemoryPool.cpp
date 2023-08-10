#include "pch.h"
#include "MemoryPool.h"

/************************
	   LockFreeStack
*************************/

mymp::LockFreeStack::LockFreeStack() : size(0)
{
	top = (StampNode*)_aligned_malloc(sizeof(StampNode), 16);
	if (top == nullptr)
	{
		std::cout << "�޸� �Ҵ翡 �����߽��ϴ�. (top)" << std::endl;
		return;
	}

	top->ptr = nullptr;
	top->stamp = 0;
}

mymp::LockFreeStack::~LockFreeStack()
{
	Clear();
}

void mymp::LockFreeStack::Push(BlockHeader* node)
{
	while (true)
	{
		StampNode oldTop;
		oldTop.ptr = top->ptr;
		oldTop.stamp = top->stamp;

		node->next = oldTop.ptr;

		if (InterlockedCompareExchange128((LONG64*)top, oldTop.stamp + 1, (LONG64)node, (LONG64*)&oldTop))
		{
			InterlockedIncrement(&size);
			break;
		}
	}
}

bool mymp::LockFreeStack::Pop(BlockHeader*& node)
{
	while (true)
	{
		StampNode oldTop;
		oldTop.ptr = top->ptr;
		oldTop.stamp = top->stamp;
		if (oldTop.ptr == nullptr)
		{
			return false;
		}

		if (InterlockedCompareExchange128((LONG64*)top, oldTop.stamp + 1, (LONG64)oldTop.ptr->next, (LONG64*)&oldTop))
		{
			InterlockedDecrement(&size);
			node = oldTop.ptr;

			return true;
		}
	}
}

void mymp::LockFreeStack::Clear()
{
	BlockHeader* ptr = nullptr;

	while (top->ptr != nullptr)
	{
		ptr = top->ptr;
		top->ptr = top->ptr->next;
		_aligned_free(ptr);

		InterlockedDecrement(&size);
	}

	top->ptr = nullptr;
	top->stamp = 0;

	std::cout << "LockFreeStack Size After Clear : " << size << std::endl;
}

/**********************
	   MemoryPool
***********************/

void mymp::MemoryPool::Push(BlockHeader* header)
{
	if (header->code != BlockHeader::CURRENT_ALLOC)
	{
		std::cout << "[MemoryPool] �߸��� �޸� �����Դϴ�." << std::endl;
		return;
	}
	else
	{
		if (InterlockedCompareExchange16(&header->code, BlockHeader::CURRENT_RELEASE, BlockHeader::CURRENT_ALLOC) != BlockHeader::CURRENT_ALLOC)
		{
			std::cout << "[MemoryPool] �ߺ� �޸� ���� �õ��Դϴ�." << std::endl;
			return;
		}
		else
		{
			container_.Push(header);
			++release_count_;
			--alloc_count_;
		}
	}
}

void mymp::MemoryPool::Pop(BlockHeader*& header)
{
	if (container_.Pop(header) == false)
	{
		header = (BlockHeader*)_aligned_malloc(alloc_size_ + sizeof(mymp::BlockHeader), 16);
		if (header == nullptr)
		{
			std::cout << "[MemoryPool] �޸� �Ҵ翡 �����ϼ̽��ϴ�." << std::endl;
			return;
		}
	}
	else
	{
		if (header->code != BlockHeader::CURRENT_RELEASE)
		{
			std::cout << "[MemoryPool] �޸�Ǯ�� �����Ǿ����ϴ�." << std::endl;
			// CRASH
			return;
		}

		--release_count_;
	}

	++alloc_count_;
}