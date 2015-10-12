//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace sysOGL {

	const char * GetOpenGLVersion() {
		const GLubyte * buffer = glGetString(GL_VERSION);
		return (char *)buffer;
	}
}
