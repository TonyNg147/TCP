#pragma once
#include <unordered_map>

#include <EventSource/EventSourceBase.h>
#include <SystemType/EventSourceKind.h>
class EventSourceCollection {
public:
	static EventSourceCollection &getInstance();

	EventSourceCollection();

	~EventSourceCollection();

	EventSource *getSource(const EvenSourceKind &) const;

private:
	std::unordered_map<EvenSourceKind, EventSource *> m_sources;
};