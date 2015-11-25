#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysKey {

#ifdef __BORLANDC__

bool IsCtrlPressed() {
	return (GetKeyState(VK_CONTROL) >> 7) != 0;
}

#endif
}

