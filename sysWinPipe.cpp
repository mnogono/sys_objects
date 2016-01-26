#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysWinPipe {
	/*
	HANDLE CreatePipe(
		const wchar_t *pipeName,
		DWORD openMode,
		DWORD pipeMode,
		DWORD maxInstance,
		DWORD outBufferSize,
		DWORD inBufferSize,
		DWORD timeout) {

		SECURITY_ATTRIBUTES securityAttribute;
		memset(&securityAttribute, 0, sizeof(securityAttribute));
		securityAttribute.nLength = sizeof(securityAttribute);
		securityAttribute.lpSecurityDescriptor = CreateSecurityInfo();
		securityAttribute.bInheritHandle = false;

		HANDLE hPipe = CreateNamedPipe(
			pipeName,
			openMode,
			pipeMode,
			maxInstance,
			outBufferSize,
			inBufferSize,
			timeout,
			&securityAttribute);

		if (hPipe == INVALID_HANDLE_VALUE) {
			sysLogger::ERR_A("Can't create named pipe, GetLastError:");
			sysLogger::ERR_A(GetLastError());
		}

		return hPipe;
	}

	HANDLE GetPipe(
		const wchar_t *pipeName,
		DWORD desiredAccess,
		DWORD shareMode,
		LPSECURITY_ATTRIBUTES securityAttributes,
		DWORD creationDisposition,
		DWORD flagsAndAttributes,
		HANDLE templateFile) {

			HANDLE hPipe = CreateFile(
				pipeName,
				desiredAccess,
				shareMode,
				securityAttributes,
				creationDisposition,
				flagsAndAttributes,
				templateFile);

			if (hPipe == INVALID_HANDLE_VALUE) {
				sysLogger::ERR_A("can't create file, GetLastError:");
				sysLogger::ERR_A(GetLastError());
			}

			return hPipe;
		}
	*/

	bool ReadPipe(HANDLE hPipe, char *buffer, size_t bufferSize, DWORD *readed, OVERLAPPED *overlapped) {
		bool result = ReadFile(hPipe, buffer, bufferSize, NULL, overlapped);
		if (result) {
			sysLogger::ERR_A("Read file error, GLE:");
			sysLogger::ERR_A(GetLastError());

			GetOverlappedResult(hPipe, overlapped, readed, true);
		}

		return result;
	}

	bool WritePipe(HANDLE hPipe, const char *buffer, size_t bufferSize, DWORD *writed, OVERLAPPED *overlapped) {
		bool result = WriteFile(hPipe, buffer, bufferSize, NULL, overlapped);
		if (result) {
			sysLogger::ERR_A("Write file error, GLE:");
			sysLogger::ERR_A(GetLastError());
			GetOverlappedResult(hPipe, overlapped, writed, true);
		}

		return result;
	}

	void ClosePipe(HANDLE hPipe) {
		CancelIo(hPipe);
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
	}

	bool ConnectPipe(HANDLE hPipe, OVERLAPPED *overlapped) {
		DWORD err;
		if (!ConnectNamedPipe(hPipe, overlapped)){
			err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				return HasOverlappedIoCompleted(overlapped);
			} else if (err == ERROR_PIPE_CONNECTED) {
                return true;
            }
		}

		return true;
	}

	void TPipe::setPipeName(const wchar_t *pipeName) {
		if (pipeName != NULL) {
			this->pipeName = new wchar_t[wcslen(pipeName) + 1];
			wcscpy(this->pipeName, pipeName);
		}
	}

	TPipe::TPipe(const wchar_t *pipeName) {
		setPipeName(pipeName);

		createSecurityInfo();

		event = CreateEvent(NULL, TRUE, FALSE, NULL);

		canceledFlag = false;

		InitializeCriticalSection(&cs);

		readPipeCallback = NULL;

		callbackContext = NULL;
	}

	void TPipe::setReadThreadCallback(ReadPipeCallback readPipeCallback) {
		this->readPipeCallback = readPipeCallback;
	}

	TPipe::~TPipe() {
		CloseHandle(event);
		closePipe();
		deleteSecurityInfo();
		DeleteCriticalSection(&cs);

		if (pipeName != NULL) {
			delete []pipeName;
		}
	}

	void TPipe::closePipe() {
		sysWinPipe::ClosePipe(hPipe);
	}

	void TPipe::createSecurityInfo() {
		DWORD dwSIDSize = SECURITY_MAX_SID_SIZE;
		DWORD dwACLSize;
		pSD = NULL;
		pUserSID = NULL;
		pSysSID = NULL;
		pServSID = NULL;
		pAccessACL = NULL;

		pSD = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (pSD == NULL) {
			sysLogger::ERR_A("can't malloc memory for pSD");
			return;
		}

		pUserSID = (PSID)::LocalAlloc(LPTR, dwSIDSize);
		if (pUserSID == NULL) {
			sysLogger::ERR_A("can't alloc memory for pUserSID");
			return;
		}

		pSysSID=(PSID)::LocalAlloc(LPTR,dwSIDSize);
		if (pSysSID == NULL) {
			sysLogger::ERR_A("can't alloc memory for pSysSID");
			return;
		}

		pServSID=(PSID)::LocalAlloc(LPTR,dwSIDSize);
		if (pServSID == NULL) {
			return;
		}

		if (!::InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {
			return;
		}

		if (!::CreateWellKnownSid(WinInteractiveSid, NULL, pUserSID, &dwSIDSize)) {
			return;
		}

		if (!::CreateWellKnownSid(WinLocalSystemSid, NULL, pSysSID, &dwSIDSize)) {
			return;
		}

		if (!::CreateWellKnownSid(WinLocalServiceSid, NULL, pServSID, &dwSIDSize)) {
			return;
		}

		dwACLSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) * 3 - sizeof(DWORD)*3;
		dwACLSize += ::GetLengthSid(pUserSID);
		dwACLSize += ::GetLengthSid(pSysSID);
		dwACLSize += ::GetLengthSid(pServSID);
		dwACLSize = (dwACLSize + 0x3) & (~0x3);

		pAccessACL = (PACL)::LocalAlloc(LPTR,dwACLSize);

		if (pAccessACL == NULL) {
			return;
		}

		memset(pAccessACL, 0, dwACLSize);

		if (!::InitializeAcl(pAccessACL,dwACLSize,ACL_REVISION)) {
			return;
		}

		if (!::AddAccessAllowedAce(pAccessACL, ACL_REVISION, GENERIC_READ | GENERIC_WRITE, pUserSID)) {
			return;
		}

		if (!::AddAccessAllowedAce(pAccessACL, ACL_REVISION, GENERIC_READ | GENERIC_WRITE, pSysSID)) {
			return;
		}

		if (!::AddAccessAllowedAce(pAccessACL, ACL_REVISION, GENERIC_READ | GENERIC_WRITE, pServSID)) {
			return;
		}

		if (!::SetSecurityDescriptorDacl(pSD, TRUE, pAccessACL, FALSE)) {
			return;
        }
	}

	void TPipe::deleteSecurityInfo() {
		if (pSD != NULL) {
			free((void*)pSD);
		}

		if (pAccessACL != NULL) {
			LocalFree(pAccessACL);
		}

		if (pSysSID != NULL) {
			LocalFree(pSysSID);
		}

		if (pUserSID != NULL) {
			LocalFree(pUserSID);
		}

		if (pServSID != NULL) {
			LocalFree(pServSID);
        }
		pSysSID = NULL;
		pUserSID = NULL;
		pServSID = NULL;
		pAccessACL = NULL;
		pSD = NULL;
	}

	bool TPipe::read(char *buffer, size_t bufferSize, DWORD *readed, OVERLAPPED *overlapped) {
		bool result = ReadFile(hPipe, buffer, bufferSize, NULL, overlapped);
		if (result) {
			sysLogger::ERR_A("Read file error, GLE:");
			sysLogger::ERR_A(GetLastError());

			GetOverlappedResult(hPipe, overlapped, readed, true);
		}

		return result;
	}

	bool TPipe::write(char *buffer, size_t bufferSize, DWORD *writed, OVERLAPPED *overlapped) {
		bool result = WriteFile(hPipe, buffer, bufferSize, NULL, overlapped);
		if (result) {
			sysLogger::ERR_A("Write file error, GLE:");
			sysLogger::ERR_A(GetLastError());
			GetOverlappedResult(hPipe, overlapped, writed, true);
		}

		return result;
	}

	bool TPipe::writeSync(char *buffer, size_t bufferSize) {
		OVERLAPPED overlapped;
		memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = event;
		ResetEvent(event);
		DWORD timeout = 100;
		DWORD readedBytes;

		DWORD err;
		if (!WriteFile(hPipe, buffer, bufferSize, NULL, &overlapped)) {
			err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				while(1) {
				if (WaitForSingleObject(event, timeout) == WAIT_OBJECT_0 &&
					GetOverlappedResult(hPipe, &overlapped, &readedBytes, false)){
						return true;
					}
				}
			}
		}

		return false;
	}

	DWORD WINAPI _startServerThread(void *pipe) {
		static_cast<TPipe *>(pipe)->mainServerLoop();
		ExitThread(0);
		return 0;
	}

	DWORD WINAPI _startClientThread(void *pipe) {
		static_cast<TPipe *>(pipe)->mainClientLoop();
		ExitThread(0);
		return 0;
	}

	void TPipe::mainServerLoop() {
		if (waitConnection()) {
			read();
		}
	}

	void TPipe::mainClientLoop() {
		OVERLAPPED overlapped;
		memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = event;

		ResetEvent(event);
	}

	void TPipe::read() {
		const size_t size = SYS_WIN_PIPE_DEFAULT_BUFFER_SIZE;
		char buffer[size];
		DWORD err;
		DWORD timeout = 10;
		DWORD readedBytes = 0;

		OVERLAPPED overlapped;

		while(1) {
			if (canceled()) {
				break;
			}

			memset(&overlapped, 0, sizeof(overlapped));
			overlapped.hEvent = event;
			ResetEvent(event);

			if (!ReadFile(hPipe, buffer, size, &readedBytes, &overlapped)) {
				err = GetLastError();
				if (err == ERROR_IO_PENDING) {
					while(1) {
						if (canceled()) {
							break;
						}

						if (WaitForSingleObject(event, timeout) == WAIT_OBJECT_0) {
							if (GetOverlappedResult(hPipe, &overlapped, &readedBytes, false)) {
								callback(buffer, readedBytes, callbackContext);
								/*
								char _readedBytes[sizeof(readedBytes)];
								memcpy(&_readedBytes[0], &readedBytes, sizeof(readedBytes));
								writeSync(_readedBytes, sizeof(readedBytes));
								*/
							} else {
								sysLogger::ERR_A("get overlappped result, GLE");
								sysLogger::ERR_A(GetLastError());
							}
							break;
						}
					}
				}
			} else {
				OutputDebugString(L"asdasd");
				callback(buffer, readedBytes, callbackContext);
			}
		}
	}

	void TPipe::callback(char *buffer, DWORD size, void *callbackContext) {
		if (readPipeCallback) {
			lock();
			readPipeCallback(buffer, size, callbackContext);
			unlock();
		}
	}


	bool TPipe::waitConnection() {
		OVERLAPPED overlapped;
		memset(&overlapped, 0, sizeof(overlapped));
		overlapped.hEvent = event;

		ResetEvent(event);

		DWORD err;
		DWORD overlappedResult = 0;
		DWORD timeout = 100;

		//wait until get client pipe connection, check connection every timeout ms
		if (!ConnectNamedPipe(hPipe, &overlapped)) {
			err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				//wating ending of connection procedure
				while (1) {
					if (canceled()) {
						break;
					}

					if (WaitForSingleObject(event, timeout) == WAIT_OBJECT_0 &&
						GetOverlappedResult(hPipe, &overlapped, &overlappedResult, false)) {
						overlappedResult = ERROR_PIPE_CONNECTED;
						break;
					}
				}
			}
		}

		return overlappedResult == ERROR_PIPE_CONNECTED;
	}

	void TPipe::create(
		DWORD openMode,
		DWORD pipeMode,
		DWORD maxInstance,
		DWORD outBufferSize,
		DWORD inBufferSize,
		DWORD timeout) {

        createSecurityInfo();

		SECURITY_ATTRIBUTES securityAttribute;
		memset(&securityAttribute, 0, sizeof(securityAttribute));
		securityAttribute.nLength = sizeof(securityAttribute);
		securityAttribute.lpSecurityDescriptor = pSD;
		securityAttribute.bInheritHandle = false;

		hPipe = CreateNamedPipe(
			pipeName,
			openMode,
			pipeMode,
			maxInstance,
			outBufferSize,
			inBufferSize,
			timeout,
			&securityAttribute);

		if (hPipe == INVALID_HANDLE_VALUE) {
			sysLogger::ERR_A("Can't create named pipe, GetLastError:");
			sysLogger::ERR_A(GetLastError());
			return;
		}

		//create new thread and wait untill pipe connection
		DWORD threadId;
		CreateThread(NULL, 0, &_startServerThread, static_cast<void *>(this), 0, &threadId);
	}

	void TPipe::connect(
		const wchar_t *pipeName,
		DWORD desiredAccess,
		DWORD shareMode,
		LPSECURITY_ATTRIBUTES securityAttributes,
		DWORD creationDisposition,
		DWORD flagsAndAttributes,
		HANDLE templateFile) {

		hPipe = CreateFile(
			pipeName,
			desiredAccess,
			shareMode,
			securityAttributes,
			creationDisposition,
			flagsAndAttributes,
			templateFile);

		if (hPipe == INVALID_HANDLE_VALUE) {
			sysLogger::ERR_A("can't create file, GetLastError:");
			sysLogger::ERR_A(GetLastError());
		}
	}

	void TPipe::cancel() {
		lock();
		canceledFlag = true;
		unlock();
	}

	bool TPipe::canceled() {
		bool _canceled;
		lock();
		_canceled = canceledFlag;
		unlock();

		return _canceled;
	}

	void TPipe::lock() {
    	EnterCriticalSection(&cs);
	}

	void TPipe::unlock() {
    	LeaveCriticalSection(&cs);
	}

	void TPipe::setCallbackContext(void *callbackContext) {
		this->callbackContext = callbackContext;
	}
}
