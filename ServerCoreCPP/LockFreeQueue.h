#pragma once
#include "pch.h"

template <typename DATA>
class LockFreeQueue
{
private:
	struct __declspec(align(16)) Node
	{
		DATA data;
		Node* next;

		Node() : next(nullptr) { }
		Node(DATA value) : data(value), next(nullptr) { }
	};

	struct __declspec(align(16)) StampNode
	{
		Node* ptr;
		LONG64 stamp;

		StampNode() : ptr(nullptr), stamp(0) { }
	};

private:
	StampNode head;
	StampNode tail;
	ULONGLONG size;

	Node* free_list;
	ULONGLONG pop_count;

public:
	LockFreeQueue() : size(0)
	{
		head.ptr = tail.ptr = (Node*)_aligned_malloc(sizeof(Node), 16);
		if (head.ptr == nullptr || tail.ptr == nullptr)
		{
			std::cout << "�޸� �Ҵ翡 �����߽��ϴ�. (head / tail)" << std::endl;
			return;
		}

		head.ptr->next = tail.ptr->next = nullptr;
		head.stamp = tail.stamp = 0;
	}

	~LockFreeQueue()
	{
		Clear();
		_aligned_free(head.ptr);
		// tail.ptr�� free�ϸ� �ߺ� ������ �Ǿ����
	}

	bool Enqueue(const DATA source)
	{
		Node* node = (Node*)_aligned_malloc(sizeof(Node), 16);
		if (node == nullptr)
		{
			std::cout << "�޸� �Ҵ翡 �����߽��ϴ�. (node)" << std::endl;
			return false;
		}
		new(node) Node(source);

		while (true)
		{
			StampNode last;
			Node* next;
			last = tail;
			next = last.ptr->next;

			if (last.ptr == tail.ptr)
			{
				if (next == nullptr)
				{
					// last.ptr->next = node;
					// ��ȯ���� nullptr�� �ƴ϶�� ���� �ٸ� �����忡 ���� last.ptr->next�� �ٲ��ٴ� ���̴�.
					if (InterlockedCompareExchangePointer((PVOID*)&last.ptr->next, node, next) == nullptr)
					{
						// tail = node;
						// �ش� ��ġ���� �����尡 sleep�Ǹ� ���� �ð� �� �۾��� 0�� �ǹǷ� �ٸ� �����尡 �����ش�.
						InterlockedCompareExchange128((LONG64*)&tail, last.stamp + 1, (LONG64)node, (LONG64*)&last);

						// tail = node; �� ��� ��ġ���� �����ߵ� ī��Ʈ�� ���⼭�� ������.
						InterlockedIncrement64((LONG64*)&size);
						return true;
					}
				}
				else
				{
					// ����ؼ� tail = next�� ���� (�� ��, ���� ���� node�� �ƴԿ� �����Ѵ�.)
					InterlockedCompareExchange128((LONG64*)&tail, last.stamp + 1, (LONG64)next, (LONG64*)&last);
				}
			}
		}
	}

	bool Dequeue(DATA* destination)
	{
		InterlockedIncrement(&pop_count);

		while (true)
		{
			StampNode first = head;
			StampNode last = tail;
			Node* next = first.ptr->next;

			if (first.ptr == head.ptr)
			{
				if (first.ptr == last.ptr)
				{
					if (next == nullptr)
					{
						return false;
					}
					else
					{
						InterlockedCompareExchange128((LONG64*)&tail, last.stamp + 1, (LONG64)next, (LONG64*)&last);
					}
				}
				else
				{
					if (InterlockedCompareExchange128((LONG64*)&head, first.stamp + 1, (LONG64)next, (LONG64*)&first))
					{
						*destination = next->data;

						TryDelete(first.ptr);
						first.ptr = nullptr;

						InterlockedDecrement64((LONG64*)&size);
						return true;
					}
				}
			}
		}
	}

	void Clear()
	{
		unsigned long long cnt = 0;

		Node* ptr = nullptr;
		while (head.ptr->next != nullptr)
		{
			ptr = head.ptr->next;
			head.ptr->next = head.ptr->next->next;
			_aligned_free(ptr);

			InterlockedDecrement(&size);
			InterlockedIncrement(&cnt);
		}

		head.stamp = 0;
		tail = head;

		std::cout << "LockFreeQueue Clear Count : " << cnt << " Size : " << size << std::endl;
	}

	ULONGLONG GetSize() { return size; }
	bool Empty() { return (head.ptr == tail.ptr && head.ptr->next == nullptr); }

	void Display(int count)
	{
		printf("\n==== Display ==== \n");

		Node* ptr = head.ptr->next;
		while (count > 0 && ptr != nullptr)
		{
			std::cout << ptr->data << " ";
			ptr = ptr->next;
			--count;
		}
		printf("\n");
	}

private:
	void TryDelete(Node* node)
	{
		if (pop_count == 1)
		{
			// ���� head�� �ű�� �� ���� TryDelete�� �ϱ� ������ �ش� ������ pop_count�� 1�̶�� ���� 
			// �ٸ� �����尡 ���� head�� ������ ���� �ʴٴ� ���̴�.
			_aligned_free(node);

			// ������ ��Ƶ� pending list�� ���� �޸� ������ �õ��Ѵ�.			
			Node* old_free_list = (Node*)InterlockedExchangePointer((PVOID*)(&free_list), nullptr);
			if (InterlockedDecrement(&pop_count) == 0)
			{
				// �ش� ������ pop_count�� 0�̶�� ���� �ٸ� �����尡 �޸� ������ �ʿ��� ��带 ��� ����Ʈ�� �����Ͽ��ų�
				// �ƹ��� �õ��� �Ͼ�� �ʾ� ������ �и��� old_free_list�� �޸� ������ �� �ִٴ� ���̴�.
				ReleaseFreeList(old_free_list);
			}
			else if (old_free_list != nullptr)
			{
				AddListToFreeList(old_free_list);
			}
		}
		else
		{
			AddNodeFreeList(node);
			InterlockedDecrement(&pop_count);
		}
	}

	void AddNodeFreeList(Node* first, Node* last)
	{
		while (true)
		{
			Node* old_free_list = free_list;
			last->next = old_free_list;
			if (InterlockedCompareExchangePointer((PVOID*)&free_list, first, old_free_list) == old_free_list)
			{
				return;
			}
		}
	}

	void AddNodeFreeList(Node* node)
	{
		AddNodeFreeList(node, node);
	}

	void AddListToFreeList(Node* first)
	{
		Node* last = first;
		while (last->next != nullptr)
		{
			last = last->next;
		}
		AddNodeFreeList(first, last);
	}

	void ReleaseFreeList(Node* node)
	{
		while (node != nullptr)
		{
			Node* next = node->next;
			_aligned_free(node);
			node = next;
		}
	}
};

