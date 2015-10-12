//---------------------------------------------------------------------------

#ifndef sysThreadH
#define sysThreadH
//---------------------------------------------------------------------------

namespace sysThread {

	class TThreadPool;

	//Base class for thread tasks
	class TThreadTask {
		protected:
			TThreadPool *threadPool;
		public:
			//thread executed current task
			void *thread;
		public:
			//main function of the thread task
			virtual void Run() = 0;

			//function invoke when task is finished and return to the pool
			virtual void CleanUp() = 0;

			void Release();

			virtual ~TThreadTask();

			virtual bool operator == (const TThreadTask *task) = 0;

			virtual bool operator == (TThreadTask *task) = 0;
	};

	//Callback function invoked after task is done
	typedef void (*TThreadTaskCallback)(TThreadTask *);

    class __TThread;

	//Base class for thead pool
	class TThreadPool {
		friend class __TThread;
		protected:
			CRITICAL_SECTION csAccess;

			std::list<TThreadTask *> queue;

			size_t threadCount;

			std::list<HANDLE> threads;

			HANDLE hSemaphore;

			TThreadTaskCallback callback;

		protected:
			static DWORD WINAPI ThreadFunction(void *vThreadPool);

			//Remove the first task from the pool
			TThreadTask * Pop();

		public:
        	TThreadPool();

			TThreadPool(size_t threadCount, TThreadTaskCallback callback);

			virtual ~TThreadPool();

			//Push new task into thread pool
			void Push(TThreadTask *task);

			/*
			check is specify task already in thread pool queue
			TThreadTask must be support the operator =
			*/
			bool Contain(TThreadTask *task);

			//return thread pool size, count of threads
			virtual size_t GetThreadCount();

			//return size of task queue
			virtual size_t GetTaskQueueCount();
	};

	//thread pool with builder TTread objects instead of native window thread
	//internal thread need to be created into thread pool

	template<class T>
	class TThreadPoolBorland;

	class __TThread : public TThread {
		protected:
			TThreadPool *threadPool;
		public:
			__fastcall __TThread(TThreadPool *threadPool);

			__fastcall virtual ~__TThread();

			virtual void __fastcall Execute();
//			virtual void __fastcall UIUpdate() = 0;
	};

	template<class T>
	class TThreadPoolBorland : public TThreadPool {
		protected:
			std::list<T *> threads;
		public:

		TThreadPoolBorland::TThreadPoolBorland(size_t threadCount, TThreadTaskCallback callback) {
			this->callback = callback;

			hSemaphore = CreateSemaphore(NULL, 0, MAXINT, NULL);

			InitializeCriticalSection(&csAccess);

			for (size_t i = 0; i < threadCount; ++i) {
				T *thread = new T(this);
				threads.push_back(thread);
			}
		}

		TThreadPoolBorland::~TThreadPoolBorland() {
		}

		virtual size_t GetSize() {
			size_t size;

			EnterCriticalSection(&csAccess);

			size = threads.size();

			LeaveCriticalSection(&csAccess);

			return size;
		}
	};

}

#endif
