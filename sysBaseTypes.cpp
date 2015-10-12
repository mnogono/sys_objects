//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace sysBaseTypes {

String EmptyTo(const String &testValue, String defaultValue){
	return (testValue.IsEmpty()) ? defaultValue : testValue;
}

int EmptyTo(int testValue, int defaultValue){
	return (testValue)?testValue:defaultValue;
}

}


