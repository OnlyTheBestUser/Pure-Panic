#pragma once
#include <windows.h>

class MutexClass
{
public:
	MutexClass();
	virtual ~MutexClass();
	virtual void LockMutex();
	virtual void UnlockMutex();
protected:
	HANDLE mutex;
};