#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysObserverable {
	TObserverable::~TObserverable() {
		observers.clear();
	}

	void TObserverable::AddObserver(sysObserver::IObserver *observer) {
		observers.push_back(observer);
	}

	void TObserverable::RemoveObserver(sysObserver::IObserver *observer) {
		observers.remove(observer);
	}

	void TObserverable::NotifyObservers(IObserverable *data) {
		for (std::list<sysObserver::IObserver *>::iterator i = observers.begin(), iEnd = observers.end(); i != iEnd; ++i) {
			(*i)->UpdateData(data);
		}
	}

	int TObserverable::GetObserverableType() {
    	return 0;
	}
}
