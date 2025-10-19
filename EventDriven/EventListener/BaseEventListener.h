#pragma once
#include <SystemType/BaseEvents.h>

struct EventListener {
	virtual void onEvent(const IEvent *) = 0;
};