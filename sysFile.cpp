#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysFile {

	VS_FIXEDFILEINFO * GetExeVersionRoot(wchar_t *fileName){
		DWORD infoSize = GetFileVersionInfoSize(fileName, 0);
		if (!infoSize) return NULL;

		char * fileInfo = new char[infoSize];
		if (!GetFileVersionInfo(fileName, 0, infoSize, fileInfo)){
			delete []fileInfo;
			return NULL;
		}

		unsigned char *pVersionData = 0;
		unsigned int len;
		if (!VerQueryValue(fileInfo, L"\\", (void **)&pVersionData, &len)){
			delete []fileInfo;
			return NULL;
		}

		VS_FIXEDFILEINFO *fixedFileInfo = (VS_FIXEDFILEINFO *)pVersionData;

		delete []fileInfo;

		return fixedFileInfo;
	}

	std::wstring getExeVersion(const wchar_t *fileName, WORD codePage) {

		DWORD  verHandle = NULL;
		UINT   size      = 0;
		LPBYTE lpBuffer  = NULL;
		DWORD  verSize   = GetFileVersionInfoSize( fileName, &verHandle);

		if (verSize != NULL)
		{
			LPSTR verData = new char[verSize];

			if (GetFileVersionInfo( fileName, verHandle, verSize, verData))
			{
				if (VerQueryValue(verData,L"\\",(VOID FAR* FAR*)&lpBuffer,&size))
				{
					if (size)
					{
						VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
						if (verInfo->dwSignature == 0xfeef04bd) {
							std::wstring version(255, 0);
							size_t n = swprintf(
								&version[0],
								version.size(),
								L"%d.%d.%d.%d",
								( verInfo->dwFileVersionMS >> 16 ) & 0xffff,
								( verInfo->dwFileVersionMS >>  0 ) & 0xffff,
								( verInfo->dwFileVersionLS >> 16 ) & 0xffff,
								( verInfo->dwFileVersionLS >>  0 ) & 0xffff
							);
							version.resize(n);
							delete[] verData;
    						return version;
						}
					}
				}
			}

			delete[] verData;
		}


		return std::wstring(L"");
	}

#ifdef __BORLANDC__
	TExeFileInfo * GetExeVersion(wchar_t *fileName, WORD codePage){
		if (!IsFileExist(fileName)) {
			throw sysException::TExceptionFileNotFound(fileName);
		}

		DWORD infoSize = GetFileVersionInfoSizeW(fileName, 0);
		if (!infoSize) throw TExceptionCantGetFileInfo(fileName);

		char *infoBuf = new char[infoSize];

		try {


			std::vector<wchar_t *> info;

			info.push_back(L"Comments");
			info.push_back(L"CompanyName");
			info.push_back(L"FileDescription");
			info.push_back(L"FileVersion");

			info.push_back(L"InternalName");
			info.push_back(L"LegalCopyright");
			info.push_back(L"LegalTrademarks");
			info.push_back(L"OriginalFilename");

			info.push_back(L"ProductName");
			info.push_back(L"ProductVersion");
			info.push_back(L"PrivateBuild");
			info.push_back(L"SpecialBuild");

			if (!GetFileVersionInfoW(fileName, 0, infoSize, infoBuf)){
				throw TExceptionCantGetFileInfo(fileName);
			}

			struct LANGANDCODEPAGE {
			  WORD wLanguage;
			  WORD wCodePage;
			} *lpTranslate;

			unsigned int langLen;
			if (!VerQueryValue(infoBuf, L"\\VarFileInfo\\Translation", (void **)&lpTranslate, &langLen)){
				throw TExceptionCantGetFileInfoQueryValue(fileName);
			}

			TExeFileInfo *exeFileInfo = new TExeFileInfo();

			//std::map<wchar_t *,
			for (int itLang = 0, langCount = langLen / sizeof(struct LANGANDCODEPAGE);
				itLang < langCount; ++itLang) {
				if (lpTranslate->wCodePage == codePage || codePage == 0) {

					wchar_t infoKey[300];
					wsprintfW(infoKey, L"\\StringFileInfo\\%04x%04x\\",
						lpTranslate->wLanguage, lpTranslate->wCodePage);
					unsigned int infoValueSize;
					//std::map<std::wstring, wchar_t *> *mapFileInfo = new std::map<std::wstring, wchar_t *>();
					for (std::vector<wchar_t *>::iterator it = info.begin(), itEnd = info.end(); it != itEnd; ++it) {
						wsprintfW(&infoKey[25], L"%s", *it);
						wchar_t *infoValue;
						if (VerQueryValue(infoBuf, infoKey, (void **)&infoValue, &infoValueSize)){
							exeFileInfo->Set(*it, infoValue);
							//(*mapFileInfo)[std::wstring(*it)] = infoValue;
						} else {
							//(*mapFileInfo)[std::wstring(*it)] = NULL;
						}
						//delete []infoValue;
					}

					//return mapFileInfo;
					return exeFileInfo;
				}
			}

			throw TExceptionCantGetFileInfo(fileName);

		} __finally {
			delete []infoBuf;
		}

        return NULL;
	}
