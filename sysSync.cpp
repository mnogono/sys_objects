//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace sysSync {

TCriticalSection::TCriticalSection() {
	InitializeCriticalSection(&cs);
}

TCriticalSection::~TCriticalSection() {
	DeleteCriticalSection(&cs);
}

}
