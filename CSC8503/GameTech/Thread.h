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
	void AddToThreadProgress(float progress) { progression += progress; }
	void AssignHDC(HDC hdc) { this->hdc = hdc; }
	void SetRendererContext();
	void ClearRendererContext();
	virtual DWORD GetId() const { return tid; }

protected:
	virtual void Run() = 0;
	friend DWORD thread_ftn(LPVOID T);
	HANDLE thread_handle;
	DWORD tid; 
	bool completed = false;
	float progression = 0.0f;
	HDC hdc;
	HGLRC hglrc;

private:
	Thread(const Thread& src);
	Thread& operator=(const Thread& rhs);
};