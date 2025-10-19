#include "FDDispatcher.h"
#include <EventSourceBase.h>
#include <Streamable.h>
#include <algorithm>

FDDisPatcher::FDDisPatcher() { m_listeners = {}; }

FDDisPatcher::~FDDisPatcher() {}

bool FDDisPatcher::register_event_listener(
	const IEvent &ev, const EventListener *const listener) {
	validate_and_raise_error();
	// Cast up to FDEvent type. This DIspatcher is dedicated to handle all the
	// stuff related to FD
	if (auto event = dynamic_cast<FDEvent *>((IEvent *)&ev); event != nullptr) {
		if (!likely_pollable(event->fd))
			return false;
		// If we've already registered this kind of event to the corresponding
		// EventSource then we are exempted to register it again.
		if (auto it = m_listeners.find(*event); it != m_listeners.end()) {
			it->second.push_back((EventListener *)listener);
		} else {
			m_listeners[*event] =
				std::vector<EventListener *>{(EventListener *)listener};
			m_eventSource->subscribeEvent(event);
		}
		return true;
	}
	return false;
}

void FDDisPatcher::validate_and_raise_error() {
	if (m_eventSource == nullptr) {
		throw std::runtime_error("Dispatcher operations failed. Must provide "
								 "EventSource beforehand.");
	}
}

void FDDisPatcher::revoke(const EventListener *const listener) {
	for (auto it = m_listeners.begin(); it != m_listeners.end();) {
		auto &listenerOfAKind = it->second;
		auto found_it =
			std::remove_if(listenerOfAKind.begin(), listenerOfAKind.end(),
						   [this, listener](const EventListener *evlistener) {
							   return evlistener == listener;
						   });

		listenerOfAKind.erase(found_it, listenerOfAKind.end());
		// If the specific interested vector becomes empty, remove that instance
		// of that kind
		if (listenerOfAKind.empty()) {
			// remove the iterator refers to that instance. The return value
			// will point to next element.
			m_eventSource->unsubscribeEvent(&it->first);
			it = m_listeners.erase(it);
		} else {
			++it;
		}
	}
}

int FDDisPatcher::totalEvent() const { return m_listeners.size(); }

void FDDisPatcher::assign_event_source(EventSource *eventSource) {
	if (eventSource != nullptr) {
		m_eventSource = eventSource;
	}
}

void FDDisPatcher::submitEvent(const IEvent *ev) {
	// A cost on performance will be imposed at this point
	// when we have to cast the Event Type
	FDEvent *event = dynamic_cast<FDEvent *>((IEvent *)ev);
	if (event == nullptr) {
		std::cerr << "Dimiss the event not related to FD for FDDispatcher\n";
	}
	if (auto it = m_listeners.find(*event); it != m_listeners.end()) {
		for (auto &listener : it->second) {
			listener->onEvent(event);
		}
	}
}
