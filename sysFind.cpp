//---------------------------------------------------------------------------
#include "pch.h"
#pragma hdrstop
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace sysFind {

void Find(const wchar_t *settingXMLFile, sysFile::IScanIterator &scanIterator) {
    TXMLDocument *xmlDocument = new TXMLDocument(Application);

    xmlDocument = InitXMLDocument(xmlDocument);

	TXMLSettings xmlSettings = ParseSettingXML(settingXMLFile, xmlDocument);

	Find(xmlSettings, scanIterator);
}

void Find(TXMLSettings &xmlSettings, sysFile::IScanIterator &scanIterator) {
	for (size_t itOperation = 0; itOperation < xmlSettings.operations.size(); ++itOperation) {
        xmlSettings.operations[itOperation].Execute(scanIterator);
    }
}

TXMLDocument * InitXMLDocument(TXMLDocument *xmlDocument) {
    xmlDocument->DOMVendor = GetDOMVendor("MSXML");
    xmlDocument->Active = true;
    xmlDocument->Encoding = "UTF-8";
    return xmlDocument;
}

TXMLSettings ParseSettingXML(const wchar_t *file, TXMLDocument *xmlDocument){
	xmlDocument->LoadFromFile(file);

    return TXMLSettings(xmlDocument->DocumentElement);
}

};

