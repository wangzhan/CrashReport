// WANGZhan -> wangzhan.1985@gmail.com
#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>


// NOTE: 在Release模式下测试，以生产崩溃日志
// C:\Users\James\AppData\Local\Temp\crashdump_1_15432_20352.dmp


void DumpException(PEXCEPTION_POINTERS pExceptionPointers);

static void myParameterHandler(const wchar_t *a1, const wchar_t *a2, const wchar_t *a3, unsigned int a4, uintptr_t a5)
{
	printf("myParameterHandler\n");

	// 主动触发异常，由myExceptionFilter处理
	throw 1;
}

static void myPurecallHandler()
{
	printf("myPurecallHandler\n");

	// 主动触发异常，由myExceptionFilter处理
	throw 2;
}

static LONG WINAPI myExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	printf("myExceptionFilter\n");

	DumpException(ExceptionInfo);

	::TerminateProcess(::GetCurrentProcess(), 100);
	return EXCEPTION_EXECUTE_HANDLER;
}


class CDerived;
class CBase
{
public:
	CBase(CDerived *pDerived) : m_pDerived(pDerived) {}
	virtual ~CBase();
	virtual void function() = 0;

private:
	CDerived *m_pDerived;
};

class CDerived : public CBase
{
public:
	CDerived() : CBase(this) {}
	virtual void function() {}
};

CBase::~CBase()
{
	m_pDerived->function();
}

// 测试无效参数 myParameterHandler -> myExceptionFilter
static void Test1()
{
	// 设置未处理异常处理例程
	SetUnhandledExceptionFilter(myExceptionFilter);

	// 设置无效参数处理例程
	_set_invalid_parameter_handler(myParameterHandler);

	printf(NULL);
}

// 测试纯虚函数 myPurecallHandler -> myExceptionFilter
static void Test2()
{
	// 设置未处理异常处理例程
	SetUnhandledExceptionFilter(myExceptionFilter);

	// 设置纯虚函数调用处理例程
	_set_purecall_handler(myPurecallHandler);

	CDerived myDerived;
}

void xxx()
{
	char *p = NULL;
	*p = 0;
}

// 测试未处理异常 -> myExceptionFilter
static void Test3()
{
	// 设置未处理异常处理例程
	SetUnhandledExceptionFilter(myExceptionFilter);

	xxx();
}


static void SetExceptionsHandler()
{
	// 设置未处理异常处理例程
	SetUnhandledExceptionFilter(myExceptionFilter);

	// 设置无效参数处理例程
	_set_invalid_parameter_handler(myParameterHandler);

	// 设置纯虚函数调用处理例程
	_set_purecall_handler(myPurecallHandler);
}



static int g_iExceptionHandlerType = 1;

static void DumpException(PEXCEPTION_POINTERS pExceptionPointers)
{
	// 这里面尽量少用系统调用、crt之类的东西，禁止用锁，因为有可能在发生崩溃时，有些资源正在被占用，这里再次申请时就会被卡住
	DWORD dwPID = ::GetCurrentProcessId();
	DWORD dwTID = ::GetCurrentThreadId();

	wchar_t szModule[1024] = { 0 };
	::GetModuleFileName(NULL, szModule, _countof(szModule));

	// 获取目录
	wchar_t* p = wcsrchr(szModule, '\\');
	if (!p)
		return;

	*p = 0;

	wchar_t szModulePath[1024] = { 0 };
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	wsprintfW(szModulePath, L"\"%s\\CrashReporter.exe\" %d %d %d %d", szModule, g_iExceptionHandlerType,
		dwPID, dwTID, pExceptionPointers);

	// TODO: 用于调试
	//wprintf(szModulePath);
	//Sleep(50 * 60 * 1000);

	if (::CreateProcess(NULL, szModulePath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}


int _tmain(int argc, _TCHAR* argv[])
{
	Test1();
	//Test2();
	//Test3();
	
	return 0;
}

