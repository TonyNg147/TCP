#pragma once

struct EventID {};

// The Event instance will be passed to Dispatcher
// and kept track by EventSource.
// EventSource knows what's need to be filled for the specific
// Event
// Which means IEvent has to identify the characteristic
// Of the event it wants to receive information when Source produces
// That means ID from IEvent needed in case the Event Type woudln't suffice
struct IEvent {
	EventID id;
	virtual ~IEvent() = default;
};
