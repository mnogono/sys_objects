//---------------------------------------------------------------------------

#ifndef sysOOH
#define sysOOH

#ifdef __BORLANDC__

#include <vcl.h>

#include "sysReg.h"
#include "sysFile.h"
#include "sysApp.h"
#include "sysProcess.h"
//---------------------------------------------------------------------------

namespace sysOO {

	/*
	check is open office installed?
	sOffiseFile - path to executable file
    example IsOpenOfficeInstalled("4.1.1", sOfficeFile);
    */
	bool IsOpenOfficeInstalled(String openOfficeVersion, String &sOfficeFile);

    /*
    Start build the report
    All necessary SkyReport extension lib must be installed into open office before execute
    */
	void BuildReport(const String &sOfficeFile, const String &sSettingsXMLFile);

	/*
	open file in open office
	*/
	bool OpenFile(const String &sOfficeInstalledPath, const wchar_t *file);
};

#endif

#endif

