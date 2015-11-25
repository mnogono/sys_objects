// ---------------------------------------------------------------------------

#ifndef sysStrH
#define sysStrH
// ---------------------------------------------------------------------------

namespace sysStr {

	/* do not forget to delete result buffer */
	/*
	template<typename T>
	char *Join(std::vector<T> &vec, const char *sep = ","){
		if (!vec.size()){
			return "\0";
		}

		std::stringstream ss;
		ss << vec.front();
		for (std::vector<T>::iterator it = vec.begin() + 1, itEnd = vec.end(); it != itEnd; ++it){
			ss << sep << *it;
		}

		char *sBuffer = new char[ss.str().size()];
		strcpy(sBuffer, ss.str().c_str());

		return sBuffer;
	}
	*/
	/*
    template<typename T>
    wchar_t *Join(std::vector<T> &vec, const wchar_t *sep=L","){
		if (!vec.size()){
			return NULL;
		}

		std::wstringstream wss;
		wss << vec.front();
		for (std::vector<T>::iterator it = vec.begin() + 1, itEnd = vec.end(); it != itEnd; ++it){
			wss << sep << *it;
		}

		wchar_t *wBuffer = new wchar_t[ss.str().size()];
		strcpy(wBuffer, ss.str().c_str());

		return wBuffer;
	}
	*/

	/*
	 allocate memmory for output data
	 do not forget to delete return buffer
	 sep like L","
	 */
	wchar_t *JoinString(std::vector<std::wstring>::iterator first, std::vector<std::wstring>::iterator end, const wchar_t *sep);

	/*
	 function allocate wchar_t buffer
	 do not forget to delete this buffer after using
	 sep like L","
	 */
	wchar_t *JoinStrings(std::vector<std::wstring> &vec, const wchar_t *sep);

	/*
	join strings into outBuffer
	outBuffer must be size allowed to input all joined strings
	sep like L","
	*/
	void JoinStrings(std::vector<std::wstring>::iterator first, std::vector<std::wstring>::iterator end, wchar_t *outBuffer, const wchar_t *sep);

	#ifdef __BORLANDC__
	template<typename T>
	void Join(std::vector<T> &order, std::map<T, std::wstring> &mapValues, const wchar_t *sep, std::wstringstream &wss){
		wss << mapValues[order.front()];
		std::wstring msg;
		for (std::vector<T>::iterator it = order.begin() + 1, itEnd = order.end(); it != itEnd; ++it){
			msg = *it;
			wss << sep << mapValues[*it];
		}
	}
	#endif

	/*
	 split string into tokens using delimeter
	 */
	void Split(const wchar_t *wcstr, const wchar_t *delimeter, std::vector<std::wstring> &out);

	/*
	 split string into tokens using max width chars of each tokens
	 */
	void Split(const wchar_t *wcstr, int maxWidth, std::vector<std::wstring> &out);

	/*
	 split string into tokens using max width of pixels
	 */
	void Split(const wchar_t *wcstr, HDC hdc, int maxWidthPixels, std::vector<std::wstring> &out);

	/*
	 return size of text in pixels
	 */
	SIZE GetTextSizePixels(const wchar_t *wcstr, int len, HDC hdc);

	/*
	 convert wide char string to multibyte string
	 do not forget to delete outStr like 'delete []outStr;'
	 len - size in characters, can be -1 if string (wcstr) is null-terminated
	 outLen output length of outStr include null terminated symbol!
	 */
	bool WideCharToUTF8(const wchar_t *wcstr, size_t len, char *&outStr, int &outLen);

	/*
	 do not forget to delete outStr like 'delete []outStr;'
	 */
	bool UTF8ToWideChar(const char *str, size_t len, wchar_t *&outStr, int &outLen);

	/*
	do not forget to delete outStr like 'delete []outStr;'
	*/
	bool UTF16ToWideChar(const char *str, int len, wchar_t *&outStr, int &outLen, bool isLittleIndian);

	/*
	 Check is char is unreserved

	 Portable character check (remember EBCDIC). Do not use isalnum() because
	 its behavior is altered by the current locale.
	 See http://tools.ietf.org/html/rfc3986#section-2.3
	 */
	bool URLEncodeCharIsUnreserved(unsigned char in);

	bool URLEncodeCharIsUnreserved(wchar_t in);

	/*
	 convert wide char string to url encoded using utf-8 encoding
	 */
	void URLEncodeUTF8(const wchar_t *wcstr, int len, char *&outStr, int &outLen);

	/*
	 convert null terminated wide string to multibyte string
     don't forget to specify locale before using this function example: setlocale(LC_ALL, ".1251");
	 don't forget to delete sBuffer after using
	 */
	bool WToMB(const wchar_t *wBuffer, char *&sBuffer);

    /*
    convert null terminated wide string to multibyte string
    sBuffer must be allocated before using this function
    don't forget to specify locale before using this function exaple: setlocale(LC_ALL, ".1251");
    */
    bool WToMB(const wchar_t *wBuffer, char *sBuffer, size_t sBufferSize);

	/*
	 convert null terminated multibute string to wide char string
     don't forget to specify locale before using this function example: setlocale(LC_ALL, ".1251");
	 don't forget to delete wBuffer after using
	 */
	bool MBToW(const char *sBuffer, wchar_t *&wBuffer, size_t &size);

    /*
    convert null terminated multibute string to wide char string
    don't forget to specify locale before using this function example: setlocale(LC_ALL, ".1251");
    function does not allocate memmory for wBuffer, so before execute function allocate necessary memmory for wBuffer
    */
    bool MBToW(const char *sBuffer, wchar_t *wBuffer);

	#ifdef __BORLANDC__

		/*
		create unit identifier string
		function allocate new memmory for output buffer
		do not forget to delect output buffer
		*/
		char *CreateGUIDEx();

		wchar_t *CreateGUIDExW();

	#endif

    //find last index token in string
    //return -1 if can't find
    int LastIndexOf(const wchar_t *wstr, const wchar_t token);

    /*
    * Screen slash of input string. Function screen all shash except of slash before quote
    * function allocate memmory for output string
    * do not forget to delete wstrOut after using
    * @param {const wchar_t *} wstrSrc
    * @param {wchar_t *&} wstrOut output string
    * @result {bool} is wstrOut was build or not
	*/
	bool ScreenSlash(const wchar_t *wstrSrc, wchar_t *&wstrOut);

	void EncodeBase64(const BYTE bytes[], size_t size, char **outBase64, size_t &outBase64Size);

	void DecodeBase64(const char *base64, size_t base64Size, BYTE **bytes, size_t &outBytesSize);

	void DecodeBase64(const wchar_t *base64, double **values, size_t &outValuesCount);

	/*
	replace key to value in templateString
	*/
	void Replace(std::wstring &str, const wchar_t *key, const wchar_t *value);

	void Replace(std::string &str, const char *key, const char *value);

	/*
	replace all key in str into value
	*/
	void ReplaceAll(std::string &str, const char *key, const char *value);
}

#endif
