#include "LockFreeTest.h"

unsigned int WINAPI PushLockFreeStack(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		mystack.Push(i);
	}

	return 0;
}

unsigned int WINAPI PopLockFreeStack(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	int dest;
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		mystack.Pop(&dest);
	}

	return 0;
}

unsigned int WINAPI RandomPushPopLockFreeStack(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	int dest;
	srand(time(NULL));
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		if (rand() % 2 == 0)
		{
			mystack.Push(i);
			InterlockedIncrement(&cntEnq);
		}
		else
		{
			if (mystack.Pop(&dest))
			{
				InterlockedIncrement(&cntDeq);
			}
		}
	}

	return 0;
}




unsigned int WINAPI PushLockFreeQueue(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		myqueue.Enqueue(i);
	}

	return 0;
}

unsigned int WINAPI PopLockFreeQueue(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	int dest;
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		myqueue.Dequeue(&dest);
		InterlockedIncrement64(&g_count[dest]);
	}

	return 0;
}

unsigned int WINAPI RandomPushPopLockFreeQueue(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	int dest;
	srand(time(NULL));
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		if (rand() % 2 == 0)
		{
			myqueue.Enqueue(i);
			InterlockedIncrement(&cntEnq);
		}
		else
		{
			if (myqueue.Dequeue(&dest))
			{
				InterlockedIncrement(&cntDeq);
			}
		}
	}

	return 0;
}

/*
unsigned int WINAPI PushPopMemoryPool(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	int* dest = nullptr;
	srand(time(NULL));
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		if (i % 2 == 0)
		{
			dest = (int*)g_memory.Allocate(sizeof(int));
			*dest = i;
			// std::cout << *dest << std::endl;
		}
		else
		{
			g_memory.Release(dest);
		}
	}

	return 0;
}
*/

unsigned int WINAPI PushPopXnew(PVOID argc)
{
	int* num_thread_arg = (int*)argc;
	int num_thread = *num_thread_arg;
	int* dest = nullptr;
	srand(time(NULL));
	for (int i = 0; i < NUM_TEST / num_thread; i++)
	{
		if (i % 2 == 0)
		{
			dest = (int*)xnew<int>();
			*dest = i;
			// std::cout << *dest << std::endl;
		}
		else
		{
			xdelete(dest);
		}
	}

	return 0;
}