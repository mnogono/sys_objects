//---------------------------------------------------------------------------
#ifndef sysFileH
#define sysFileH
//---------------------------------------------------------------------------

namespace sysFile {

	class TExceptionCantGetFileInfo : public sysException::TException {
		public:
			TExceptionCantGetFileInfo(wchar_t *fileName) : sysException::TException(L"") {
				wchar_t sText[1024];
				wsprintfW(sText, L"%s iLastError: %d", fileName, GetLastError());
				msg = std::wstring(sText);
			}
	};

	class TExceptionCantGetFileInfoQueryValue : public sysException::TException {
		public:
		  TExceptionCantGetFileInfoQueryValue(wchar_t *fileName) : sysException::TException(fileName) {}
	};

	struct TExeFileInfo {
		private:
			std::wstring fileVersion;

		public:
			TExeFileInfo() {}

			TExeFileInfo(TExeFileInfo &) {}

			std::wstring GetFileVersion() { return fileVersion; }

			void Set(wchar_t *infoName, wchar_t *infoValue) {
				if (wcsncmp(infoName, L"FileVersion", wcslen(L"FileVersion")) == 0) {
					fileVersion = std::wstring(infoValue);
				}
			};
	};

	VS_FIXEDFILEINFO * GetExeVersionRoot(wchar_t *fileName);

	/*
	"Comments"
	"CompanyName"
	"FileDescription"
	"FileVersion"
	"InternalName"
	"LegalCopyright"
	"LegalTrademarks"
	"OriginalFilename"
	"ProductName"
	"ProductVersion"
	"PrivateBuild"
	"SpecialBuild"
	*/
	//TExeFileInfo * GetExeVersion(wchar_t * fileName, unsigned int codePage = 0);


#ifdef __BORLANDC__
	/**
	@return file version string or empty string
	*/
	std::wstring getExeVersion(const wchar_t *file, WORD codePage = 1251);
#endif

	bool IsFileExist(const char *file);

	bool IsFileExist(const wchar_t *file);

	bool IsFolderExist(const wchar_t *folder);

	bool IsFolderExist(const char *folder);

	long GetFileSize(const wchar_t *file);

	size_t GetFileSize(const char *file);

#ifdef __BORLANDC__
	__int64 GetFileSize64(const wchar_t *file);

	__int64 GetFileSize64(const char *file);
#endif


	/*
	Convert each byte of file to hex text like "0F"
	*/
	void BinaryToStream(const wchar_t *file, std::ostream *out);

    void BinaryToStream(const unsigned char *buffer, int size, std::ostream *out);

    /*
	Convert each byte of file to hex text like "0F" and return string
	function allocate memmory for output buffer, do not forget to delete buffer after using
	*/
	char * BinaryToString(const wchar_t *file);

	/*
	Convert each byte of file to hex text like "0F" and return string
	out - buffer need to be already allocated
	the size of out must be [2 * file_size_bytes + 1]
	*/
	char * BinaryToString(const wchar_t *file, char *out);

#ifdef __BORLANDC__
	/*
	convert each byte of buffer to hex text like "XX" and return string
	*/
	char * ByteBufferToString(const TByteDynArray &buffer);

    /*
    memmory for buffer must be already allocated before invoke function FileToByteBuffer
    use function sysFile::GetFileSize for allocate memmory for buffer
    */
    void FileToByteBuffer(const wchar_t *file, unsigned char *buffer);
#endif

	/*
		return specify executable module path
		hModule == NULL for current executable module
	*/
	std::wstring GetExecutablePath(HMODULE hModule = NULL);

	/*
		return specify executable module path
        example C:\Debug\application.exe
		hModule == NULL for current executable module
	*/
	bool GetExecutablePath(wchar_t *outPath, int maxPathLen, HMODULE hModule = NULL);

    /*
    	return executable folder absolute path
		example C:\Debug
        do not forget to delete returned buffer
    */
    wchar_t * GetExecutableFolder(HMODULE hModule = NULL);

	/*
		function allocate memmory
		do not forget to release allocated memmory
	*/
	wchar_t * GetFilePath(wchar_t *path);

	void GetFilePath(wchar_t *path, wchar_t *out);

