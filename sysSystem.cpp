//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace sysSystem {
	DWORD GetCPUCores() {
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

		return sysinfo.dwNumberOfProcessors;
	}
}