#endif

	bool IsFileExist(const char *file) {
	  DWORD ftyp = GetFileAttributesA(file);
	  if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	  if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY))
		return true;   // this is not a directory!

	  return false;    // this is not a file
	}

	bool IsFileExist(const wchar_t * file){
	  DWORD ftyp = GetFileAttributesW(file);
	  if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	  if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY))
		return true;   // this is not a directory!

	  return false;    // this is not a file
	}

    bool IsFolderExist(const wchar_t * folder) {
		DWORD ftyp = GetFileAttributesW(folder);
		if (ftyp == INVALID_FILE_ATTRIBUTES) {
			return false;  //something is wrong with your path!
		}

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
			return true;
		}

		return false; //this is not a folder
	}

	bool IsFolderExist(const char *folder) {
		DWORD ftyp = GetFileAttributesA(folder);
		if (ftyp == INVALID_FILE_ATTRIBUTES) {
			return false;  //something is wrong with your path!
		}

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
			return true;
		}

		return false; //this is not a folder
	}

	bool IsFolderEmpty(const wchar_t *folder) {
		WIN32_FIND_DATAW ffd;

		HANDLE hFind = INVALID_HANDLE_VALUE;

		wchar_t wBuffer[MAX_PATH];

		swprintf(wBuffer, MAX_PATH, L"%s\\*", folder);

		hFind = FindFirstFileW(wBuffer, &ffd);
		if (hFind == INVALID_HANDLE_VALUE) return false;

		bool isFolder;

        bool isFolderEmpty = true;

		do {
			isFolder = ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
			if (isFolder) {
				if (wcscmp(ffd.cFileName, L".") == 0) continue;
				else if (wcscmp(ffd.cFileName, L"..") == 0) continue;

				return false;
			} else {
				return false;
			}

		} while (FindNextFileW(hFind, &ffd));

		FindClose(hFind);

		return isFolderEmpty;
	}

	long GetFileSize(const wchar_t *file){
		struct _stat st;
		_wstat(file, &st);
		return st.st_size;
	}

	size_t GetFileSize(const char *file){
		struct _stat st;
		_stat(file, &st);
		return st.st_size;
	}

#ifdef __BORLANDC__
	__int64 GetFileSize64(const wchar_t *file){
		struct stati64 st;
		_wstati64(file, &st);
		return st.st_size;
	}

	__int64 GetFileSize64(const char *file){
		struct stati64 st;
		_stati64(file, &st);
		return st.st_size;
	}

    void FileToByteBuffer(const wchar_t *file, unsigned char *buffer) {
		std::ifstream in(file, std::ios_base::binary);

		try {
        	long itByteCount = 0;
			while (in.good()){
				unsigned char b;
				in.read((char *)&b, 1);
				buffer[itByteCount++] = b;
			}
		} __finally {
			in.close();
		}
    }
#endif

	void BinaryToStream(const unsigned char *buffer, int size, std::ostream *out) {
		char hexBuffer[] = {0,0};

        for (int itByte = 0; itByte < size; ++itByte) {
            sprintf(hexBuffer, "%.2X", buffer[itByte]);
            *out << hexBuffer;
		}
    }

