#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysApp {
#ifdef __BORLANDC__
	std::wstring defautlApplicationSettingPath = L"";

	std::wstring GetApplicationSettingPath() {
		if (defautlApplicationSettingPath.empty()) {
			std::wstring appPath = sysFile::GetExecutablePath();
			std::wstring appFolder = sysFile::GetFolder(appPath.c_str());
			std::wstring appName = sysFile::GetFileName(appPath.c_str());

			std::wstring appSettingFile = appFolder + std::wstring(L"\\") + appName + std::wstring(L"_app_settings.json");

			return appSettingFile;
		} else {
			std::wstring appPath = sysFile::GetExecutablePath();
			std::wstring appName = sysFile::GetFileName(appPath.c_str());
			std::wstring appSettingFile = defautlApplicationSettingPath + std::wstring(L"\\") + appName + std::wstring(L"_app_settings.json");
			return appSettingFile;
		}
	}

	bool SetSetting(const wchar_t *settingName, bool value) {
		return SetSetting(settingName, (value)?L"1":L"0");
	}

	bool SetSetting(const wchar_t *settingName, double value) {
		wchar_t sValue[256];
		_snwprintf(sValue, 255, L"%.*f", DBL_DIG, value);
		return SetSetting(settingName, sValue);
	}

	bool SetSetting(const wchar_t *settingName, const TJSONValue *value){
		static std::wstring sFile = GetApplicationSettingPath();

		if (!sysFile::IsFileExist(sFile.c_str()) || !sysFile::GetFileSize(sFile.c_str())){
			std::ofstream out(sFile.c_str(), std::ios_base::out);
			out.write("{}", 2);
			out.close();
		}

		#ifdef SYSAPP_HIDDEN_SETTINGS
		SetFileAttributes(sFile.c_str(), FILE_ATTRIBUTE_HIDDEN);
		#endif


		TJSONObject *jsonSetting = sysJSON::ParseJSONFile(sFile.c_str());
		if (!jsonSetting){
			return false;
		}

		//make copy of input value
		TJSONValue *cpyValue = (TJSONValue *)const_cast<TJSONValue *>(value)->Clone();

		TJSONPair *jsonPair = sysJSON::GetPair(jsonSetting, const_cast<wchar_t *>(settingName));
		if (jsonPair){
			jsonPair->JsonValue = cpyValue;
		} else {
			jsonSetting->AddPair(String(settingName), cpyValue);
		}

		char *buffer;
		int len;
		String sJSONSetting = jsonSetting->ToString();
		sysStr::WideCharToUTF8(sJSONSetting.c_str(), sJSONSetting.Length(), buffer, len);

		sysFile::WriteToFile(sFile.c_str(), buffer, len);

		delete[]buffer;
		delete jsonSetting;

		return true;

	}

	bool SetSetting(const wchar_t *settingName, const wchar_t *value){
		static std::wstring sFile = GetApplicationSettingPath();

		if (!sysFile::IsFileExist(sFile.c_str()) || !sysFile::GetFileSize(sFile.c_str())){
			sysFile::WriteToFile(sFile.c_str(), "{}", 2);
		}

		#ifdef SYSAPP_HIDDEN_SETTINGS
		SetFileAttributes(sFile.c_str(), FILE_ATTRIBUTE_HIDDEN);
		#endif

		TJSONObject *jsonSetting = sysJSON::ParseJSONFile(sFile.c_str());
		if (!jsonSetting){
			return false;
		}

		TJSONPair *jsonPair = sysJSON::GetPair(jsonSetting, const_cast<wchar_t *>(settingName));
		if (jsonPair){
			jsonPair->JsonValue = new TJSONString(value);
		} else {
			String _name(settingName);
			String _value(value);
			jsonSetting->AddPair(_name, _value);
		}

		// screen special simbols in JSON strings before save it in file
		// if you try to save for instance {"v":"C:\"}, after it you can't parse it JSON after it
		jsonSetting = (TJSONObject *)sysJSON::Screen(jsonSetting);

		char *buffer;
		int len;
		String sJSONSetting = jsonSetting->ToString();
		sysStr::WideCharToUTF8(sJSONSetting.c_str(), sJSONSetting.Length(), buffer, len);

		bool result = false;

		//refuse from std::ofstream because of CodeGuard
		FILE *f = _wfopen(sFile.c_str(), L"wb");
		if (f) {
			fwrite(buffer, sizeof(char), len, f);
			fclose(f);

			result = true;
		} else {
			//TODO make log here
			//String log = "Error while trying to open for write the application settings file errno: " + IntToStr(errno);
			//TLogger::logger->Log(log.c_str(), LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__);
			result = false;
		}

		delete[]buffer;
		delete jsonSetting;

		return result;
	}

	std::unique_ptr<TJSONObject> GetAppSettings() {
		static std::wstring sFile = GetApplicationSettingPath();

		if (!sysFile::IsFileExist(sFile.c_str())){
			return std::unique_ptr<TJSONObject>(NULL);
		}

		TJSONObject *jsonSetting = sysJSON::ParseJSONFile(sFile.c_str());
		if (!jsonSetting){
			return std::unique_ptr<TJSONObject>(NULL);
		}

		return std::unique_ptr<TJSONObject>(jsonSetting);
	}

	TJSONValue * GetSetting(const wchar_t *settingName){
		static std::wstring sFile = GetApplicationSettingPath();

		if (!sysFile::IsFileExist(sFile.c_str())){
			return NULL;
		}

		TJSONObject *jsonSetting = sysJSON::ParseJSONFile(sFile.c_str());
		if (!jsonSetting){
			return NULL;
		}

		TJSONValue *jsonValue = sysJSON::Get(jsonSetting, const_cast<wchar_t *>(settingName));
		if (!jsonValue){
			delete jsonSetting;
			return NULL;
		}

		TJSONValue *cpyJsonValue = static_cast<TJSONValue *>(jsonValue->Clone());

		delete jsonSetting;

		return cpyJsonValue;
	}

	int GetSettingInt(const wchar_t *settingName, int defaultValue) {
		String value = GetSettingString(settingName, "");
		if (value.IsEmpty()){
			return defaultValue;
		}

		return StrToIntDef(value, defaultValue);
	}

	double GetSettingDouble(const wchar_t *settingName, double defaultValue) {
		String value = GetSettingString(settingName, "");
		if (value.IsEmpty()){
			return defaultValue;
		}

		return StrToFloatDef(value, defaultValue);
	}

	String GetSettingString(const wchar_t *settingName, String defaultValue) {
		TJSONValue * jsonValue = GetSetting(settingName);
		if (!jsonValue) {
			return defaultValue;
		}

		String value = jsonValue->Value();

		delete jsonValue;

		return value;
	}

	bool GetSettingBool(const wchar_t *settingName, bool defaultValue) {
		String value = GetSettingString(settingName, (defaultValue)?"1":"0");
		return value == "1";
	}

	bool CheckSetting(const wchar_t *settingName) {
		static std::wstring sFile = GetApplicationSettingPath();

		if (!sysFile::IsFileExist(sFile.c_str())){
			return false;
		}

		TJSONObject *jsonSetting = sysJSON::ParseJSONFile(sFile.c_str());
		if (!jsonSetting) {
			return false;
		}

		bool isExist = false;

		TJSONValue *jsonValue = sysJSON::Get(jsonSetting, settingName);
		if (jsonValue) {
			isExist = true;
		}

		delete jsonSetting;

		return isExist;
	}


    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	#define UNKNOWN_OS_BIT -1

	bool IsWow64(){
        static char isWow64 = UNKNOWN_OS_BIT;
        if (isWow64 != UNKNOWN_OS_BIT) {
        	//cache result for next invoke, I hope OS BIT does not changed while program running
			return isWow64 == TRUE;
        }
		BOOL bIsWow64 = FALSE;

		// IsWow64Process is not available on all supported versions of Windows.
		// Use GetModuleHandle to get a handle to the DLL that contains the function
		// and GetProcAddress to get a pointer to the function if available.

		fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

		if (NULL != fnIsWow64Process){
			if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)){
				// handle error
			}
		}
        isWow64 = bIsWow64;

		return bIsWow64 == TRUE;
	}

	void SaveFormSettings(TForm *form) {
		wchar_t name[255];
		size_t size = swprintf(name, L"%s", form->Name.c_str());

		swprintf(&name[size], L"_Top");
		SetSetting(name, IntToStr(form->Top).c_str());

		swprintf(&name[size], L"_Left");
		SetSetting(name, IntToStr(form->Left).c_str());

		swprintf(&name[size], L"_Width");
		SetSetting(name, IntToStr(form->Width).c_str());

		swprintf(&name[size], L"_Height");
		SetSetting(name, IntToStr(form->Height).c_str());

		swprintf(&name[size], L"_WindowState");
		SetSetting(name, IntToStr(form->WindowState).c_str());
	}

	void RestoreFormSettings(TForm *form) {
		wchar_t name[255];
		size_t size = swprintf(name, L"%s", form->Name.c_str());

		swprintf(&name[size], L"_WindowState");
		form->WindowState = (TWindowState)GetSettingInt(name, 1);

		if (form->WindowState == 0) {

			swprintf(&name[size], L"_Top");
			int top = GetSettingInt(name);

			swprintf(&name[size], L"_Left");
			int left = GetSettingInt(name);

			swprintf(&name[size], L"_Width");
			int width = GetSettingInt(name);

			swprintf(&name[size], L"_Height");
			int height = GetSettingInt(name);

			if (top >= 0) {
				form->Top = top;
			}
			if (left >= 0) {
				form->Left = left;
			}
			if (width > 0) {
				form->Width = width;
			}
			if (height) {
				form->Height = height;
			}
		}
	}

	void SaveControlState(TControl *control, String prefix) {
		String parameter;
		if (prefix == NULL) {
			parameter = control->Name;
		} else {
			parameter = prefix + control->Name;
		}

		std::unique_ptr<TJSONObject> jsonState = sysControlState::GetControlState(control);
		if (jsonState) {
			sysApp::SetSetting(parameter.c_str(), jsonState.get());
		}
	}

	void SaveControlsState(const std::list<TControl *> &controls, String prefix) {
		std::unique_ptr<TJSONObject> jsonAppSettings = GetAppSettings();
		if (!jsonAppSettings) {
        	return;
		}

        //TODO
	}

	void RestoreControlState(TControl *control, String prefix) {
		std::unique_ptr<TJSONObject> jsonAppSettings = GetAppSettings();
		if (jsonAppSettings) {
			sysControlState::RestoreControlState(jsonAppSettings, control, prefix);
		}
	}

	void RestoreControlsState(const std::list<TControl *> &controls, String prefix) {
		std::unique_ptr<TJSONObject> jsonAppSettings = GetAppSettings();
		if (!jsonAppSettings) {
        	return;
		}

		for (std::list<TControl *>::const_iterator i = controls.begin(), iEnd = controls.end(); i != iEnd; ++i) {
			sysControlState::RestoreControlState(jsonAppSettings, *i, prefix);
		}
	}
#endif
};




