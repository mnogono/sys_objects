//---------------------------------------------------------------------------
#ifndef sysObserverH
#define sysObserverH
//---------------------------------------------------------------------------

//#pragma option push -w-8130

namespace sysObserverable {
	class IObserverable;
}

namespace sysObserver {

	class IObserver {
		public:
			virtual void UpdateData(sysObserverable::IObserverable *data = NULL) = 0;
	};
}

//#pragma option pop

#endif
