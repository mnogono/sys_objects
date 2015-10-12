//---------------------------------------------------------------------------

#ifndef sysProcessH
#define sysProcessH

//---------------------------------------------------------------------------
namespace sysProcess {
	/*
    return process id by process name
    return 0 if no process found
    */
	DWORD FindProcessId(const std::wstring& processName);

   	bool ExecuteCmd(const wchar_t *application, wchar_t *parameters);

	bool ExecuteCmd(const wchar_t *application, wchar_t *parameters, int timeoutMS);
}

#endif
