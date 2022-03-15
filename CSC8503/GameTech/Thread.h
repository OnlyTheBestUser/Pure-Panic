#pragma once
#include <windows.h>

class Thread
{
public:
	Thread() {}
	virtual ~Thread() { CloseHandle(thread_handle); }
	virtual void Start();
	virtual void Join();
	bool GetThreadState() { return completed; }
	virtual DWORD GetId() const { return tid; }

protected:
	virtual void Run() = 0;
	friend DWORD thread_ftn(LPVOID T);
	HANDLE thread_handle;
	DWORD tid; 
	bool completed = false;

private:
	Thread(const Thread& src);
	Thread& operator=(const Thread& rhs);
};