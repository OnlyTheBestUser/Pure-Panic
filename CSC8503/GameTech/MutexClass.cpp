#include "MutexClass.h"

MutexClass::MutexClass()
{
	mutex = CreateMutex(
		NULL,	// Default security
		FALSE,	// Initially not owned
		NULL);	// Unamed mutex
}

MutexClass::~MutexClass()
{
	CloseHandle(mutex);
}

void MutexClass::LockMutex()
{
	WaitForSingleObject(
		mutex,		// Handle to mutex
		INFINITE);	// No time-out interval
}

void MutexClass::UnlockMutex()
{
	ReleaseMutex(mutex);
}