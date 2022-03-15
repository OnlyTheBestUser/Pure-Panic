#include "Thread.h"

DWORD thread_ftn(LPVOID T)
{
	Thread* t = static_cast<Thread*>(T);
	t->Run();
	return NULL;
}

void Thread::Start()
{
	thread_handle = CreateThread(
		NULL,	// Default security
		0,		// Default stack size
		(LPTHREAD_START_ROUTINE)& thread_ftn,	// Thread function name
		(LPVOID)this,	// Argument to thread function
		0,		// Use default creation flags
		&tid);
}

void Thread::Join()
{
	WaitForSingleObject(thread_handle, INFINITE);
}