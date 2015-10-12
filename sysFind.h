//---------------------------------------------------------------------------

#ifndef sysFindH
#define sysFindH

//---------------------------------------------------------------------------

namespace sysFind {

struct TXMLSettings;

/*
	Find files in folders using criteria
	iterate over files by invoke IScanIterator object for each find file
*/
void Find(TXMLSettings &settings, sysFile::IScanIterator &scanIterator);

struct IXMLParse {
	virtual void Parse(IXMLNode  *node) = 0;
};

struct TXMLExclude : IXMLParse {
	String folder;

	TXMLExclude(){}

	TXMLExclude(IXMLNode *node) {
		Parse(node);
	}

	virtual void Parse(IXMLNode *node) {
		folder = node->GetAttribute("folder");
	}
};

struct TXMLInclude : IXMLParse {
	String folder;

	TXMLInclude(){}

	TXMLInclude(IXMLNode *node) {
		Parse(node);
    }

    virtual void Parse(IXMLNode *node) {
    	folder = node->GetAttribute("folder");
    }
};

struct TXMLCriteria : IXMLParse{
	enum Criteria {_unknown, _ext, _created, _size, _name, _age};
	enum Operation {_lt, _le, _eq, _ge, _gt, _like};
	enum Group {_and, _or};

    Criteria name;
    Operation op;
	String ext;
	DWORD size;
	Group group;
	String fileName;
	LONGLONG msecCreatedLocalTime;
	double dayCreatedLocalTime;
	double ageDays;

    TXMLCriteria(){
    	name = Criteria::_unknown;
        group = Group::_and;
    }

    TXMLCriteria(IXMLNode *node) {
    	Parse(node);
    }

	bool IsGood(const wchar_t *folder, const WIN32_FIND_DATAW &ffd) {
		if (name == Criteria::_unknown) {
			return true;
        }

    	if (name == Criteria::_ext && op == Operation::_eq) {
        	const wchar_t *ext = sysFile::GetFileExt(ffd.cFileName);
//			return wcscmp(this->ext.c_str(), ext) == 0;
			return _wcsicmp(this->ext.c_str(), ext) == 0;

        } else if (name == Criteria::_size) {
            DWORD fileSize = (ffd.nFileSizeHigh * (MAXDWORD+1)) + ffd.nFileSizeLow;
            if (op == Operation::_lt) {
				return fileSize < size;
            } else if (op == Operation::_le) {
            	return fileSize <= size;
			} else if (op == Operation::_eq) {
            	return fileSize == size;
			} else if (op == Operation::_ge) {
				return fileSize >= size;
			} else if (op == Operation::_gt) {
            	return fileSize > size;
			}

		} else if (name == Criteria::_created) {
			SYSTEMTIME stFileCreationTimeGMT;
			FileTimeToSystemTime(&(ffd.ftCreationTime), &stFileCreationTimeGMT);
			LONGLONG biasMSec = sysTime::GetLocalTimeBias() * sysTime::SEC2MSEC;
			LONGLONG msecFileCreationLocalTime = sysTime::SystemTimeToMSec(stFileCreationTimeGMT) - biasMSec;
			//LONGLONG msecCreated = sysTime::SystemTimeToMSec(createdLT);
            char compareResult = 0;
			if (msecFileCreationLocalTime > msecCreatedLocalTime) {
				compareResult = 1;
			} else if (msecFileCreationLocalTime < msecCreatedLocalTime) {
            	compareResult = -1;
			}
			//char compareResult = sysTime::CompareSystemTime(stFileCreationTimeGMT, createdLT);
            if (op == Operation::_lt) {
				return compareResult == -1;
            } else if (op == Operation::_le) {
            	return compareResult == -1 || compareResult == 0;
			} else if (op == Operation::_eq) {
				return compareResult == 0;
			} else if (op == Operation::_ge) {
            	return compareResult == 1 || compareResult == 0;
			} else if (op == Operation::_gt) {
            	return compareResult == 1;
			}
		} else if (name == Criteria::_name) {
			if (op == Operation::_eq) {
				return wcscmp(ffd.cFileName, fileName.c_str()) == 0;
			} else if (op == Operation::_like) {
				String originalFileName(ffd.cFileName);
				return originalFileName.Pos(fileName) != 0;
			}
		} else if (name == Criteria::_age) {
			SYSTEMTIME stFileCreationTimeGMT;
			FileTimeToSystemTime(&(ffd.ftCreationTime), &stFileCreationTimeGMT);
			LONGLONG biasMSec = sysTime::GetLocalTimeBias() * sysTime::SEC2MSEC;
			LONGLONG msecFileCreationLocalTime = sysTime::SystemTimeToMSec(stFileCreationTimeGMT) - biasMSec;

			SYSTEMTIME stLocalTime;
			GetLocalTime(&stLocalTime);
			LONGLONG msecLocalTime = sysTime::SystemTimeToMSec(stLocalTime);

			if (op == Operation::_gt) {
				return sysTime::MSEC2DAY * (msecLocalTime - msecFileCreationLocalTime) > ageDays;
			} else if (op == Operation::_lt) {
				return sysTime::MSEC2DAY * (msecLocalTime - msecFileCreationLocalTime) < ageDays;
			}
		}

		return false;
    }