#ifdef __BORLANDC__
	void BinaryToStream(const wchar_t *file, std::ostream *out) {
		std::ifstream in(file, std::ios_base::binary);

		try {
			unsigned char buffer;
			char hexBuffer[] = {0,0};
			int readedBytes;

			while (in.good()){
				in.read((char *)&buffer, 1);
                if (!in.good()) {
					break;
                }
				sprintf(hexBuffer, "%.2X", buffer);
				*out << hexBuffer;
			}

		} __finally {
			in.close();
		}
	}

	char * BinaryToString(const wchar_t *file) {
		long iFileSize = GetFileSize(file);
		long iBufferSize = iFileSize * 2 + 1;
		char *cBuffer = new char[iBufferSize];
		memset(cBuffer, 0, iBufferSize * sizeof(char));

		return BinaryToString(file, cBuffer);
	}
#endif

#ifdef __BORLANDC__
	char * BinaryToString(const wchar_t *file, char *out) {
		std::ifstream in(file, std::ios_base::binary);

		//long iFileSize = GetFileSize(file);
		//long iBufferSize = iFileSize * 2 + 1;
		//char *cBuffer = new char[iBufferSize];
		//memset(cBuffer, 0, iBufferSize * sizeof(char));
		try {
			unsigned char byte;
			char hexBuffer[] = {0,0};
			long iByteReaded = 0;
			while (in.good()){
				in.read((char *)&byte, sizeof(unsigned char));
                if (!in.good()) {
					break;
                }
				sprintf(hexBuffer, "%.2X", byte);
				out[iByteReaded * 2] = hexBuffer[0];
				out[iByteReaded * 2 + 1] = hexBuffer[1];
				++iByteReaded;
			}

		} __finally {
			in.close();
		}

		return out;
	}

	char * ByteBufferToString(const TByteDynArray &buffer) {
		char *cBuffer = new char[buffer.Length * 2 + 1];
		//cBuffer[buffer.Length * 2] = 0;
		//memset(cBuffer, 0, buffer.Length * 2 + 1);
		char hexBuffer[] = {0,0,0};
		for (int i = 0, size = buffer.Length; i < size; ++i) {
			sprintf(hexBuffer, "%.2X", buffer[i]);
			cBuffer[i * 2] = hexBuffer[0];
			cBuffer[i * 2 + 1] = hexBuffer[1];
		}
		cBuffer[buffer.Length * 2] = 0;
		return cBuffer;
	}
