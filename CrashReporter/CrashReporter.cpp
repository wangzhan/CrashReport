// WANGZhan -> wangzhan.1985@gmail.com
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "..\CrashReportLib\CrashReport.h"


// 将崩溃进程内存的数据读取到本进程
int _tmain(int argc, _TCHAR* argv[])
{
	// crash dump 命令行参数
	// CrashReporter.exe [exception type] [PID] [TID] [exception pointer]
	if (argc < 5)
		return 1;

	crashdump::CrashDumpData crashdata;
	memset(&crashdata, 0, sizeof(crashdata));

	wchar_t szCrashDumpDir[MAX_PATH + 1] = { 0 };
	::GetTempPath(MAX_PATH, szCrashDumpDir);

	wchar_t szCrashDumpPath[MAX_PATH + 1] = { 0 };
	swprintf_s(szCrashDumpPath, MAX_PATH, _T("%s\\crashdump_%s_%s_%s.dmp"), szCrashDumpDir, argv[1],
		argv[2], argv[3]);

	crashdata.strDumpPath = szCrashDumpPath;

	crashdata.dwPID = _wtoi(argv[2]);
	crashdata.dwTID = _wtoi(argv[3]);
	EXCEPTION_POINTERS *pRemoteExceptionPointers = (EXCEPTION_POINTERS *)_wtoi(argv[4]);

	CHandleHelper hProcess(::OpenProcess(PROCESS_ALL_ACCESS, FALSE, crashdata.dwPID));
	if (hProcess == NULL)
		return 1;

	EXCEPTION_POINTERS exp;
	DWORD dwBytesRead = 0;
	if (::ReadProcessMemory(hProcess, pRemoteExceptionPointers, &exp, sizeof(EXCEPTION_POINTERS), &dwBytesRead)
		&& dwBytesRead == sizeof(EXCEPTION_POINTERS))
	{
		CONTEXT *pContext = new CONTEXT;
		if (::ReadProcessMemory(hProcess, exp.ContextRecord, pContext, sizeof(CONTEXT), &dwBytesRead) && dwBytesRead == sizeof(CONTEXT))
		{
			EXCEPTION_RECORD *pRecord = new EXCEPTION_RECORD;
			if (::ReadProcessMemory(hProcess, exp.ExceptionRecord, pRecord, sizeof(EXCEPTION_RECORD), &dwBytesRead) && dwBytesRead == sizeof(EXCEPTION_RECORD))
			{
				for (EXCEPTION_RECORD *pNext = pRecord; pNext->ExceptionRecord != NULL;)
				{
					EXCEPTION_RECORD *pNextRecord = new EXCEPTION_RECORD;
					if (!::ReadProcessMemory(hProcess, pNext->ExceptionRecord, pNextRecord, sizeof(EXCEPTION_RECORD), &dwBytesRead)
						|| dwBytesRead != sizeof(EXCEPTION_RECORD))
					{
						break;
					}
					pNext->ExceptionRecord = pNextRecord;
					pNext = pNextRecord;
				}
				crashdata.pExceptionPointer = new EXCEPTION_POINTERS;
				crashdata.pExceptionPointer->ExceptionRecord = pRecord;
				crashdata.pExceptionPointer->ContextRecord = pContext;
			}
		}
	}

	crashdump::GenerateDumpFile(&crashdata);

	return 0;
}


// 直接引用崩溃进程的内存
//int _tmain(int argc, _TCHAR* argv[])
//{
//	// crash dump 命令行参数
//	// CrashReporter.exe [exception type] [PID] [TID] [exception pointer]
//	if (argc < 5)
//		return 1;
//
//	crashdump::CrashDumpData crashdata;
//	memset(&crashdata, 0, sizeof(crashdata));
//
//	wchar_t szCrashDumpDir[MAX_PATH + 1] = { 0 };
//	::GetTempPath(MAX_PATH, szCrashDumpDir);
//
//	wchar_t szCrashDumpPath[MAX_PATH + 1] = { 0 };
//	swprintf_s(szCrashDumpPath, MAX_PATH, _T("%s\\crashdump_%s_%s_%s.dmp"), szCrashDumpDir, argv[1],
//		argv[2], argv[3]);
//
//	crashdata.strDumpPath = szCrashDumpPath;
//
//	crashdata.dwPID = _wtoi(argv[2]);
//	crashdata.dwTID = _wtoi(argv[3]);
//	crashdata.pExceptionPointer = (EXCEPTION_POINTERS *)_wtoi(argv[4]);
//
//	crashdump::GenerateDumpFile2(&crashdata);
//
//	return 0;
//}

