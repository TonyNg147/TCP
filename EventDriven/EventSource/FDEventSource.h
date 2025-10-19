#pragma once

#include "EventSourceBase.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct DesignatedMonitorInstance;

class FDEventSource : public EventSource {
public:
	FDEventSource();

	~FDEventSource();

	void register_dispatcher(EventBroker *broker) final;

	void startDetection() final;
	void stopDetection() final;
	bool isOnDuty() const final;

	void subscribeEvent(const IEvent *ev) final;

	bool unsubscribeEvent(const IEvent *ev) final;

	void eventLoop();

private:
	// Should be weak ref - no needed for EventSource to keep track of the life
	// of Dispatcher
	EventBroker *m_broker = nullptr;
	int m_epoll_fd;
	std::thread m_w;
	std::mutex m_lock;
	std::condition_variable m_cvWorkerLoop;
	bool m_isFinished = false;
	bool m_isReady = false;
	std::vector<epoll_event> m_ready_fds;
	std::vector<DesignatedMonitorInstance> m_interesteds;
};
