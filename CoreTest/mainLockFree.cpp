#include <process.h>
#include <chrono>
#include <stack>

#include "LockFreeTest.h"

using namespace std;
using namespace chrono;

// 1~1000 사이의 숫자를 랜덤하게 4백만회 삽입 / 삭제하는 프로그램을 통해
// 실행속도를 측정
#define KEY_RANGE 1000

unsigned long long cntEnq = 0;
unsigned long long cntDeq = 0;

LockFreeQueue<int> myqueue;
LockFreeStack<int> mystack;
stack<int> stlstack;
CRITICAL_SECTION stcs;

long long* g_count;

int main(void)
{
	InitializeCriticalSection(&stcs);
	int numofthread;
	unsigned int threadid;
	
	std::cout << "start.." << std::endl;
	for (int n = 1; n <= 32; n *= 2)
	{
		/*
		g_count = new long long[NUM_TEST / n];
		for (int i = 0; i < NUM_TEST / n; i++)
		{
			g_count[i] = 0;
		}
		*/
		// sum = 0;
		HANDLE* hThreads = (HANDLE*)malloc(sizeof(HANDLE) * n);
		if (hThreads == NULL)
		{
			cout << "메모리 부족" << endl;
			return 0;
		}
		numofthread = n;

		auto start_time = high_resolution_clock::now();

		for (int i = 0; i < n; i++)
		{
			hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, PushPopXnew, &numofthread, 0, 0);			
		}

		WaitForMultipleObjects(n, hThreads, TRUE, INFINITE);

		/*
		for (int i = 0; i < n; i++)
		{
			hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, PopLockFreeQueue, &numofthread, 0, 0);
		}

		WaitForMultipleObjects(n, hThreads, TRUE, INFINITE);
		*/
		free(hThreads);

		auto end_time = high_resolution_clock::now();		
		auto exec_time = end_time - start_time;

		int exec_ms = duration_cast<milliseconds>(exec_time).count();

		// stack.Display(20);
		// myqueue.Display(20);
		cout << "Threads[" << n << "]" << "Exec_time = " << exec_ms << "msecs\n";
		// printf("Difference : %d(%d, %d)\n", cntEnq - cntDeq, cntEnq, cntDeq);

		/*
		 myqueue.Clear();

		std::cout << g_count[NUM_TEST / n  - 1] << std::endl;

		for (int i = 0; i < NUM_TEST / n; i++)
		{
			if (g_count[i] != n)
			{
				std::cout << "error" << std::endl;
			}
		}
		
		delete g_count;
		*/
		/*
		// Push
		auto start_time = high_resolution_clock::now();

		for (int i = 0; i < n; i++)
		{
			hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, PushLockFreeStack, &numofthread, 0, 0);
		}

		WaitForMultipleObjects(n, hThreads, TRUE, INFINITE);

		auto end_time = high_resolution_clock::now();
		// free(hThreads);
		auto exec_time = end_time - start_time;

		int exec_ms = duration_cast<milliseconds>(exec_time).count();

		// stack.Display(20);
		// queue.Display(20);
		cout << "Threads[" << n << "]" << "Exec_time = " << exec_ms << "msecs\n";
		// printf("Difference : %d(%d, %d)", cntEnq - cntDeq, cntEnq, cntDeq);

		// Pop
		start_time = high_resolution_clock::now();

		for (int i = 0; i < n; i++)
		{
			hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, PopLockFreeStack, &numofthread, 0, 0);
		}
		

		WaitForMultipleObjects(n, hThreads, TRUE, INFINITE);

		end_time = high_resolution_clock::now();
		free(hThreads);
		exec_time = end_time - start_time;

		exec_ms = duration_cast<milliseconds>(exec_time).count();

		// stack.Display(20);
		// queue.Display(20);
		cout << "Threads[" << n << "]" << "Exec_time = " << exec_ms << "msecs\n";
		*/
		

		/*
		std::cout << stlstack.size() << std::endl;
		while (!stlstack.empty()) stlstack.pop();
		*/

		cntEnq = 0;
		cntDeq = 0;
	}

	while (true)
	{

	}
	return 0;
}

