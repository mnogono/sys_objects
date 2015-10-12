//---------------------------------------------------------------------------

#ifndef sysExceptionH
#define sysExceptionH

//---------------------------------------------------------------------------

namespace sysException {

class TException : public std::exception {
	private:
		std::locale loc;
	public:
		std::wstring msg;

		TException(std::wstring &msg);

		TException(wchar_t *msg);

		virtual const char *what() const;
};

class TExceptionFileNotFound : public TException {
	public:
	  TExceptionFileNotFound(std::wstring &fileName);
	  TExceptionFileNotFound(wchar_t *fileName);
};

}



#endif
