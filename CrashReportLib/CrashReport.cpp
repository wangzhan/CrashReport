#include "stdafx.h"
#include "CrashReport.h"
#include "DbgHelp.h"

#pragma comment(lib, "DbgHelp.lib")


bool crashdump::GenerateDumpFile(CrashDumpData *pCrashDumpData)
{
	if (pCrashDumpData == NULL)
		return false;

	CHandleHelper hProcess(::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pCrashDumpData->dwPID));
	if (hProcess == NULL)
		return false;

	CHandleHelper hMiniDumpFile(::CreateFileW(pCrashDumpData->strDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	if (hMiniDumpFile == INVALID_HANDLE_VALUE)
		return false;

	MINIDUMP_EXCEPTION_INFORMATION eDumpInfo;
	eDumpInfo.ThreadId = pCrashDumpData->dwTID;
	eDumpInfo.ExceptionPointers = pCrashDumpData->pExceptionPointer;
	eDumpInfo.ClientPointers = FALSE;		// 表示 ExceptionPointers 指向调用进程的内存

	int miniDumpType = MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData | MiniDumpWithFullMemoryInfo;
	BOOL bRet = ::MiniDumpWriteDump(hProcess, pCrashDumpData->dwPID, hMiniDumpFile, (MINIDUMP_TYPE)miniDumpType,
		&eDumpInfo, NULL, NULL);

	return !!bRet;
}

bool crashdump::GenerateDumpFile2(CrashDumpData *pCrashDumpData)
{
	if (pCrashDumpData == NULL)
		return false;

	CHandleHelper hProcess(::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pCrashDumpData->dwPID));
	if (hProcess == NULL)
		return false;

	CHandleHelper hMiniDumpFile(::CreateFileW(pCrashDumpData->strDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	if (hMiniDumpFile == INVALID_HANDLE_VALUE)
		return false;

	MINIDUMP_EXCEPTION_INFORMATION eDumpInfo;
	eDumpInfo.ThreadId = pCrashDumpData->dwTID;
	eDumpInfo.ExceptionPointers = pCrashDumpData->pExceptionPointer;
	eDumpInfo.ClientPointers = TRUE;		// 表示 ExceptionPointers 指向调用进程的内存

	int miniDumpType = MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData | MiniDumpWithFullMemoryInfo;
	BOOL bRet = ::MiniDumpWriteDump(hProcess, pCrashDumpData->dwPID, hMiniDumpFile, (MINIDUMP_TYPE)miniDumpType,
		&eDumpInfo, NULL, NULL);

	return !!bRet;
}

