#pragma once

#include "LockFreeStack.h"
#include "LockFreeQueue.h"
#include "Memory.h"
#include "MemoryMacro.h"

#define NUM_TEST 16000000

extern LockFreeStack<int> mystack;
extern LockFreeQueue<int> myqueue;

extern unsigned long long cntEnq;
extern unsigned long long cntDeq;

extern long long* g_count;

unsigned int WINAPI PushLockFreeStack(PVOID argc);
unsigned int WINAPI PopLockFreeStack(PVOID argc);
unsigned int WINAPI RandomPushPopLockFreeStack(PVOID argc);

unsigned int WINAPI PushLockFreeQueue(PVOID argc);
unsigned int WINAPI PopLockFreeQueue(PVOID argc);
unsigned int WINAPI RandomPushPopLockFreeQueue(PVOID argc);

unsigned int WINAPI PushPopMemoryPool(PVOID argc);
unsigned int WINAPI PushPopXnew(PVOID argc);
