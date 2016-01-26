//---------------------------------------------------------------------------
#ifndef sysSyncH
#define sysSyncH
//---------------------------------------------------------------------------

namespace sysSync {
	class TCriticalSection {
		protected:
			CRITICAL_SECTION cs;
		public:
			TCriticalSection();
			virtual ~TCriticalSection();
			inline void lock() {
				EnterCriticalSection(&cs);
			}
			inline void unlock() {
				LeaveCriticalSection(&cs);
			}
	};
}

#endif
