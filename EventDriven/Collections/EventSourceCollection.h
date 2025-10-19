#pragma once
#include <EventSource/EventSourceBase.h>
#include <SystemType/EventSourceKind.h>
#include <unordered_map>
class EventSourceCollection {
public:
	static EventSourceCollection &getInstance();

	EventSourceCollection();

	~EventSourceCollection();

	EventSource *getSource(const EvenSourceKind &) const;

private:
	std::unordered_map<EvenSourceKind, EventSource *> m_sources;
};