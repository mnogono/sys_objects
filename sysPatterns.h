//---------------------------------------------------------------------------
#ifndef sysPatternsH
#define sysPatternsH
//---------------------------------------------------------------------------

namespace sysPatterns {
	// Observer - Observerable pattern
	/*
	class TObserver : public sysObserver::IObserver {
		public:
			virtual ~TObserver();
			virtual void Update();
	};
	*/
	/*
	typedef std::list<sysObserver::IObserver *>::iterator itObserver;

	class TObserverable : public sysObserverable::IObserverable {
		protected:
			std::list<sysObserver::IObserver *> observers;
		public:
			virtual ~TObserverable();
			virtual void AddObserver(sysObserver::IObserver *observer);
			virtual void RemoveObserver(sysObserver::IObserver *observer);
			virtual void NotifyObservers();
	};
	*/

	class IPooledObject {
		public:
			//when pooled object released back to pool, the CleanUp function invoked
			virtual void CleanUp() = 0;
	};

	// Pool objects pattern
	/*
	it is a good idea to implement class T from TPooledObject, for auto perform CleanUp procedure
	*/
	template<class T /* class T must be implement IPooledObject */>
	class TPool {
		private:
			CRITICAL_SECTION csAccess;
			std::list<T *> used;
			std::list<T *> available;
		public:
			TPool() {
				InitializeCriticalSection(&csAccess);
			}

			size_t GetUsedSize() {
				size_t size;

				EnterCriticalSection(&csAccess);

				size = used.size();

				LeaveCriticalSection(&csAccess);

				return size;
			}

			size_t GetAvailableSize() {
				size_t size;

				EnterCriticalSection(&csAccess);

				size = available.size();

				LeaveCriticalSection(&csAccess);

				return size;
			}

			//request 1 object from object pool
			template<class K>
			std::list<T *>::iterator const Get() {
				EnterCriticalSection(&csAccess);

				T *t;
				if (available.size() != 0) {
					t = available.front();
					available.pop_front();
				} else {
					t = new K();
				}
				used.push_back(t);

				std::list<T *>::iterator i = used.end();
				--i;

				LeaveCriticalSection(&csAccess);

				return i;
			}

			//request count objects from object pool
			template<class K>
			std::list<T *> Get(size_t count) {
				std::list<T *> objects;

				EnterCriticalSection(&csAccess);

				T *t;

				for (int itObject = 0; itObject < count; ++itObject) {
					if (available.size() != 0) {
						t = available.front();
						available.pop_front();
					} else {
						t = new K();
					}
					used.push_back(t);

					objects.push_back(t);
				}

				LeaveCriticalSection(&csAccess);

				return objects;
			}

			void Release(const std::list<T *>::iterator &i) {
				EnterCriticalSection(&csAccess);

				(*i)->CleanUp();

				available.push_back(*i);
				used.erase(i);

				LeaveCriticalSection(&csAccess);
			}

			void Release(T *pooledObject) {
				EnterCriticalSection(&csAccess);

				for (std::list<T *>::iterator i = used.begin(), iEnd = used.end(); i != iEnd; ++i) {
					if (*i == pooledObject) {

						(*i)->CleanUp();

						used.erase(i);
						available.push_back(pooledObject);
						break;
					}
				}

				LeaveCriticalSection(&csAccess);
			}
			/*
			void Release(const std::list<T *>::iterator &i) {
				EnterCriticalSection(&csAccess);

				TPooledObject *pooledObject = dynamic_cast<TPooledObject *>(*i);
				if (pooledObject != NULL) {
					pooledObject->CleanUp();
				}

				available.push_back(*i);
				used.erase(i);

				LeaveCriticalSection(&csAccess);
			}

			void Release(T *pooledObject) {
				EnterCriticalSection(&csAccess);

				for (std::list<T *>::iterator i = used.begin(), iEnd = used.end(); i != iEnd; ++i) {
					if (*i == pooledObject) {
						if (dynamic_cast<TPooledObject *>(*i)) {
							(static_cast<TPooledObject *>(*i))->CleanUp();
						}

						used.erase(i);
						available.push_back(pooledObject);
						break;
					}
				}

				LeaveCriticalSection(&csAccess);
			}
			*/
	};
}

#endif
