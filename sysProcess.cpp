#include "pch.h"

#ifdef  __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysProcess {
	DWORD FindProcessId(const std::wstring& processName) {
		PROCESSENTRY32 processInfo;
		processInfo.dwSize = sizeof(processInfo);

		HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if ( processesSnapshot == INVALID_HANDLE_VALUE )
			return 0;

		Process32First(processesSnapshot, &processInfo);
		if ( !processName.compare(processInfo.szExeFile) )
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}

		while ( Process32Next(processesSnapshot, &processInfo) )
		{
			if ( !processName.compare(processInfo.szExeFile) )
			{
				CloseHandle(processesSnapshot);
				return processInfo.th32ProcessID;
			}
		}

		CloseHandle(processesSnapshot);
		return 0;
	}

	bool ExecuteCmd(const wchar_t *application, wchar_t *parameters) {
		PROCESS_INFORMATION pi;
		STARTUPINFO si = { sizeof(STARTUPINFO) };

		DWORD lRes = ::CreateProcess(application, parameters, NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);

		return lRes != 0;
	}

	bool ExecuteCmd(const wchar_t *application, wchar_t *parameters, int timeoutMS) {
		PROCESS_INFORMATION pi;
		STARTUPINFO si = { sizeof(STARTUPINFO) };

		DWORD lRes = ::CreateProcess(application, parameters,
			NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

		if (lRes) {
			while (WAIT_TIMEOUT == ::WaitForSingleObject(pi.hProcess, timeoutMS)) {
			  //Application->ProcessMessages();
			}
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}

        return lRes != 0;
	}
}
