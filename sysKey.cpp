//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace sysKey {

bool IsCtrlPressed(){
	return (GetKeyState(VK_CONTROL) >> 7) != 0;
}

}

