#include "pch.h"

#ifdef  __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysThread {
#ifdef __BORLANDC__

	TThreadTask::~TThreadTask() {
	}

	TThreadPool::TThreadPool() {
	}

	TThreadPool::TThreadPool(size_t threadCount, TThreadTaskCallback callback) {
		this->callback = callback;

		hSemaphore = CreateSemaphore(NULL, 0, MAXINT, NULL);

		InitializeCriticalSection(&csAccess);

		for (size_t i = 0; i < threadCount; ++i) {
			HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, (void *)this, 0, NULL);
			if (hThread != NULL) {
				threads.push_back(hThread);
			}
		}
	}

	TThreadPool::~TThreadPool() {
		DeleteCriticalSection(&csAccess);
	}

	DWORD WINAPI TThreadPool::ThreadFunction(void *vThreadPool) {
		TThreadPool *threadPool = (TThreadPool *)vThreadPool;

		while (WaitForSingleObject(threadPool->hSemaphore, INFINITE) == WAIT_OBJECT_0) {
			TThreadTask *threadTask = threadPool->Pop();
			if (threadTask) {
				threadTask->thread = NULL;
				threadTask->Run();
				threadPool->callback(threadTask);
			}
		}

		return 0;
	}

	void TThreadPool::Push(TThreadTask *threadTask) {
		EnterCriticalSection(&csAccess);

		queue.push_back(threadTask);

		LeaveCriticalSection(&csAccess);

		ReleaseSemaphore(hSemaphore, 1, NULL);
	}

	TThreadTask * TThreadPool::Pop() {
		TThreadTask *threadTask = NULL;

		EnterCriticalSection(&csAccess);

		if (!queue.empty()) {
			threadTask = queue.front();
			queue.pop_front();
		}

		LeaveCriticalSection(&csAccess);

		return threadTask;
	}

	bool TThreadPool::Contain(TThreadTask *threadTask) {
		EnterCriticalSection(&csAccess);

		bool isTaskExist = false;
		for (std::list<TThreadTask *>::iterator i = queue.begin(), iEnd = queue.end(); i != iEnd; ++i) {
			if (**i == threadTask) {
				isTaskExist = true;
				break;
			}
		}

		LeaveCriticalSection(&csAccess);

		return isTaskExist;
	}

	size_t TThreadPool::GetThreadCount() {
		return threads.size();
	}

	size_t TThreadPool::GetTaskQueueCount() {
		size_t size;

		EnterCriticalSection(&csAccess);

		size = queue.size();

		LeaveCriticalSection(&csAccess);

		return size;
	}

	//borland thread pool

	__fastcall __TThread::__TThread(TThreadPool *threadPool) : TThread() {
		this->threadPool = threadPool;
	}

	__fastcall __TThread::~__TThread() {
	}

	void __fastcall __TThread::Execute() {
		while (WaitForSingleObject(threadPool->hSemaphore, INFINITE) == WAIT_OBJECT_0) {
			TThreadTask *threadTask = threadPool->Pop();
			if (threadTask) {
				threadTask->thread = this;
				threadTask->Run();
				threadPool->callback(threadTask);
			}
		}
	}

#endif
};


