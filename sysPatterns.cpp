#include "pch.h"

#ifdef  __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysPatterns {
    /*
	TObserverable::~TObserverable() {
		observers.clear();
	}

	void TObserverable::AddObserver(sysObserver::IObserver *observer) {
		observers.push_back(observer);
	}

	void TObserverable::RemoveObserver(sysObserver::IObserver *observer) {
		observers.remove(observer);
	}

	void TObserverable::NotifyObservers() {
		for (itObserver it = observers.begin(), itEnd = observers.end(); it != itEnd; ++it) {
			(*it)->Update();
		}
	}
	*/
}

