#ifndef sysJVMH
#define sysJVMH

#include "jni.h"

namespace sysJVM {

#ifndef JNI_VERSION
#define JNI_VERSION JNI_VERSION_1_6
#endif

extern bool JAVA_REMOTE_DEBUGGER;

typedef jint (__stdcall *_JNI_CreateJavaVM)(JavaVM **pvm, void **penv, void *args);
typedef jint (__stdcall *_JNI_GetCreatedJavaVMs)(JavaVM **vmBuf, jsize bufLen, jsize *nVMs);

JNIEnv * AttachCurrentThread(JavaVM *jvm);

void DetachCurrentThread(JavaVM *jvm);

JavaVM * CreateJVM();

JavaVM * CreateJVM(const char *jvmPropertyFile);

JavaVM * CreateJVM(const wchar_t *jvmPropertyFile);

JavaVM * CreateJVM(JavaVMInitArgs vm_args);

//JavaVM * CreateJVM(JavaVMInitArgs vm_args);

//check exception of jvm and print it into stderr stream and clear exception stack
void CheckException(JNIEnv *env);

/*
register native callback natives will be invokes from java into c++

*/
bool RegisterNatives(JNIEnv *env, jclass jClass, JNINativeMethod *methods, int count);

}

#endif
