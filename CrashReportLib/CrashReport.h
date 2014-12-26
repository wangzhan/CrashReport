// WANGZhan -> wangzhan.1985@gmail.com
#pragma once
#include <windows.h>
#include <iostream>


// Handle 辅助类
class CHandleHelper
{
public:
	CHandleHelper() : m_hHandle(NULL) {}
	CHandleHelper(HANDLE hHandle) : m_hHandle(hHandle) {}

	~CHandleHelper()
	{
		if (m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE)
			CloseHandle(m_hHandle);
	}

	operator HANDLE& ()
	{
		return m_hHandle;
	}

private:
	HANDLE m_hHandle;
};


namespace crashdump
{

	struct CrashDumpData
	{
		std::wstring strDumpPath;

		DWORD dwPID;
		DWORD dwTID;
		EXCEPTION_POINTERS *pExceptionPointer;
	};


	// 访问本进程的异常内存
	bool GenerateDumpFile(CrashDumpData *pCrashDumpData);

	// 直接访问崩溃进程的异常内存（涉及跨进程）
	bool GenerateDumpFile2(CrashDumpData *pCrashDumpData);

}
