#include "pch.h"

#ifdef  __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysSystem {
	DWORD GetCPUCores() {
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

		return sysinfo.dwNumberOfProcessors;
	}

	DWORD GetPageSize() {
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

        return sysinfo.dwPageSize;
	}
}