    virtual void Parse(IXMLNode *node) {
    	String name = node->GetAttribute("name");
    	String op = node->GetAttribute("op");
    	String value = node->GetAttribute("value");
        String group = node->GetAttribute("group");

        if (name == "ext") {
			this->name = Criteria::_ext;
            ext = value;
        } else if (name == "size") {
        	this->name = Criteria::_size;
        	size = StrToInt(value);
		} else if (name == "created") {
        	this->name = Criteria::_created;
			DWORD day;
            DWORD month;
            DWORD year;
            DWORD hours;
			DWORD minutes;
			DWORD seconds;
			swscanf(value.c_str(), L"%2d.%2d.%4d %2d:%2d:%2d", &day, &month, &year, &hours, &minutes, &seconds);

			TDateTime __dt(year, month, day, hours, minutes, seconds, 0);
			dayCreatedLocalTime = __dt.Val;

			SYSTEMTIME createdLT;
            createdLT.wDay = day;
            createdLT.wMonth = month;
            createdLT.wYear = year;

            createdLT.wHour = hours;
            createdLT.wMinute = minutes;
            createdLT.wSecond = seconds;
            createdLT.wMilliseconds = 0;

            msecCreatedLocalTime = sysTime::SystemTimeToMSec(createdLT);
		} else if (name == "age") {
			this->name = Criteria::_age;
			this->ageDays = StrToFloat(value);
		}

        if (op == "lt") {
			this->op = Operation::_lt;
        } else if (op == "le") {
        	this->op = Operation::_le;
		} else if (op == "eq") {
        	this->op = Operation::_eq;
		} else if (op == "ge") {
        	this->op = Operation::_ge;
		} else if (op == "gt") {
        	this->op = Operation::_gt;
		}

        if (group == "and") {
			this->group = Group::_and;
        } else if (group == "or") {
        	this->group = Group::_or;
		}
    }
};

struct TXMLOperation : IXMLParse, sysFile::IScanIterator {
	enum Operation {_find};
    enum Entity {_file};

    bool abort;

    //list of criteria for finding
	std::vector<TXMLCriteria> criteriesAnd;
    std::vector<TXMLCriteria> criteriesOr;
    //list of excluded folders
    std::vector<TXMLExclude> excludes;
    //list of included folders
    std::vector<TXMLInclude> includes;
    //what to find
    Entity entity;
	//type of operation
    Operation type;

    sysFile::IScanIterator *externalScanIterator;

    TXMLOperation(){
    	abort = false;
    }

	TXMLOperation(const TXMLOperation &operation) {
    	type = operation.type;
        criteriesAnd = operation.criteriesAnd;
        criteriesOr = operation.criteriesOr;
        excludes = operation.excludes;
        includes = operation.includes;
        abort = operation.abort;
    }

	TXMLOperation (IXMLNode *node) {
		abort = false;
    	if (node->NodeName == "operation") {
			Parse(node);
        }
    }

