// Injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define PROC_THREAD_ATTRIBUTE_BNO_ISOLATION \
    ProcThreadAttributeValue (19, FALSE, TRUE, FALSE)

typedef struct _BNO_ISOLATION_ATTRIBUTE {
	INT     IsolationEnabled;
	WCHAR   IsolationPrefix[136];
} BNO_ISOLATION_ATTRIBUTE;


int create_process(wstring program);
wstring get_error_by_win32code(HRESULT rc);
wstring get_last_error_as_string();
inline bool file_exist(const wstring& name);

int main()
{
	while (true)
	{
		wchar_t program[1024];
		wcout << "Enter program full path (for example: C:\\Windows\\system32\\notepad.exe): " << endl;
		wcin.getline(program, 1024);

		if(wstring(program).empty())
		{
			wcout << "Program name is empty. Try again..." << endl;
			continue;		
		}

		if (!wcscmp(program, _T("exit")) || !wcscmp(program, _T("quit")))
		{
			break;
		}

		if(!file_exist(program))
		{
			wcout << "program " << program << " not found" << endl;
			continue;
		}
		
		if (create_process(program) != S_OK)
		{
			::OutputDebugString(_T("Error start process"));
		}


	}

	return S_OK;
}

int create_process(wstring program)
{
	PROCESS_INFORMATION pi;
	STARTUPINFOEX si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	
	SIZE_T attributeListLength = 0;
	BNO_ISOLATION_ATTRIBUTE bnoIsolation{ TRUE, {'p','r','e','f','i','x'} };
	if (!InitializeProcThreadAttributeList(nullptr, 1, 0, &attributeListLength) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		wcout << _T("InitializeProcThreadAttributeList failed (") << ::get_last_error_as_string() << _T(").") << endl;
		return E_FAIL;
	}
	
	
	si.StartupInfo.cb = sizeof(si);
	si.lpAttributeList = static_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(HeapAlloc(GetProcessHeap(), 0, attributeListLength));
	if (!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeListLength))
	{
		wcout << _T("InitializeProcThreadAttributeList failed (") << ::get_last_error_as_string() << _T(").") << endl;
		return E_FAIL;
	}
	if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_BNO_ISOLATION, &bnoIsolation, sizeof bnoIsolation, nullptr, nullptr))
	{
		wcout << _T("UpdateProcThreadAttribute failed (") << ::get_last_error_as_string() << _T(").") << endl;
		return E_FAIL;
	}

	if (!CreateProcessW(nullptr, &program[0], nullptr, nullptr, FALSE, EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, reinterpret_cast<LPSTARTUPINFOW>(&si), &pi))
	{
		wcout << "create_process_kernel failed" << hex << ::GetLastError() << get_last_error_as_string() << endl;
		return E_FAIL;
	}
	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return S_OK;
}

wstring get_last_error_as_string()
{
	//Get the error message, if any.
	auto rc = ::GetLastError();
	if (rc == 0)
		return std::wstring(); //No error message has been recorded

	return get_error_by_win32code(rc);
}

wstring get_error_by_win32code(HRESULT rc)
{
	LPWSTR messageBuffer = nullptr;
	const size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, rc, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&messageBuffer), 0, nullptr);

	wstring message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

inline bool file_exist(const wstring& name)
{
    wifstream file(name);
    return !file ? false : true;
}
