//---------------------------------------------------------------------------
#ifndef sysWinPipeH
#define sysWinPipeH
//---------------------------------------------------------------------------

namespace sysWinPipe {
	/**
	create new named pipe
	return handle of created object
	check return handle value for INVALID_HANDLE_VALUE

	pipeName
	The name of the named pipe.
	The string must include the name of the computer
	on which the server process is executing.
	A period (.) may be used for the servername if the pipe is local.
	The following pipe name format is used:
	\\servername\pipe\pipename

	example const wchar_t *pipeName = L"\\\\.\\pipe\\MyPipe";
	*/
	/*
	HANDLE CreatePipe(
		const wchar_t *pipeName,
		DWORD openMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		DWORD pipeMode = PIPE_WAIT | PIPE_READMODE_BYTE | PIPE_TYPE_BYTE,
		DWORD maxInstance = 1,
		DWORD outBufferSize = 4096,
		DWORD inBufferSize = 4096,
		DWORD timeout = 5000
		);

	HANDLE GetPipe(
		const wchar_t *pipeName,
		DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE,
		DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE,
		LPSECURITY_ATTRIBUTES securityAttributes = NULL,
		DWORD creationDisposition = OPEN_EXISTING,
		DWORD flagsAndAttributes = FILE_FLAG_OVERLAPPED,
		HANDLE templateFile = NULL
		);

	PSECURITY_DESCRIPTOR CreateSecurityInfo();

	bool ReadPipe(HANDLE hPipe, char *buffer, size_t bufferSize, DWORD *readed, OVERLAPPED *overlapped);

	bool WritePipe(HANDLE hPipe, const char *buffer, size_t bufferSize, DWORD *writed, OVERLAPPED *overlapped);

	void ClosePipe(HANDLE hPipe);

	bool ConnectPipe(HANDLE hPipe, OVERLAPPED *overlapped);
	*/

	typedef void (*ReadPipeCallback)(char *buffer, DWORD size);

	class TPipe {
		public:
			TPipe(const wchar_t *pipeName = NULL);

			virtual ~TPipe();

			bool read(
				char *buffer,
				size_t bufferSize,
				DWORD *readed,
				OVERLAPPED *overlapped);

            bool writeSync(char *buffer, size_t bufferSize);

			bool write(
				char *buffer,
				size_t bufferSize,
				DWORD *readed,
				OVERLAPPED *overlapped);

			void create(
				//const wchar_t *pipeName,
				DWORD openMode = PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
				DWORD pipeMode = PIPE_WAIT | PIPE_READMODE_BYTE | PIPE_TYPE_BYTE,
				DWORD maxInstance = 1,
				DWORD outBufferSize = 4096,
				DWORD inBufferSize = 4096,
				DWORD timeout = 5000);

			//connect to pipe
			void connect(
				const wchar_t *pipeName,
				DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE,
				DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE,
				LPSECURITY_ATTRIBUTES securityAttributes = NULL,
				DWORD creationDisposition = OPEN_EXISTING,
				DWORD flagsAndAttributes = FILE_FLAG_OVERLAPPED,
				HANDLE templateFile = NULL
			);

			void setPipe(const wchar_t *pipeName);

			//new thread server main procedure
			//waiting connection and read from pipe
			void mainServerLoop();

			//new thread client main procedure
			//write to pipe
			void mainClientLoop();

			void setCallback(ReadPipeCallback readPipeCallback);

			void setPipeName(const wchar_t *pipeName);

		private:
			wchar_t *pipeName;
			PSECURITY_DESCRIPTOR pSD;
			PSID pUserSID;
			PSID pSysSID;
			PSID pServSID;
			PACL pAccessACL;
			CRITICAL_SECTION cs;
			ReadPipeCallback readPipeCallback;

			bool canceledFlag;

			void closePipe();
			void deleteSecurityInfo();
			void createSecurityInfo();

			//waiting cient pipe connection
			bool waitConnection(OVERLAPPED *overlapped);

			//loop reading pipe buffer
			void read(OVERLAPPED *overlapped);

			//return if true for interrupt waiting pipe connection or reading pipe data
			bool canceled();

			//set cancel flag
			void cancel();

			//enter critical section
			void lock();

			//leave critical section
			void unlock();

			//invoke callback when buffer reads from pipe
			void callback(char *buffer, DWORD size);

		public:
			HANDLE event;
			HANDLE hPipe;
	};
}

#endif
