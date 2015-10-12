//---------------------------------------------------------------------------

#ifndef sysVCLH
#define sysVCLH


//---------------------------------------------------------------------------

/**
do not forget to prepare valid OpenGL context before execute any Open GL functions
*/
namespace sysOGL {
	/**
	return version of OpenGL driver installed on user
	@example: "4.4.0 NVIDIA 344.11"
	*/
	const char * GetOpenGLVersion();
}
#endif
