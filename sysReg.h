// ---------------------------------------------------------------------------

#ifndef sysRegH
#define sysRegH

// ---------------------------------------------------------------------------

namespace sysReg {

	/**
	wcsValue is returns
    do not forget to free wcsValue buffer memmory after use

	 hKey constants:
	 HKEY_CLASSES_ROOT
	 HKEY_CURRENT_CONFIG
	 HKEY_CURRENT_USER
	 HKEY_LOCAL_MACHINE
	 HKEY_USERS
	 wcsSubKey string of sub key
	 wcsParamName string of parameter name
	 wcsValue out string value
	 valueSize

	 return true if success
	 */
	bool RegQueryStringValue(HKEY hKey, const wchar_t *wcsSubKey, const wchar_t *wcsParamName, wchar_t *&wcsValue);
}
#endif
