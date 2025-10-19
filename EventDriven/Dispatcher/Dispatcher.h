#pragma once
#include <functional>
#include <iostream>
#include <sys/epoll.h>

#include <Broker/EventBroker.h>
#include <EventListener/BaseEventListener.h>

struct EventSource;

// This class will manage all the Events and Event Listerners
class Dispatcher : public EventBroker {
public:
	virtual bool register_event_listener(const IEvent &,
										 const EventListener *const) = 0;
	virtual void assign_event_source(EventSource *) = 0;
	virtual void revoke(const EventListener *const) = 0;

	virtual int totalEvent() const = 0;

	virtual void validate_and_raise_error() = 0;

	virtual ~Dispatcher() = default;

protected:
	EventSource *m_eventSource = nullptr;
};
