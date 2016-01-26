#include "pch.h"
#include "sysJVM.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysJVM {

JNIEnv * AttachCurrentThread(JavaVM *jvm) {
	JNIEnv * env;
	int getEnvStat = jvm->GetEnv((void **)&env, JNI_VERSION);
	if (getEnvStat == JNI_EDETACHED) {
		if (jvm->AttachCurrentThread((void **) &env, NULL) != JNI_OK) {
			sysLogger::ERR_A("AttachCurrentThread FAIL (JNI_EDETACHED)");

			return NULL;
		} else {
			//sysLogger::TRACE_A("jvm->AttachCurrentThread == JNI_OK");
		}

	} else if (getEnvStat == JNI_OK) {
		//sysLogger::TRACE_A("AttachCurrentThread JNI_OK");

	} else if (getEnvStat == JNI_EVERSION) {
		sysLogger::ERR_A("AttachCurrentThread FAIL (JNI_EVERSION)");

		return NULL;
	}

	return env;
}

void DetachCurrentThread(JavaVM *jvm) {
	jvm->DetachCurrentThread();
}

void CheckException(JNIEnv *env) {
	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

JavaVM * createJVM() {
	JavaVMOption options[1];
	options[0].optionString = "-Djava.class.path=.";

	JavaVMInitArgs vm_args;                        // Initialization arguments
	vm_args.version = JNI_VERSION_1_6;             // minimum Java version
	vm_args.nOptions = 1;       // number of options
	vm_args.options = options;
	vm_args.ignoreUnrecognized = false;            // invalid options make the JVM init fail

	return CreateJVM(vm_args);
}

JavaVM * CreateJVM(const wchar_t *jvmPropertyFile) {
	char *buffer;
	if (sysStr::WToMB(jvmPropertyFile, buffer)) {
		JavaVM *jvm = CreateJVM(buffer);
		delete []buffer;
		return jvm;
	}

	return NULL;
}

JavaVM * CreateJVM(const char *jvmPropertyFile) {
	if (sysFile::IsFileExist(jvmPropertyFile) == false) {
		sysLogger::ERR_A("file not found:");
		sysLogger::ERR_A(jvmPropertyFile);
		return NULL;
	}

	std::ifstream in(jvmPropertyFile, std::ios_base::in);
	std::string line;
	std::vector<std::string> jvmProperties;

	const size_t bufsize = 1024;
	char lineBuffer[bufsize];
	char parameter[bufsize];
	char value[bufsize];
	std::string jvmProperty;
	//std::string strParameter;
	//std::string strValue;
	//std::string strJavaClassPath = "-Djava.class.path";
	/*
	while (!in.eof()) {
		in.getline(parameter, bufsize, '=');
		in.getline(value, bufsize);
		strParameter = parameter;
		if (strJavaClassPath == strParameter) {
			std::string absolute = sysFile::GetAbsolutePath(value);
			absolute.replace(
		} else {
			jvmProperty = strParameter + "=" + strValue;
		}
		//strParameter.assign(parameter[0], parameter[
		//if (std::string("")
	}
	*/
	
	while (!in.eof()) {
		in.getline(lineBuffer, bufsize);
		if (strncmp("-Djava.class.path=", lineBuffer, 18) == 0) {
			char *classPath = sysFile::GetAbsolutePath(&lineBuffer[18]);
			sysStr::ReplaceAll(classPath, '\\', '/');
			std::string propertyClassPath(std::string("-Djava.class.path=") + std::string(classPath));
			delete []classPath;

			jvmProperty = propertyClassPath;
		} else {
			jvmProperty = lineBuffer;
		}

		sysLogger::TRACE_A(jvmProperty.c_str());

		jvmProperties.push_back(jvmProperty);
	}
	
	
	in.close();

	JavaVMOption *options = new JavaVMOption[jvmProperties.size()];
	for (int i = 0; i < jvmProperties.size(); ++i) {
		options[i].optionString = new char[jvmProperties[i].length() + 1];
		strncpy(options[i].optionString, jvmProperties[i].c_str(), jvmProperties[i].length() + 1);
	}

	JavaVMInitArgs vm_args;                        // Initialization arguments
	vm_args.version = JNI_VERSION_1_6;             // minimum Java version
	vm_args.nOptions = jvmProperties.size();       // number of options
	vm_args.options = options;
	vm_args.ignoreUnrecognized = false;            // invalid options make the JVM init fail

	return CreateJVM(vm_args);
}

HINSTANCE LoadJvmDll() {
	static HINSTANCE hinstLib = NULL;
	if (hinstLib == NULL) {
		wchar_t *currentVersion = NULL;
		wchar_t *javaHome = NULL;

		if (sysReg::RegQueryStringValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\JavaSoft\\Java Runtime Environment", L"CurrentVersion", currentVersion) == true) {
			wchar_t regKey[1024];
			swprintf(regKey, 1024, L"SOFTWARE\\JavaSoft\\Java Runtime Environment\\%s", currentVersion);

			if (sysReg::RegQueryStringValue(HKEY_LOCAL_MACHINE, regKey, L"JavaHome", javaHome) == true) {
				wchar_t jvmDllPath[1024];
				swprintf(jvmDllPath, 1024, L"%s\\bin\\server\\jvm.dll", javaHome);

				if (sysFile::IsFileExist(jvmDllPath) == true) {
					hinstLib = LoadLibrary(jvmDllPath);
					if (hinstLib == NULL) {
						DWORD e = GetLastError();
						sysLogger::ERR_A("jvm.dll file path:");
						sysLogger::ERR_W(jvmDllPath);
						sysLogger::ERR_A("can't load library jvm.dll LastErrorCode: ");
						sysLogger::ERR_A((int)e);
						return NULL;
					}
				} else {
					sysLogger::ERR_A("jvm.dll not found:");
					sysLogger::ERR_W(jvmDllPath);
				}
			} else {
				sysLogger::ERR_A("can't find reg key:");
				sysLogger::ERR_W(regKey);
			}
		} else {
			sysLogger::ERR_A("can't find reg key HKLM\\SOFTWARE\\JavaSorft\\Java Runtime Environment\\CurrentVersion");
		}

		if (currentVersion != NULL) {
			delete []currentVersion;
			currentVersion = NULL;
		}	
	
		if (javaHome != NULL) {
			delete []javaHome;
			javaHome = NULL;
		}


	}

	return hinstLib;
}

JavaVM * CreateJVM(JavaVMInitArgs vm_args) {
	HINSTANCE hinstLib = LoadJvmDll();
	if (hinstLib == NULL) {
		return NULL;
	}

	//check is jvm is already created
	//jvm can't be created againe in the one thread

	_JNI_GetCreatedJavaVMs pJNI_GetCreatedJavaVMs;
	pJNI_GetCreatedJavaVMs = (_JNI_GetCreatedJavaVMs) GetProcAddress(hinstLib, "JNI_GetCreatedJavaVMs");
	if (pJNI_GetCreatedJavaVMs != NULL) {
		JavaVM *jvms = new JavaVM[1];
		jsize count;

		//try to get first of created java vm and return it
		jint rc = pJNI_GetCreatedJavaVMs(&jvms, 1, &count);
		if (count > 0) {
			return &jvms[0];
		}
	}

	JNIEnv *env;
	_JNI_CreateJavaVM pJNI_CreateJavaVM;
	JavaVM *jvm = NULL;

	pJNI_CreateJavaVM = (_JNI_CreateJavaVM) GetProcAddress(hinstLib, "JNI_CreateJavaVM");
	if (pJNI_CreateJavaVM != NULL) {
		jint rc = pJNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
		if(rc != JNI_OK) {
			if(rc == JNI_EVERSION) {
				sysLogger::ERR_A("VM is oudated and doesn't meet requirements");
			} else if(rc == JNI_ENOMEM) {
				sysLogger::ERR_A("not enough memory for JVM");
			} else if(rc == JNI_EINVAL) {
				sysLogger::ERR_A("invalid ragument for launching JVM");
			} else if(rc == JNI_EEXIST) {
				sysLogger::ERR_A("the process can only launch one JVM an not more");
			} else {
				sysLogger::ERR_A("could not create the JVM instance error code: ");
				sysLogger::ERR_A(rc);
			}

			sysLogger::ERR_A("could not create the JVM unknown error code");
		}
	}

	return jvm;
}

bool RegisterNatives(JNIEnv *env, jclass jClass, JNINativeMethod *methods, int count) {
	//sysLogger::TRACE_A("0");

	if (env->RegisterNatives(jClass, methods, count) < 0) {

		//sysLogger::TRACE_A("1");

		//CheckException(env);
		if (env->ExceptionOccurred()) {
			sysLogger::ERR_A("exception when register natives");
		}
	}

	//sysLogger::TRACE_A("2");

	return true;
}

}
