#include "pch.h"

#ifdef  __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysStr {
	/*
	template<class T>
	std::wstring Join(T container, const wchar_t *sep) {
		return std::wstring(L"");
	}
    */
	wchar_t *JoinString(std::vector<std::wstring>::iterator first, std::vector<std::wstring>::iterator end, const wchar_t *sep){
		std::wstringstream ss;
		// ss << vec.front();
		ss << *first;
		for (std::vector<std::wstring>::iterator it = first + 1; it != end; ++it){
			ss << sep << *it;
		}

		wchar_t *sBuffer = new wchar_t[ss.str().size() + 1];
		wcscpy(sBuffer, ss.str().c_str());

		return sBuffer;
	}

	wchar_t *JoinStrings(std::vector<std::wstring> &vec, const wchar_t *sep){
		if (!vec.size()){
			return L"\0";
		}

		return JoinString(vec.begin(), vec.end(), sep);
	}

	void JoinStrings(std::vector<std::wstring>::iterator first, std::vector<std::wstring>::iterator end, wchar_t *outBuffer, const wchar_t *sep) {
		wsprintfW(outBuffer, L"%s", first->c_str());
		size_t index = first->size();
		for (std::vector<std::wstring>::iterator it = first + 1; it != end; ++it, index = wcslen(outBuffer)){
			wsprintfW(&outBuffer[index], L"%s%s", sep, it->c_str());
		}
	}

	void Split(const wchar_t *wcstr, size_t maxWidth, std::vector<std::wstring> &out){
		unsigned int len = wcslen(wcstr);
		if (!len){
			return;
		}

		size_t tokenCount = ceil(len / (double)maxWidth);
		size_t remainLen = len;
		size_t pos = 0;

		for (size_t itToken = 0; itToken < tokenCount; ++itToken){
			if (remainLen < maxWidth){
				maxWidth = remainLen;
			}
			wchar_t *token = new wchar_t[maxWidth + 1];
			token[maxWidth] = 0;
			wcsncpy(token, wcstr + pos, maxWidth);
			out.push_back(token);
			delete[]token;

			pos += maxWidth;
		}
	}

	void Split(const wchar_t *wcstr, const wchar_t *delimeter, std::vector<std::wstring> &out){
		unsigned int len = wcslen(wcstr);
		if (!len){
			return;
		}

		wchar_t *cpyWcstr = new wchar_t[len + 1];
		wcscpy(cpyWcstr, wcstr);
		cpyWcstr[len] = 0;

		wchar_t *pwc = wcstok(cpyWcstr, delimeter);
		out.push_back(pwc);

		while (pwc != NULL){
			pwc = wcstok(NULL, delimeter);
			if (!pwc){
				break;
			}
			out.push_back(pwc);
		}

		delete[]cpyWcstr;

		/*

		 char str[] ="- This, a sample string.";
		 char * pch;
		 printf ("Splitting string \"%s\" into tokens:\n",str);
		 pch = strtok (str," ,.-");
		 while (pch != NULL)
		 {
		 printf ("%s\n",pch);
		 pch = strtok (NULL, " ,.-");
		 }
		 return 0;
		 */
	}

	SIZE GetTextSizePixels(const wchar_t *wcstr, int len, HDC hdc){
		SIZE textSizePixels;
		// HDC __hdc = GetDC(Application->Handle);
		GetTextExtentPoint32W(hdc, wcstr, len, &textSizePixels);

		return textSizePixels;
	}

	void Split(const wchar_t *wcstr, HDC hdc, int maxWidthPixels, std::vector<std::wstring> &out){
		// SIZE textSize = GetTextWidthPixels(wcstr,  ,hdc);
		int len = wcslen(wcstr);
		int posToken = 0;
		int lenToken = 0;

		while (true){
			posToken += lenToken;
			lenToken = 1;
			while (true){
				SIZE s = GetTextSizePixels(wcstr + posToken, lenToken, hdc);
				if (s.cx > maxWidthPixels){
					break;
				}
				++lenToken;
				if (posToken + lenToken >= len){
					break;
				}
			}

			wchar_t *token = new wchar_t[lenToken + 1];
			wcsncpy(token, wcstr + posToken, lenToken);
			token[lenToken] = 0;
			out.push_back(token);
			delete[]token;

			if (posToken + lenToken >= len){
				break;
			}
		}

		/*
		 std::vector<String>splitted;
		 TStringGrid *Grid = StringGridSensors;

		 int pos = 0;
		 int count = text.Length();

		 while (Grid->Canvas->TextWidth(text.SubString(pos, count)) > width){
		 while (Grid->Canvas->TextWidth(text.SubString(pos, count)) > width){
		 --count;
		 }
		 splitted.push_back(text.SubString(pos, count));
		 pos += count + 1;
		 count = text.Length() - pos;
		 }
		 splitted.push_back(text.SubString(pos, count + 1));

		 return splitted;
		 */
	}

	bool WideCharToUTF8(const wchar_t *wcstr, size_t len, char *&outStr, int &outLen){
		outLen = WideCharToMultiByte(CP_UTF8, 0, wcstr, len, NULL, 0, NULL, NULL);
		if (outLen <= 0){
			return false;
		}

		outStr = new char[outLen + 1];
		WideCharToMultiByte(CP_UTF8, 0, wcstr, len, outStr, outLen, NULL, NULL);
		outStr[outLen] = 0;

		return true;
	}

	bool UTF8ToWideChar(const char *str, size_t len, wchar_t *&outStr, int &outLen){
		outLen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
		if (outLen <= 0){
			return false;
		}

		outStr = new wchar_t[outLen + 1];
		MultiByteToWideChar(CP_UTF8, 0, str, len, outStr, outLen);
		outStr[outLen] = 0;

		return true;
	}

	bool UTF16ToWideChar(const char *str, int len, wchar_t *&outStr, int &outLen, bool isLittleIndian) {
		int codePage = (isLittleIndian)?1251:1251;
		outLen = MultiByteToWideChar(codePage, 0, str, len, NULL, 0);
		if (outLen <= 0){
			return false;
		}

		outStr = new wchar_t[outLen + 1];
		MultiByteToWideChar(codePage, 0, str, len, outStr, outLen);
		outStr[outLen] = 0;

    	return true;
	}

	bool URLCharIsUnreserved(unsigned char in){
		switch (in){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '-':
		case '.':
		case '_':
		case '~':
			return true;
		default:
			break;
		}
		return false;
	}

	bool URLEncodeCharIsUnreserved(wchar_t in){
		switch (in){
		case L'0':
		case L'1':
		case L'2':
		case L'3':
		case L'4':
		case L'5':
		case L'6':
		case L'7':
		case L'8':
		case L'9':
		case L'a':
		case L'b':
		case L'c':
		case L'd':
		case L'e':
		case L'f':
		case L'g':
		case L'h':
		case L'i':
		case L'j':
		case L'k':
		case L'l':
		case L'm':
		case L'n':
		case L'o':
		case L'p':
		case L'q':
		case L'r':
		case L's':
		case L't':
		case L'u':
		case L'v':
		case L'w':
		case L'x':
		case L'y':
		case L'z':
		case L'A':
		case L'B':
		case L'C':
		case L'D':
		case L'E':
		case L'F':
		case L'G':
		case L'H':
		case L'I':
		case L'J':
		case L'K':
		case L'L':
		case L'M':
		case L'N':
		case L'O':
		case L'P':
		case L'Q':
		case L'R':
		case L'S':
		case L'T':
		case L'U':
		case L'V':
		case L'W':
		case L'X':
		case L'Y':
		case L'Z':
		case L'-':
		case L'.':
		case L'_':
		case L'~':
			return true;
		default:
			break;
		}
		return false;
	}

	void URLEncodeUTF8(const wchar_t *wcstr, int len, char *&outStr, int &outLen){
		char *wc2mb;
		int wc2mbLen;

		/* calculate number of needed bytes */
		outLen = 0;
		for (int itWChar = 0; itWChar < len; ++itWChar){
			if (URLEncodeCharIsUnreserved(wcstr[itWChar])){
				++outLen;
			} else if (wcstr[itWChar] == L' '){
				++outLen;
			} else {
				WideCharToUTF8(&wcstr[itWChar], 1, wc2mb, wc2mbLen);
				outLen += wc2mbLen * 3;
				delete[]wc2mb;
			}
		}

		outStr = new char[outLen + 1];
		memset(outStr, 0, outLen + 1);

		int outIndex = 0;

		for (int itWChar = 0; itWChar < len; ++itWChar){
			if (URLEncodeCharIsUnreserved(wcstr[itWChar])){
				outStr[outIndex] = wcstr[itWChar];
				++outIndex;

			} else if (wcstr[itWChar] == L' '){
				outStr[outIndex] = '+';
				++outIndex;

			} else {
				WideCharToUTF8(&wcstr[itWChar], 1, wc2mb, wc2mbLen);
				for (int itMB = 0; itMB < wc2mbLen; ++itMB){
					//snprintf(&outStr[outIndex], 4, "%%%02X", (unsigned char)wc2mb[itMB]); c++11
					sprintf(&outStr[outIndex], "%%%02X", (unsigned char)wc2mb[itMB]);
					outIndex += 3;
				}
				delete[]wc2mb;
			}
		}

		outLen = strlen(outStr);
	}

    bool WToMB(const wchar_t *wBuffer, char *sBuffer, size_t sBufferSize) {
        size_t bytesCount = wcstombs(sBuffer, wBuffer, sBufferSize);
		if (bytesCount == size_t(-1) || bytesCount > sBufferSize){
			return false;
        }
        sBuffer[bytesCount] = 0;
        memset(sBuffer + bytesCount, 0, sBufferSize - bytesCount);

		return true;
    }

	bool WToMB(const wchar_t *wBuffer, char *&sBuffer) {
		size_t size = wcstombs(0, wBuffer, 0);
		if (size == size_t(-1)){
			return false;
		}

		sBuffer = new char[size + 1];
		memset(sBuffer, 0, size + 1);
		return WToMB(wBuffer, sBuffer, (size_t)size);
	}

	bool MBToW(const char *sBuffer, wchar_t *&wBuffer, size_t &size){
		size = mbstowcs(NULL, sBuffer, 0);
        if (size < 0) {
			return false;
        }

        wBuffer = new wchar_t[size + 1];
        mbstowcs(wBuffer, sBuffer, size);
        wBuffer[size] = 0;

        return true;
    }

    bool MBToW(const char *sBuffer, wchar_t *wBuffer){
		int size = mbstowcs(NULL, sBuffer, 0);
        if (size < 0) {
			return false;
        }

		mbstowcs(wBuffer, sBuffer, size);
        wBuffer[size] = 0;

        return true;
    }

	#ifdef __BORLANDC__

	char *CreateGUIDEx() {
		GUID guid;
		CreateGUID(guid);
		String sGuid = GUIDToString(guid);
		sGuid = StringReplace(sGuid, "{", "", TReplaceFlags() << rfReplaceAll);
		sGuid = StringReplace(sGuid, "}", "", TReplaceFlags() << rfReplaceAll);
		sGuid = StringReplace(sGuid, "-", "", TReplaceFlags() << rfReplaceAll);

		char *sBuffer;

		WToMB(sGuid.c_str(), sBuffer);

		return sBuffer;
	}

	wchar_t *CreateGUIDExW() {
		GUID guid;
		CreateGUID(guid);
		String sGuid = GUIDToString(guid);
		sGuid = StringReplace(sGuid, "{", "", TReplaceFlags() << rfReplaceAll);
		sGuid = StringReplace(sGuid, "}", "", TReplaceFlags() << rfReplaceAll);
		sGuid = StringReplace(sGuid, "-", "", TReplaceFlags() << rfReplaceAll);

		size_t len = wcslen(sGuid.c_str()) + 1;
		wchar_t *wBuffer = new wchar_t[len];
		wcscpy(wBuffer, sGuid.c_str());
		wBuffer[len-1] = 0;
		return wBuffer;
	}

	#endif

    int LastIndexOf(const wchar_t *wstr, const wchar_t token) {
    	size_t len = wcslen(wstr);
        for (int i = len - 1; i >= 0; --i) {
			if (wstr[i] == token) {
				return i;
            }
        }

        return -1;
    }

    bool ScreenSlash(const wchar_t *wstrSrc, wchar_t *&wstrOut) {
	    wstrOut = NULL;

		//1. find new size of wstrOut
        //2. allocate memmory
        //3. build wstrOut

        int len = wcslen(wstrSrc);
        unsigned int iFoundedSlashCount = 0;
        for (int i = 0; i < len; ++i) {
        	if (wstrSrc[i] == L'\\') {
                if (/*wstrSrc[i+1] == L'\\' ||*/
                	wstrSrc[i+1] == L'"') {
                    //skip already screen slash
                    ++i;
                    continue;
                } else {
                    ++iFoundedSlashCount;
                }
            }
        }

        if (iFoundedSlashCount != 0) {
        	wstrOut = new wchar_t[len + iFoundedSlashCount + 1];
            unsigned int itSlash = 0;
            for (int i = 0; i < len; ++i) {
                wstrOut[i+itSlash] = wstrSrc[i];
                if (wstrSrc[i] == L'\\'){
                    if (/*wstrSrc[i+1] == L'\\' ||*/
	                    wstrSrc[i+1] == L'"') {

                        wstrOut[i+itSlash] = wstrSrc[i];
                        wstrOut[i+itSlash+1] = wstrSrc[i+1];
                        //skip already screen slash
						++i;
                        continue;
                    } else {
                        wstrOut[i+itSlash] = wstrSrc[i];
                        ++itSlash;
                        wstrOut[i+itSlash] = L'\\';
                    }
				}
            }
            wstrOut[len + iFoundedSlashCount] = 0;
			return true;
        } else {
            return false;
        }
    }
    
	char _encTools_BASE64_Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	static void Append(char val, char *base64, size_t &position) {
		if (val != 100) {
			base64[position] = _encTools_BASE64_Alphabet[val];
		} else {
    		base64[position] = '=';
		}
		++position;
	}

	static inline DWORD Extract(const char *base64, size_t &position) {
		char a = base64[position];
		position++;

		if ('A' <= a && a <= 'Z') {
			return a - 'A';
		} else if ('a' <= a && a <= 'z') {
			return 26 + a - int('a');
		} else if ('0' <= a && a <= '9') {
			return 52 + a - '0';
		} else if (a == '+') {
			return 62;
		} else if (a == '/') {
			return 63;
		} else if (a == '=') {
        	return 100;
		}

		return 101;
	}

	void DecodeBase64(const char *base64, size_t base64Size, BYTE **bytes, size_t &outBytesSize) {
		outBytesSize = base64Size * 3.0 / 4.0;
		if (base64[base64Size - 2] == '=') {
			outBytesSize -= 2;
		} else if (base64[base64Size - 1] == '=') {
        	outBytesSize -= 1;
		}

		*bytes = new BYTE[outBytesSize];
        memset(*bytes, 0, outBytesSize);

		DWORD byte3;
		size_t position = 0;
		BYTE bit6, bit12, bit18, bit24;
		size_t byteOffset = 0;

		while(base64[position + 3] != '=' && byteOffset < outBytesSize) {
			bit6  = Extract(base64, position);
			bit12 = Extract(base64, position);
			bit18 = Extract(base64, position);
			bit24 = Extract(base64, position);

			byte3 = 0;
			byte3 = ((DWORD)bit6)  << 18 |
					((DWORD)bit12) << 12 |
					((DWORD)bit18) << 6  |
					((DWORD)bit24) << 0;

			(*bytes)[byteOffset + 0] = (char)((byte3 >> 16) & 0xFF);
			(*bytes)[byteOffset + 1] = (char)((byte3 >> 8)  & 0xFF);
			(*bytes)[byteOffset + 2] = (char)((byte3 >> 0)  & 0xFF);

			byteOffset += 3;
		}
		if (byteOffset >= outBytesSize) {
			return;
		}

		if (base64[position + 2] == '=') {
			bit6  = Extract(base64, position);
			bit12 = Extract(base64, position);

			byte3 = 0;
			byte3 = ((DWORD)bit6) << 18 |
					((DWORD)bit12) << 12;

			(*bytes)[byteOffset + 0] = (char)((byte3 >> 16) & 0xFF);

		} else if (base64[position + 3] == '=') {
			bit6  = Extract(base64, position);
			bit12 = Extract(base64, position);
			bit18 = Extract(base64, position);

			byte3 = 0;
			byte3 = ((DWORD)bit6)  << 18 |
					((DWORD)bit12) << 12 |
					((DWORD)bit18) << 6;

			(*bytes)[byteOffset + 0] = (char)((byte3 >> 16) & 0xFF);
			(*bytes)[byteOffset + 1] = (char)((byte3 >> 8) & 0xFF);
		}
	}

	void EncodeBase64(const BYTE bytes[], size_t size, char **outBase64, size_t &outBase64Size) {
		outBase64Size = ceil(size * 4.0 / 3.0);
		if (size % 3 == 1) {
			outBase64Size += 2;
		} else if (size % 3 == 2) {
			outBase64Size += 1;
		}
		*outBase64 = new char[outBase64Size];
		memset(*outBase64, 0, outBase64Size);

		size_t byteOffset = 0;
		size_t position = 0;

		DWORD byte3;
		while ((size - byteOffset) >= 3) {
			byte3 = 0;
			byte3 = ((DWORD)(bytes[byteOffset + 0]) << 16) |
					((DWORD)(bytes[byteOffset + 1]) << 8)  |
					((DWORD)(bytes[byteOffset + 2]) << 0);

			Append((byte3 >> 18) & 63, *outBase64, position);
			Append((byte3 >> 12) & 63, *outBase64, position);
			Append((byte3 >> 6)  & 63, *outBase64, position);
			Append((byte3 >> 0)  & 63, *outBase64, position);

			byteOffset += 3;
		}

		size_t remainBytes = size - byteOffset;
		if (remainBytes) {
			byte3 = 0;
			memcpy(&byte3, &bytes[byteOffset], remainBytes);

			if (remainBytes == 2) {
				byte3 = 0;
				byte3 = (DWORD)bytes[byteOffset + 0] << 8 |
						(DWORD)bytes[byteOffset + 1] << 0;

				Append((byte3 >> 10) & 63, *outBase64, position);
				Append((byte3 >> 4)  & 63, *outBase64, position);
				Append((byte3 << 2)  & 63, *outBase64, position);
				Append(100, *outBase64, position);

			} else if (remainBytes == 1) {
				byte3 = 0;
				byte3 = (DWORD)bytes[byteOffset + 0];

				Append((byte3 >> 2) & 63, *outBase64, position);
				Append((byte3 & 3) << 4 , *outBase64, position);
				Append(100              , *outBase64, position);
				Append(100              , *outBase64, position);
			}
		}
	}

	void DecodeBase64(const wchar_t *base64, double **values, size_t &outValuesCount) {
		outValuesCount = 0;

		char *__base64 = NULL;
		if (!WToMB(base64, __base64)) {
			if (__base64 != NULL) {
				delete []__base64;
			}
			return;
		}

		BYTE *bytes = NULL;
		size_t bytesSize;
		sysStr::DecodeBase64(__base64, strlen(__base64), &bytes, bytesSize);

		if (__base64 != NULL) {
			delete []__base64;
		}

		if (bytesSize % 8 != 0) {
			if (bytes != NULL) {
				delete []bytes;
				return;
			}
		}

		outValuesCount = bytesSize / 8;

		*values = new double[outValuesCount];
		memcpy(&(*values)[0], &bytes[0], bytesSize);

		delete []bytes;
	}

	void Replace(std::wstring &str, const wchar_t *key, const wchar_t *value) {
		size_t index = str.find(key, 0);
		if (index == std::wstring::npos) {
			return;
		}

		str.replace(index, wcslen(key), value);
	}

	void Replace(std::string &str, const char *key, const char *value) {
		size_t index = str.find(key, 0);
		if (index == std::string::npos) {
			return;
		}

		str.replace(index, strlen(key), value);
	}

	void ReplaceAll(std::string &str, const char *key, const char *value) {
		while (true) {
			size_t index = str.find(key, 0);
			if (index == std::string::npos) {
				return;
			}

			str.replace(index, strlen(key), value);
		}
	}
}
