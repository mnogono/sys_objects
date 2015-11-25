// ---------------------------------------------------------------------------

#ifndef sysAppH
#define sysAppH

#include <cstddef>
//define SYSAPP_HIDDEN_SETTINGS in startup project for make setting file as hidden. probelm with Hidden files, cant be modify

// ---------------------------------------------------------------------------

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

namespace sysApp {
#ifdef __BORLANDC__
	//need to define this variable into the project, for instance as empty string
	extern std::wstring defautlApplicationSettingPath;

	/**
	return full path to application settings
	rule of application settings file name:
	[Full Path]\[Application Name].exe_app_settings.json
	*/
	std::wstring GetApplicationSettingPath();

	/*
	 save setting to json file '.application_setting.json'
	 if file '.application_setting.json' is not exist, create them

	 @param setting name
	 @param value

	 @return success
	 */
	bool SetSetting(const wchar_t *settingName, const wchar_t *value);

	bool SetSetting(const wchar_t *settingName, double value);

	bool SetSetting(const wchar_t *settingName, const TJSONValue *value);

	bool SetSetting(const wchar_t *settingName, bool value);

	/*
	 open file '.application_setting.json and read setting
	 do not forget to delete result buffer (TJSONValue *)

	 @return result value || NULL
	 */
	TJSONValue * GetSetting(const wchar_t *settingName);

	int GetSettingInt(const wchar_t *settingName, int defaultValue = 0);

	double GetSettingDouble(const wchar_t *settingName, double defaultValue = 0);

	String GetSettingString(const wchar_t *settingName, String defaultValue);

	bool GetSettingBool(const wchar_t *settingName, bool defaultValue);

	/*
	check is settings exists
	*/
	bool CheckSetting(const wchar_t *settingName);

	/*
	 check is OS is 64bit
	 */
	bool IsWow64();

	//save into application setting file, the form parameters
	void SaveFormSettings(TForm *form);

	//restore from application setting file, the form parameters and apply it to form
	void RestoreFormSettings(TForm *form);

	//save into application setting file, the control state
	//prefix it is optional parameters, by defaut it control name
	void SaveControlState(TControl *control, String prefix = NULL);

	void SaveControlsState(const std::list<TControl *> &controls, String prefix = NULL);

	void RestoreControlState(TControl *contro, String prefix = NULL);

	void RestoreControlsState(const std::list<TControl *> &controls, String prefix = NULL);
#endif
}

#endif