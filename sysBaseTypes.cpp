#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysBaseTypes {

#ifdef __BORLANDC__

String EmptyTo(const String &testValue, String defaultValue){
	return (testValue.IsEmpty()) ? defaultValue : testValue;
}

int EmptyTo(int testValue, int defaultValue){
	return (testValue)?testValue:defaultValue;
}

#endif

}


