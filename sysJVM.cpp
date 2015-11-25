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
		if (jvm->AttachCurrentThread((void **) &env, NULL) != 0) {
			return NULL;
		}
	} else if (getEnvStat == JNI_OK) {
	} else if (getEnvStat == JNI_EVERSION) {
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

JavaVM * CreateJVM(const char *jvmPropertyFile) {
	if (sysFile::IsFileExist(jvmPropertyFile) == false) {
		sysLogger::ERR_A("file not found:");
		sysLogger::ERR_A(jvmPropertyFile);
		return NULL;
	}

	std::ifstream in(jvmPropertyFile, std::ios_base::in);
	std::string line;
	std::vector<std::string> jvmProperties;
	while(in >> line) {
		jvmProperties.push_back(line);
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


JavaVM * CreateJVM(JavaVMInitArgs vm_args) {
	wchar_t *runtimeLib = NULL;
	wchar_t *currentVersion;
	if (sysReg::RegQueryStringValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\JavaSoft\\Java Runtime Environment", L"CurrentVersion", currentVersion) == true) {
		wchar_t javaRuntimeEnvironment[1024];
		swprintf(javaRuntimeEnvironment, 1024, L"SOFTWARE\\JavaSoft\\Java Runtime Environment\\%s", currentVersion);

		if (sysReg::RegQueryStringValue(HKEY_LOCAL_MACHINE, javaRuntimeEnvironment, L"RuntimeLib", runtimeLib) == true) {
			
		} else {
			sysLogger::ERR_A("can't find reg key HKLM\\SOFTWARE\\JavaSorft\\Java Runtime Environment\\%version%\\RuntimeLib");
			return NULL;
		}
	} else {
		sysLogger::ERR_A("can't find reg key HKLM\\SOFTWARE\\JavaSorft\\Java Runtime Environment\\CurrentVersion");
		return NULL;
	}

	if (runtimeLib != NULL && sysFile::IsFileExist(runtimeLib) == true) {
		JavaVM *jvm;
		JNIEnv *env;

		HINSTANCE hinstLib;
		_JNI_CreateJavaVM pJNI_CreateJavaVM;

		hinstLib = LoadLibrary(runtimeLib);
		if (hinstLib == NULL) {
			DWORD e = GetLastError();
			sysLogger::ERR_A("jvm.dll file path:");
			sysLogger::ERR_W(runtimeLib);
			sysLogger::ERR_A("can't load library jvm.dll LastErrorCode: ");
			sysLogger::ERR_A_((int)e);
			return NULL;
		}

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
					sysLogger::ERR_A_(rc);
				}
				return NULL;
			}
		}

		return jvm;
	}

	return NULL;
}

bool RegisterNatives(JNIEnv *env, jclass jClass, JNINativeMethod *methods, int count) {
	if (env->RegisterNatives(jClass, methods, count) < 0) {
		CheckException(env);
		/*
		if (env->ExceptionOccurred()) {
			sysLogger::ERR_A("exception when register natives");
		}
		*/
	}

	return true;
}

}
