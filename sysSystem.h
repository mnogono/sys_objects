//---------------------------------------------------------------------------
#ifndef sysSystemH
#define sysSystemH
//---------------------------------------------------------------------------

namespace sysSystem {
	// return count of cpu cores
	DWORD GetCPUCores();

	// return OS page size
	DWORD GetPageSize();
}

#endif
