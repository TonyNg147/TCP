#pragma once
#include <Dispatcher/Dispatcher.h>
#include <SystemType/BaseEvents.h>
class EventSource {
public:
	virtual void register_dispatcher(EventBroker *) = 0;
	virtual void subscribeEvent(const IEvent *ev) = 0;
	virtual bool unsubscribeEvent(const IEvent *ev) = 0;
	virtual void startDetection() = 0;
	virtual void stopDetection() = 0;
	virtual bool isOnDuty() const = 0;
	virtual ~EventSource() = default;
};