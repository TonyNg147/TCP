#include "FDEventSource.h"
#include <Dispatcher/FDDispatcher.h>
#include <SystemType/FDEvents.h>
#include <algorithm>
#include <exception>
#include <string.h>

struct DesignatedMonitorInstance {
public:
	DesignatedMonitorInstance(FDEvent *event, int source_epoll_fd)
		: event{*event}, source_epoll_fd{source_epoll_fd} {
		// Validation needed
		if (event->event_kind == -1) {
			throw std::runtime_error(
				"You must specify the interested kind of event for FD");
		}

		epoll_event interested_fd;
		interested_fd.events = event->event_kind;
		interested_fd.data.ptr = this;

		// What if the given event->fd refers to an unvalid FD???

		if (epoll_ctl(source_epoll_fd, EPOLL_CTL_ADD, event->fd,
					  &interested_fd) == -1) {
			printf("Cann ot add into queue %d\n", source_epoll_fd);
			exit(EXIT_FAILURE);
		}
	}
	DesignatedMonitorInstance(const DesignatedMonitorInstance &o) {
		event = o.event;
		source_epoll_fd = o.source_epoll_fd;
	}
	~DesignatedMonitorInstance() {
		if (epoll_ctl(source_epoll_fd, EPOLL_CTL_DEL, event.fd, nullptr) ==
				-1 &&
			errno == EBADF) {
			// Stated in the epoll docs when the file descriptors closed
			// It also be removed from the interested list in Epoll
			// So an error EBADF will be returned.
		}
	}

	int getId() const { return event.fd; }

private:
	FDEvent event;
	int source_epoll_fd = -1;
};

FDEventSource::FDEventSource()
	: m_w{std::bind(&FDEventSource::eventLoop, this)} {
	m_ready_fds.resize(20);

	if (m_epoll_fd = epoll_create1(0); m_epoll_fd == -1) {
		perror("Epoll instance created failed");
		exit(EXIT_FAILURE);
	}
}

FDEventSource::~FDEventSource() {
	m_isReady = true; // The purpose under the hood is to stop to working thread
					  // from looping. Should name it the different sematic
	m_isFinished = true;
	m_w.join();
}
void FDEventSource::startDetection() {
	{
		std::unique_lock<std::mutex> lock(m_lock);
		m_isReady = true;
	}
	m_cvWorkerLoop.notify_one();
}

void FDEventSource::stopDetection() { m_isReady = false; }

bool FDEventSource::isOnDuty() const { return m_isReady; }

void FDEventSource::register_dispatcher(EventBroker *broker) {
	// Should we need verification at this step???
	m_broker = broker;
}

void FDEventSource::subscribeEvent(const IEvent *ev) {
	// Must be convertible to
	if (auto event = dynamic_cast<FDEvent *>((IEvent *)ev); event != nullptr) {
		// At this point, we must maintain the internal queue. So at the time
		// the clients don't have any interests in the event anymore, we can be
		// able to clean the event
		m_interesteds.emplace_back(event, m_epoll_fd);
	}
}

bool FDEventSource::unsubscribeEvent(const IEvent *ev) {
	if (auto event = dynamic_cast<FDEvent *>((IEvent *)ev); event != nullptr) {
		auto it = std::remove_if(
			m_interesteds.begin(), m_interesteds.end(),
			[this, event](const DesignatedMonitorInstance &instance) {
				return instance.getId() == event->fd;
			});
		m_interesteds.erase(it, m_interesteds.cend());
		return true;
	}
	return false;
}

void FDEventSource::eventLoop() {
	std::unique_lock<std::mutex> lock(m_lock);

	do {
		m_cvWorkerLoop.wait(lock, [this]() -> bool { return m_isReady; });

		while (!m_isFinished && m_isReady) {
			int readyN = epoll_wait(m_epoll_fd, m_ready_fds.data(), 20, 1000);
			for (int i = 0; i < readyN; ++i) {
				FDEvent *event = (FDEvent *)m_ready_fds[i].data.ptr;
				event->event_kind = m_ready_fds[i].events;
				m_broker->submitEvent(event);
			}
		}
	} while (!m_isReady);
}