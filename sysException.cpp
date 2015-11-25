#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysException {
	TException::TException(std::wstring &msg) {
		this->msg = msg;
	}

	TException::TException(wchar_t *msg) {
		this->msg = std::wstring(msg);
	}

	const char * TException::what() const {
		char *strMsg = new char [msg.length() + 1];
		std::use_facet<std::ctype<wchar_t> >(loc).narrow(msg.c_str(), msg.c_str() + msg.length() + 1, '?', strMsg);
		return strMsg;
	}

	TExceptionFileNotFound::TExceptionFileNotFound(std::wstring &fileName) : TException(fileName) {
	}

	TExceptionFileNotFound::TExceptionFileNotFound(wchar_t *fileName) : TException(fileName) {
	}
}