#endif

	SYSTEMTIME GetFileLastModifyDateTimeUTC(const wchar_t *file){
		HANDLE hFile = CreateFileW(
			file,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			SECURITY_ANONYMOUS);

		SYSTEMTIME stUTC;

		FILETIME lpCreationTime;
		FILETIME lpLastAcessTime;
		FILETIME lpLastWriteTime;

		FILETIME ftCreate, ftAccess, ftWrite;

		if (GetFileTime(hFile, &lpCreationTime, &lpLastAcessTime, &lpLastWriteTime)){
			FileTimeToSystemTime(&lpLastWriteTime, &stUTC);
		}

		CloseHandle(hFile);

		//return sysTime::SystemTimeToDateTime(&stUTC);
        return stUTC;
	}


	/*
	void GetRegValue(HKEY hKey) {
		RegQueryValueEx(hKey);
	}
	*/

	std::wstring GetExecutablePath(HMODULE hModule) {
		wchar_t path[MAX_PATH + 1];

		if (!GetExecutablePath(path, MAX_PATH, hModule)) {
			return std::wstring(L"");
		}

		return std::wstring(path);
	}

	bool GetExecutablePath(wchar_t *outPath, int maxPathLen, HMODULE hModule) {
		DWORD size = GetModuleFileNameW(hModule, outPath, maxPathLen);
		if (size == 0) {
			return false;
		}

		outPath[size] = 0;

		return true;
	}

	wchar_t * GetAbsolutePath(const wchar_t *relativePath) {
		wchar_t *base = GetExecutableFolder();
		wchar_t *absolute = new wchar_t[MAX_PATH];
		swprintf(absolute, L"%s\\%s", base, relativePath);
		delete []base;

		return absolute;
	}

	wchar_t *GetAbsolutePath(const wchar_t *relativePath, const wchar_t delim) {
		wchar_t *absolute = GetAbsolutePath(relativePath);
		if (delim == L'\\') {
			return sysStr::ReplaceAll(absolute, L'/', delim);
		}
		
		return sysStr::ReplaceAll(absolute, L'\\', delim);
	}

	char * GetAbsolutePath(const char *relativePath) {
		char *base = GetExecutableFolderA();
		char *absolute = new char[MAX_PATH];
		sprintf(absolute, "%s\\%s", base, relativePath);
		delete []base;

		return absolute;
	}

	char * GetAbsolutePath(const char *relativePath, const char delim) {
		char *absolute = GetAbsolutePath(relativePath);
		if (delim == '\\') {
			return sysStr::ReplaceAll(absolute, '/', delim);
		}

		return sysStr::ReplaceAll(absolute, '\\', delim);
	}
	/*
	std::string GetAbsolutePath(const std::string &relativePath) {
		char *absolute = GetAbsolutePath(relativePath.c_str());
		std::string strAbsolute = absolute;
		delete []absolute;
		return strAbsolute;
	}
	*/

    wchar_t * GetExecutableFolder(HMODULE hModule) {
    	wchar_t *buffer = new wchar_t[MAX_PATH];
        memset(buffer, 0, sizeof(wchar_t) * MAX_PATH);
		if (GetExecutablePath(buffer, MAX_PATH, hModule)) {
			buffer = TrunkFileName(buffer, wcslen(buffer));
        }

        return buffer;
    }

	char * GetExecutableFolderA() {
		wchar_t *w = GetExecutableFolder(NULL);
		char *s;
		sysStr::WToMB(w, s);

		delete []w;

		return s;
	}

    void GetLastFolderName(wchar_t *path, wchar_t *out) {

    }

	wchar_t * GetFilePath(wchar_t *path) {
		wchar_t *folder = new wchar_t[MAX_PATH];

		GetFilePath(path, folder);

		return folder;
	}

	void GetFilePath(wchar_t *path, wchar_t *out) {
		std::vector<std::wstring> folders;
		sysStr::Split(path, L"\\", folders);
		//out = sysStr::JoinString(folders.begin(), folders.end() - 1, L"\\");
        sysStr::JoinStrings(folders.begin(), folders.end() - 1, out, L"\\");
	}

	wchar_t *TrunkFileName(wchar_t *path, size_t len) {
		if (!len) len = wcslen(path);
		if (!len) return path;

		for (int i = len - 1; i >= 0; --i) {
			if (path[i] == L'\\') {
				path[i] = 0;
				return path;
			}
		}

		return path;
	}

    const wchar_t * GetFileExt(const wchar_t *file) {
        size_t size = wcslen(file);
        for (; size != 0; --size) {
			if (file[size] == L'.') {
				break;
            }
        }
        return &file[++size];
    }

	/*
	const wchar_t * const GetFileName(wchar_t *path, size_t len){
		if (!len) len = wcslen(path);
		if (!len) return path;
		if (path[len-1] == L'\\') return path;

		for (int i = len - 1; i >= 0; --i) {
			if (path[i] == L'\\') {
				return &path[i + 1];
			}
		}

		return NULL;
	}
	*/
	std::wstring GetFileName(const wchar_t *path) {
		int len = wcslen(path);
		if (!len) {
			return std::wstring(L"");
		}

		for (int i = len - 1; i >= 0; --i) {
			if (path[i] == L'\\') {
				std::wstring fileName(&path[i+1]);
				return fileName;
			}
		}

		return std::wstring(L"");
	}

	std::wstring GetFolder(const wchar_t *path) {
		int len = wcslen(path);
		if (!len) {
			return std::wstring(L"");
		}

		for (int i = len - 1; i >= 0; --i) {
			if (path[i] == L'\\') {
				std::wstring folder(&path[0], &path[i]);
				return folder;
			}
		}

		return std::wstring(L"");
	}

