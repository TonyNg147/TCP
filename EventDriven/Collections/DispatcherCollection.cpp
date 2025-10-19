#include "DispatcherCollection.h"
#include <Dispatcher/FDDispatcher.h>

DispatcherCollection &DispatcherCollection::getInstance() {
	static DispatcherCollection instance;
	return instance;
}

DispatcherCollection::DispatcherCollection() {
	m_dispatchers[DispatcherKind::FD] = new FDDisPatcher;
}

DispatcherCollection::~DispatcherCollection() {
	for (auto &it : m_dispatchers) {
		delete it.second;
	}
}

Dispatcher *
DispatcherCollection::getSource(const DispatcherKind &dispatcherKind) const {
	if (auto it = m_dispatchers.find(dispatcherKind);
		it != m_dispatchers.end()) {
		return it->second;
	}
	return nullptr;
}