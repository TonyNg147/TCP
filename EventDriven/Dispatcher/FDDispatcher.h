#pragma once
#include "Dispatcher.h"
#include <SystemType/FDEvents.h>

#include <unordered_map>
#include <vector>

// We know that this dispatcher is dedicated to process FD events
class FDDisPatcher : public Dispatcher {
public:
	FDDisPatcher();
	bool register_event_listener(const IEvent &,
								 const EventListener *const) final;
	void assign_event_source(EventSource *) final;
	void revoke(const EventListener *const) final;
	void submitEvent(const IEvent *) final;

	virtual int totalEvent() const final;

	void validate_and_raise_error() final;

	~FDDisPatcher() override;

private:
	// Take a weak ref to EventListeners
	// No need to take the ownership of them.
	std::unordered_map<FDEvent, std::vector<EventListener *>> m_listeners;
};