#ifdef __BORLANDC__
    String GetFileNameWithoutExt(String path) {
    	for (int i = path.Length(); i >= 0; --i) {
			if (path[i] == L'.') {
				return path.SubString(0, path.Length() - i);
			}
		}

		return "";
	}
#endif

    //void ScanFiles(const wchar_t *dir, const wchar_t *filter, FileIteratorCallback fileIterator, bool bRecursive, const std::vector<std::wstring> *excludeFolders, void *data) {
	void ScanFiles(const wchar_t *folder, const wchar_t *filter, IScanIterator &scanIterator, bool bRecursive, const std::vector<std::wstring> *excludeFolders) {
    	if (excludeFolders) {
        	size_t lenOfExcludeFolder;
			for (size_t itExclude = 0; itExclude < excludeFolders->size(); ++itExclude) {
				lenOfExcludeFolder = wcslen((*excludeFolders)[itExclude].c_str());
                if (wcsncmp(folder, (*excludeFolders)[itExclude].c_str(), lenOfExcludeFolder) == 0) {
					return;
                }
            }
        }

		WIN32_FIND_DATAW ffd;

        HANDLE hFile = INVALID_HANDLE_VALUE;

		wchar_t wBuffer[MAX_PATH];

        swprintf(wBuffer, MAX_PATH, L"%s%s", folder, filter);

        hFile = FindFirstFileW(wBuffer, &ffd);
        if (hFile == INVALID_HANDLE_VALUE) {
			return;
        }

		bool next = true;
		bool isFolder;
		do {
			if (wcscmp(ffd.cFileName, L".") == 0) {
				continue;
			} else if (wcscmp(ffd.cFileName, L"..") == 0) {
				continue;
			}

			isFolder = ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
			if (isFolder) {
				swprintf(wBuffer, MAX_PATH, L"%s%s\\", folder, ffd.cFileName);
				ScanFiles(wBuffer, filter, scanIterator, bRecursive, excludeFolders);
			} else {
				next = scanIterator.ScanIterateCallback(folder, ffd);
			}
		} while (FindNextFileW(hFile, &ffd) && next == true);

        FindClose(hFile);
	}

    void GetFiles(const wchar_t *dir, std::vector<std::wstring> &files, const wchar_t *filter) {
		WIN32_FIND_DATAW ffd;

		HANDLE hFind = INVALID_HANDLE_VALUE;

		wchar_t wBuffer[MAX_PATH];

		swprintf(wBuffer, MAX_PATH, L"%s\\%s", dir, filter);

		hFind = FindFirstFileW(wBuffer, &ffd);
		if (hFind == INVALID_HANDLE_VALUE) return;

        bool isFolder;

		do {
        	isFolder = ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

			if (!isFolder) {
				files.push_back(ffd.cFileName);
			}

		} while (FindNextFileW(hFind, &ffd));

        FindClose(hFind);
    }

	void GetFolders(const wchar_t *dir, std::vector<std::wstring> &subFolders) {
		WIN32_FIND_DATAW ffd;
//		ffd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;

		HANDLE hFind = INVALID_HANDLE_VALUE;

		wchar_t wBuffer[MAX_PATH];

		swprintf(wBuffer, L"%s\\*", dir);

		hFind = FindFirstFileW(wBuffer, &ffd);
		if (hFind == INVALID_HANDLE_VALUE) return;

		do {
//			if (ffd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
				if (wcscmp(ffd.cFileName, L".") == 0) continue;
				else if (wcscmp(ffd.cFileName, L"..") == 0) continue;

				std::wstring sFile = ffd.cFileName;
				subFolders.push_back(sFile);
			}

		} while (FindNextFileW(hFind, &ffd));

        FindClose(hFind);


		//static unsigned char LEN_DATE_FOLDER_NAME = 8;
		/*
		DIR *dp;
		struct dirent *dirp;
		if ((dp = opendir(dir)) == NULL) {
			return;
		}
		long lDateFile;
		while ((dirp = readdir(dp)) != NULL) {
			if (strcmp(dirp->d_name, ".") == 0) {
				continue;
			}
			if (strcmp(dirp->d_name, "..") == 0) {
				continue;
			}
			if (strlen(dirp->d_name) == LEN_DATE_FOLDER_NAME) {
				lDateFile = atol(dirp->d_name);
				folders->push_back(lDateFile);
			}
		}
		*/
	}

    bool DeleteFile(const char *file) {
        return remove(file) == 0;
    }

    bool DeleteFile(const wchar_t *file) {
    	return _wremove(file) == 0;
    }


    /*
    bool MoveFolder(const wchar_t *srcFolder, const wchar_t *destFolder, int &iOperationResult){
        SHFILEOPSTRUCT fop = {};
        fop.wFunc = FO_MOVE;

        fop.pFrom = srcFolder;
        fop.pTo = destFolder;

        iOperationResult = SHFileOperation(&fop);

        return iOperationResult == 0;
    }
    */

    bool DeleteFolder(const wchar_t *folder) {
    	wchar_t wsFolder[1024];

        wcscpy(wsFolder, folder);

    	wchar_t wsFile[1024];

        wcscpy(wsFile, folder);

        int iFolderLen = wcslen(folder);

		std::vector<std::wstring> files;

        GetFiles(folder, files, L"*.*");

        bool isAllDataDeleted = true;

        for (int itFile = 0, len = files.size(); itFile < len; ++itFile) {
        	wsprintfW(&wsFile[iFolderLen], L"\\%s", files[itFile].c_str());
			isAllDataDeleted = isAllDataDeleted && DeleteFile(wsFile);
        }

		std::vector<std::wstring> folders;

        GetFolders(folder, folders);

		for (int itFolder = 0, len = folders.size(); itFolder < len; ++itFolder) {
        	wsprintfW(&wsFolder[iFolderLen], L"\\%s", folders[itFolder].c_str());
           isAllDataDeleted = isAllDataDeleted && DeleteFolder(wsFolder);
        }

        if (isAllDataDeleted) {
		   isAllDataDeleted = isAllDataDeleted && RemoveDirectoryW(folder);
		}

		return isAllDataDeleted;
    }

    bool MoveFolder(const wchar_t *srcFolder, const wchar_t *destFolder){
    	if (CopyFolder(srcFolder, destFolder)) {
	        return DeleteFolder(srcFolder);
        }

        return false;
    }

    bool CopyFile(const wchar_t *srcFile, const wchar_t *destFile) {

    	std::ifstream in(srcFile, std::ios_base::binary | std::ios_base::in);

        if (!in.good() || !in.is_open()) {
			return false;
        }

        std::ofstream out(destFile, std::ios_base::binary | std::ios_base::out);
		if (!out.good()) {
			return false;
        }

        char buffer[1024];
        int readedBytes;

        while (!in.eof()) {
			in.read(buffer, 1024);
            readedBytes = in.gcount();
            out.write(buffer, readedBytes);
        }

        out.close();

        in.close();

        return true;
    }

    bool CopyFiles(const wchar_t *srcFolder, const wchar_t *destFolder, const wchar_t *filter){
    	wchar_t wsSrcFile[1024];

        int iSrcFolderLen = wcslen(srcFolder);

        wcscpy(wsSrcFile, srcFolder);

        wchar_t wsDestFile[1024];

        int iDestFolderLen = wcslen(destFolder);

        wcscpy(wsDestFile, destFolder);

        std::vector<std::wstring> files;

        GetFiles(srcFolder, files, filter);

        bool isAllFilesCopied = true;

        for (int itFile = 0, len = files.size(); itFile < len; ++itFile) {
        	wsprintfW(&wsSrcFile[iSrcFolderLen], L"\\%s", files[itFile].c_str());
        	wsprintfW(&wsDestFile[iDestFolderLen], L"\\%s", files[itFile].c_str());
			isAllFilesCopied = isAllFilesCopied && CopyFile(wsSrcFile, wsDestFile);
        }

        return isAllFilesCopied;
    }

    bool CopyFolder(const wchar_t *srcFolder, const wchar_t *destFolder){
    	wchar_t wsDestFolder[1024];

        int iDestFolderLen = wcslen(destFolder);

        wcscpy(wsDestFolder, destFolder);

    	int iLastFolderIndex = sysStr::LastIndexOf(srcFolder, L'\\');
        if (iLastFolderIndex != -1) {
			wsprintfW(&wsDestFolder[iDestFolderLen], L"\\%s", &srcFolder[iLastFolderIndex + 1]);
        }

        if (!IsFolderExist(wsDestFolder)) {
			CreateDirectoryW(wsDestFolder, NULL);
        }

        return __CopyFolder(srcFolder, wsDestFolder);
    }

    bool __CopyFolder(const wchar_t *srcFolder, const wchar_t *destFolder){

    	wchar_t wsSrcFolder[1024];

        int iSrcFolderLen = wcslen(srcFolder);

        wcscpy(wsSrcFolder, srcFolder);

    	wchar_t wsDestFolder[1024];

        int iDestFolderLen = wcslen(destFolder);

        wcscpy(wsDestFolder, destFolder);

		std::vector<std::wstring> folders;

        GetFolders(srcFolder, folders);

        bool isAllFilesCopied = true;

        for (int itFolder = 0, len = folders.size(); itFolder < len; ++itFolder) {

        	wsprintfW(&wsDestFolder[iDestFolderLen], L"\\%s", folders[itFolder].c_str());

			if (!sysFile::IsFolderExist(wsDestFolder)) {
				CreateDirectoryW(wsDestFolder, NULL);
            }

            if (!sysFile::IsFolderExist(wsDestFolder)) {
				return false;
            }

            wsprintfW(&wsSrcFolder[iSrcFolderLen], L"\\%s", folders[itFolder].c_str());

            isAllFilesCopied = isAllFilesCopied && __CopyFolder(wsSrcFolder, wsDestFolder);
        }

        isAllFilesCopied = isAllFilesCopied && CopyFiles(srcFolder, destFolder, L"*.*");

        return isAllFilesCopied;
    }

	#ifdef __BORLANDC__

	void WriteToFile(wchar_t *destFile, TByteDynArray &data) {
		std::ofstream out(destFile, std::ios_base::binary | std::ios_base::out);
        for (int i = 0; i < data.Length; ++i) {
			out.write((char *)&(data[i]), sizeof(char));
        }
        out.close();
    }

	#endif

	void WriteToFile(const wchar_t *destFile, const char *buffer, size_t size) {
		if (!size) {
			size = strlen(buffer);
		}
		std::ofstream out(destFile, std::ios_base::binary | std::ios_base::out);
		out.write(buffer, size);
		out.close();
	}

	void AppendToFile(const wchar_t *destFile, const char *buffer, size_t size) {
		if (!size) {
			size = strlen(buffer);
		}
		std::ofstream out(destFile, std::ios_base::binary | std::ios_base::app);
		out.write(buffer, size);
		out.close();
	}

	void WriteToFileUTF8(const wchar_t *destFile, const wchar_t *buffer) {
		char *out = NULL;
		int outLen;
		if (sysStr::WideCharToUTF8(buffer, wcslen(buffer), out, outLen)) {
        	WriteToFile(destFile, out, outLen);
		}
		if (out) {
	        delete []out;
		}
	}

    std::vector<std::wstring> GetDrives() {
    	wchar_t buffer[MAX_PATH];
        memset(buffer, 0, MAX_PATH * sizeof(wchar_t));
		GetLogicalDriveStringsW(MAX_PATH, buffer);

        std::vector<std::wstring> drives;

        for (long it = 0; it < MAX_PATH; it += 4) {
			if (buffer[it] == 0 && buffer[it+1] == 0) {
				break;
            }
            drives.push_back(std::wstring(&buffer[it]));
		}

        return drives;
    }

    ULONGLONG GetFileAge(const WIN32_FIND_DATAW &ffd) {
    	SYSTEMTIME stCurrent;
		GetSystemTime(&stCurrent);
    	SYSTEMTIME stFileCreated;
		FileTimeToSystemTime(&(ffd.ftCreationTime), &stFileCreated);
        return sysTime::DiffSystemTimeMSec(stCurrent, stFileCreated);
    }

	void ShowFolderInExplorer(const wchar_t *wFolder) {
		ShellExecute(NULL, L"open", wFolder, NULL, NULL, SW_SHOWDEFAULT);
	}

	void ShowFileInExplorer(const wchar_t *wFile) {
		wchar_t wBuffer[MAX_PATH];
		memset(wBuffer, 0, sizeof(wchar_t) * MAX_PATH);
		swprintf(wBuffer, L"Explorer.exe /select,%s", wFile);

		sysProcess::ExecuteCmd(NULL, wBuffer, 5000);
		//ShellExecute(NULL, L"open", wFile, NULL, NULL, SW_SHOWDEFAULT);
		//char cBuffer[MAX_PATH];
		/*
		//sysStr::WToMB(wFile, &cBuffer[strlen(cBuffer)], MAX_PATH);
		sysProcess::ExecuteCmd(L"explorer", wBuffer, 5000);
		*/
        //system(cBuffer);
	}

	std::string ReadFile(const wchar_t *sFile) {
		size_t fileSize = GetFileSize(sFile);
		std::ifstream in(sFile, std::ios_base::binary | std::ios_base::in);
		if (!in.is_open() || !in.good()) {
			return std::string("");
		}

		std::string content;
		content.resize(fileSize);
		in.read(&content[0], fileSize);
		in.close();

		return content;
	}

	/*
	std::wstring ReadFile(const wchar_t *sFile, int codePage) {
		std::string text = ReadFile(sFile);
		if (text.empty() == true) {
			return std::wstring(L"");
		}

		wchar_t *wBuffer;
		size_t size;
		if (sysStr::MBToW(text.c_str(), wBuffer, size) == false) {
        	return std::wstring(L"");
		}

		std::wstring wText(wBuffer);
		delete []wBuffer;

		return wText;
	}
	*/

	bool CreateFolder(const wchar_t *wFolder) {
		std::vector<std::wstring> folders;
		sysStr::Split(wFolder, L"\\", folders);
		if (!folders.size()) {
			return false;
		}
		size_t bufferSize = wcslen(wFolder);
		wchar_t *wFolderBuffer = new wchar_t[bufferSize + 1];
		memset(wFolderBuffer, 0, bufferSize * sizeof(wchar_t));

		wcscpy(&wFolderBuffer[wcslen(wFolderBuffer)], folders[0].c_str());
		wcscpy(&wFolderBuffer[wcslen(wFolderBuffer)], L"\\");

		for (size_t itFolder = 1 /*skip drive*/; itFolder < folders.size(); ++itFolder) {
			wcscpy(&wFolderBuffer[wcslen(wFolderBuffer)], folders[itFolder].c_str());
			wcscpy(&wFolderBuffer[wcslen(wFolderBuffer)], L"\\");

			if (!IsFolderExist(wFolderBuffer)) {
				if (!CreateDirectory(wFolderBuffer, NULL)) {
					delete []wFolderBuffer;
					return false;
				}
			}
		}

		delete []wFolderBuffer;
		return folders.size() > 1;
	}

	struct DeleteFileIterator : public sysFile::IScanIterator {
		private:
            ULONGLONG age;
		public:

		DeleteFileIterator(ULONGLONG age) {
            this->age = age;
		}

		//Return true if ScanFiles function can go to the next file, otherwise false
		virtual bool ScanIterateCallback(const wchar_t *folder, const WIN32_FIND_DATAW &ffd, void *data = NULL) {
			if (GetFileAge(ffd) > age) {
				wchar_t absolutePath[MAX_PATH];
				if (snwprintf(absolutePath, MAX_PATH, L"%s%s", folder, ffd.cFileName) != -1) {
                    DeleteFile(absolutePath);
				}
			}
			return true;
		}
	};

	void DeleteFilesOlderThen(const wchar_t *folder, const wchar_t *filter, bool recursive, ULONGLONG age) {
		DeleteFileIterator iterator(age);
		ScanFiles(folder, filter, iterator, recursive);
	}
}



