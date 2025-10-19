#include "EventSourceCollection.h"
#include "../EventSource/FDEventSource.h"

EventSourceCollection &EventSourceCollection::getInstance() {
	static EventSourceCollection instance;
	return instance;
}

EventSourceCollection::EventSourceCollection() {
	m_sources[EvenSourceKind::FD] = new FDEventSource;
}
EventSourceCollection::~EventSourceCollection() {
	for (auto &it : m_sources) {
		delete it.second;
	}
}

EventSource *
EventSourceCollection::getSource(const EvenSourceKind &sourceKind) const {
	if (auto it = m_sources.find(sourceKind); it != m_sources.end()) {
		return it->second;
	}
	return nullptr;
}