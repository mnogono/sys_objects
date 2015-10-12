// ---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

// ---------------------------------------------------------------------------

#pragma package(smart_init)

namespace sysReg {

	bool RegQueryStringValue(HKEY hKey, const wchar_t *wcsSubKey, const wchar_t *wcsParamName, wchar_t *&wcsValue/*, int &valueSize*/){
    	bool isSuccess = false;
    	HKEY hValKey;
		LONG result = RegOpenKeyEx(hKey, wcsSubKey, 0, KEY_READ, &hValKey);
		if (result == ERROR_SUCCESS){
			unsigned long bufferSize;
            RegQueryValueEx(hValKey, wcsParamName, NULL, NULL, NULL, &bufferSize);
			//RegGetValue(hValKey, wcsSubKey, wcsParamName, RRF_RT_REG_SZ, NULL, NULL, &bufferSize);

			unsigned char *sValueBuffer = new unsigned char[bufferSize];

			//result = RegGetValue(hValKey, wcsSubKey, wcsParamName, RRF_RT_REG_SZ, NULL, sValueBuffer, &bufferSize);
			result = RegQueryValueEx(hValKey, wcsParamName, NULL, NULL, sValueBuffer, &bufferSize);
			if (result == ERROR_SUCCESS){
                wcsValue = (wchar_t *)sValueBuffer;
                //valueSize = wcslen(wcsValue);
                isSuccess = true;
			}

			RegCloseKey(hValKey);
		}

		return isSuccess;
	}
}
