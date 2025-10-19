#pragma once

#include "../SystemType/BaseEvents.h"

struct EventBroker {
	virtual void submitEvent(const IEvent *) = 0;
};