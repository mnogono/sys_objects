//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#pragma package(smart_init)

#ifdef __BORLANDC__

namespace sysOO {

//---------------------------------------------------------------------------
bool IsOpenOfficeInstalled(String openOfficeVersion, String &path) {
	bool isOpenOfficeInstalled = false;

    wchar_t wcsOO4InstallPath[1024];
    memset(&wcsOO4InstallPath[0], 0, sizeof(wchar_t) * 1024);
    if (sysApp::IsWow64()) {
    	String RegPath = String(L"SOFTWARE\\Wow6432Node\\OpenOffice\\OpenOffice\\") + openOfficeVersion;
    	wcscpy(&wcsOO4InstallPath[0], RegPath.c_str());
    } else {
	    String RegPath = String(L"SOFTWARE\\OpenOffice\\OpenOffice\\") + openOfficeVersion;
		wcscpy(&wcsOO4InstallPath[0], RegPath.c_str());
    }

    wchar_t *__path = NULL;
    if (sysReg::RegQueryStringValue(HKEY_LOCAL_MACHINE, wcsOO4InstallPath, L"Path", __path)) {
		//check is file soffice.exe exist
        if (sysFile::IsFileExist(__path)) {
        	path = String(__path);
        	isOpenOfficeInstalled = true;
        }
    }

    if (__path) {
	    delete []__path;
    }

    return isOpenOfficeInstalled;
}

void BuildReport(const String &sOfficeInstalledPath, const String &sSettingsXMLFile) {
	TStringBuilder *parameters = new TStringBuilder();

    parameters->Clear();
    parameters->Append("\"")->Append(sOfficeInstalledPath)->Append("\"")
        ->Append(" \"macro:///SiamsReportBuilder.Writer.BuildReport(")->Append(sSettingsXMLFile)->Append(")\"");
        //-headless for some reason when this parameter exist, soffice stubs on winXP

	sysProcess::ExecuteCmd(NULL, parameters->ToString().c_str(), 30000);
	delete parameters;
}

bool OpenFile(const String &sOfficeInstalledPath, const wchar_t *file) {
	TStringBuilder *parameters = new TStringBuilder();

	parameters->Clear();
	parameters->Append("\"")->Append(sOfficeInstalledPath)->Append("\" \"")->Append(String(file))->Append("\"");

	bool res = sysProcess::ExecuteCmd(NULL, parameters->ToString().c_str());

	delete parameters;
}

}

#endif




