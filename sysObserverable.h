//---------------------------------------------------------------------------
#ifndef sysObserverableH
#define sysObserverableH
//---------------------------------------------------------------------------
namespace sysObserverable {
	//interface
	class IObserverable {
		public:
			virtual void AddObserver(sysObserver::IObserver *observer) = 0;
			virtual void RemoveObserver(sysObserver::IObserver *observer) = 0;
			virtual void NotifyObservers(IObserverable *data = NULL) = 0;

			//return uniq id for identify what kind of observerable it is
			//specifyed by software application
			virtual int GetObserverableType() = 0;
	};

	//simple implementation
	class TObserverable : public IObserverable {
		protected:
			std::list<sysObserver::IObserver *> observers;
		public:
			virtual ~TObserverable();

			virtual void AddObserver(sysObserver::IObserver *observer);

			virtual void RemoveObserver(sysObserver::IObserver *observer);

			//TODO here should be IObserverableData not IObserverable
			virtual void NotifyObservers(IObserverable *data = NULL);

			virtual int GetObserverableType();
	};

}

#endif