    virtual void Parse(IXMLNode *nodeOperation) {
    	String type = nodeOperation->GetAttribute("type");
        if (type == "find") {
			this->type = Operation::_find;
        }

        String entity = nodeOperation->GetAttribute("entity");
        if (entity == "file") {
			this->entity = Entity::_file;
        }

        IXMLNodeList *listNodes = nodeOperation->ChildNodes;
        for (int itNode = 0; itNode < listNodes->GetCount(); ++itNode) {
            IXMLNode *node = listNodes[0].Get(itNode);
            if (node->NodeName == "criteria") {
            	TXMLCriteria criteria(node);
                if (criteria.group == TXMLCriteria::Group::_and) {
                  criteriesAnd.push_back(criteria);
                } else if (criteria.group == TXMLCriteria::Group::_or) {
					criteriesOr.push_back(criteria);
				}

            } else if (node->NodeName == "exclude") {
            	excludes.push_back(TXMLExclude(node));

			} else if (node->NodeName == "include") {
            	includes.push_back(TXMLInclude(node));
			}
        }
    }

    void Execute(sysFile::IScanIterator &scanIterator){
    	if (type == Operation::_find) {
			Find(scanIterator);
        }
    }

	virtual bool ScanIterateCallback(const wchar_t *folder, const WIN32_FIND_DATAW &ffd, void *data = NULL) {
		bool bFilePass = true;
		//check all and criteria
		for (int itCriteria = 0, size = criteriesAnd.size(); itCriteria < size; ++itCriteria) {
			if (!criteriesAnd[itCriteria].IsGood(folder, ffd)) {
				bFilePass = false;
				break;
			}
		}
		if (criteriesOr.size() && bFilePass) {
			bFilePass = false;
			//check at least 1 or criteria
			for (int itCriteria = 0, size = criteriesOr.size(); itCriteria < size; ++itCriteria) {
				if (criteriesOr[itCriteria].IsGood(folder, ffd)) {
					bFilePass = true;
					break;
				}
			}
		}

		externalScanIterator->ScanIterateCallback(folder, ffd, (void *)bFilePass);

		return abort;
	}

    /*
    Find all objects correspond with settings
	*/
    void Find(sysFile::IScanIterator &scanIterator){
    	externalScanIterator = &scanIterator;
    	bool bRecursive = true;
        std::vector<std::wstring> excludeFolders;
        for (size_t itExclude = 0; itExclude < excludes.size(); ++itExclude) {
			excludeFolders.push_back(std::wstring(excludes[itExclude].folder.c_str()));
        }

    	for (size_t itInclude = 0; itInclude < includes.size(); ++itInclude) {
        	sysFile::ScanFiles(includes[itInclude].folder.c_str(), L"*.*", *this, bRecursive, abort, &excludeFolders);
        }
    }

    void Delete(){
    }

    void AbortOperation(){
		abort = true;
    }
};

struct TXMLSettings : IXMLParse {
	std::vector<TXMLOperation> operations;

    TXMLSettings(){}

    TXMLSettings(IXMLNode *node) {
        Parse(node);
    }

    virtual void Parse(IXMLNode *nodeSettings) {
        IXMLNodeList *listOperations = nodeSettings->ChildNodes;
        for (int itNode = 0; itNode < listOperations->GetCount(); ++itNode) {
			IXMLNode *node = listOperations[0].Get(itNode);
			operations.push_back(TXMLOperation(node));
        }
    }

    void Clear() {
		operations.clear();
	}

	void LoadFromFile(const wchar_t *settingsXML) {
		TXMLDocument *xmlDocument = new TXMLDocument(Application);
		xmlDocument->DOMVendor = GetDOMVendor("MSXML");
		xmlDocument->Active = true;
		xmlDocument->Encoding = "UTF-8";

		xmlDocument->LoadFromFile(settingsXML);

		Parse(xmlDocument->DocumentElement);
	}
};

/*
@example
TXMLDocument *xmlDocument = new TXMLDocument(Application);
InitXMLDocument(xmlDocument)

before invoke function, allocate memmory for xmlDocument
Initialize xmlDocument
*/
TXMLDocument * InitXMLDocument(TXMLDocument *xmlDocument);

/*
Parse setting.xml file
before invoke function ensure that you specify locale
@example
    setlocale(LC_ALL, ".1251");
    ParseSettingXML("C:\temp\setting.xml", xmlDocument);
*/
TXMLSettings ParseSettingXML(const wchar_t *file, TXMLDocument *xmlDocument);

};

#endif