	/*
	function not allocate any memory for new string. Function modify path variable
	find last '\\' symbol and set 0 into this position, return the same ptr
	*/
	wchar_t *TrunkFileName(wchar_t *path, size_t len = 0);

    const wchar_t * GetFileExt(const wchar_t *file);

    /*
    function not allocate any memory for new string.
	Function return new ptr for current string path begin after last '\\' symbol
	path - null terminated string
	*/
	//const wchar_t * const GetFileName(wchar_t *path, size_t len = 0);

	std::wstring GetFileName(const wchar_t *path);

	std::wstring GetFolder(const wchar_t *path);

#ifdef __BORLANDC__

    //return file name without extension
    String GetFileNameWithoutExt(String path);
#endif

	//function pointer for callback function FilesIteration function
    typedef void (*FileIteratorCallback)(const wchar_t *folder, const WIN32_FIND_DATAW &ffd, void *data);

    struct IScanIterator {
    	//Return true if ScanFiles function can go to the next file, otherwise false
    	virtual bool ScanIterateCallback(const wchar_t *folder, const WIN32_FIND_DATAW &ffd, void *data = NULL) = 0;
    };

    /*
	    iterate over all files in directory and invoke callback function, recursive
        excludeFolders - vector of absolude path of exclude folders, for do not iterate inside it
    */
    //void ScanFiles(const wchar_t *dir, const wchar_t *filter, FileIteratorCallback callback, bool bRecursive, const std::vector<std::wstring> *excludeFolders = NULL, void *data = NULL);
    void ScanFiles(const wchar_t *dir, const wchar_t *filter, IScanIterator &scanIterator, bool bRecursive, bool &abort, const std::vector<std::wstring> *excludeFolders = NULL);

	//return true if folder is empty
    bool IsFolderEmpty(const wchar_t *folder);

	/*
	get all files from dir, not recursive search
	filter like L"*.*"
	*/
	void GetFiles(const wchar_t *dir, std::vector<std::wstring> &files, const wchar_t *filter);

	//get all folder from dir, not recursive searche
	void GetFolders(const wchar_t *folder, std::vector<std::wstring> &subFolders);

    //remove file
    bool DeleteFile(const char *file);

    bool DeleteFile(const wchar_t *file);

    //move folder from one place to another
    bool MoveFolder(const wchar_t *srcFolder, const wchar_t *destFolder);

    //copy srcFolder into destFolder. srcFolder name created into destFolder
    bool CopyFolder(const wchar_t *srcFolder, const wchar_t *destFolder);

    //recurcive function for copy all from srcFolder into destFolder
    bool __CopyFolder(const wchar_t *srcFolder, const wchar_t *destFolder);

	/*
	copy files from srcFolder into destFolder using filter
	by default copy all files
	filter like L"*.*"
	*/
	bool CopyFiles(const wchar_t *srcFolder, const wchar_t *destFolder, const wchar_t *filter);

    //copy file
    bool CopyFile(const wchar_t *srcFile, const wchar_t *destFile);

    bool DeleteFolder(const wchar_t *folder);

	#ifdef __BORLANDC__

    /*
    write data into file
    */
	void WriteToFile(wchar_t *file, TByteDynArray &data);
	#endif

	//return vector of available driversin system
	std::vector<std::wstring> GetDrives();

	/*
	write to file buffer
	if size is 0, size = strlen(buffer)
	*/
	void WriteToFile(const wchar_t *destFile, const char *buffer, size_t size = 0);

	void WriteToFileUTF8(const wchar_t *destFile, const wchar_t *buffer);

	void AppendToFile(const wchar_t *destFile, const char *buffer, size_t size = 0);

    /*
    return age of file in msec
    */
    ULONGLONG GetFileAge(const WIN32_FIND_DATAW &ffd);

	void ShowFileInExplorer(const wchar_t *wFile);

	void ShowFolderInExplorer(const wchar_t *wFolder);

	std::string ReadFile(const wchar_t *sFile);

	//std::wstring ReadFileW(const wchar_t *sFile);

	//create all folders in wFolder
	// @param wFolder absolute folder path
	bool CreateFolder(const wchar_t *wFolder);
}

#endif
