//---------------------------------------------------------------------------

#ifndef sysBaseTypesH
#define sysBaseTypesH
//---------------------------------------------------------------------------

namespace sysBaseTypes {

#ifdef __BORLANDC__

String EmptyTo(const String &testValue, String defaultValue);

int EmptyTo(int testValue, int defaultValue);

#endif
}

#endif
