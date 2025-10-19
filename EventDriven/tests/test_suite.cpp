// #pragma once

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <Collections/DispatcherCollection.h>
#include <Collections/EventSourceCollection.h>
#include <EventListener/BaseEventListener.h>
#include <FDEvents.h>
#include <doctest/doctest.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>

struct WrapFD {
	int fd;
	~WrapFD() { close(fd); }
};

/**
 * @brief This dedicated struct is for testing whether
 * 		our EventPool works as expected when we apply modification into it.
 *
 */
struct CustomHandlerStruct : EventListener {

	void onEvent(const IEvent *) final { m_hasEventInvoked = true; }
	bool m_hasEventInvoked = false;
};

TEST_CASE("Verify the listen count") {
	DispatcherCollection &dispatcherInstance =
		DispatcherCollection::getInstance();

	auto FDDispatcher = dispatcherInstance.getSource(DispatcherKind::FD);

	auto FDSource =
		EventSourceCollection::getInstance().getSource(EvenSourceKind::FD);

	FDDispatcher->assign_event_source(FDSource);

	CustomHandlerStruct handler;
	FDDispatcher->register_event_listener(FDEvent{STDIN_FILENO, EPOLLIN},
										  &handler);

	REQUIRE(FDDispatcher->totalEvent() == 1);

	FDDispatcher->revoke(&handler);

	REQUIRE(FDDispatcher->totalEvent() == 0);
}

TEST_CASE("Ensure the basic operations of FDEventSource work fine") {
	auto FDSource =
		EventSourceCollection::getInstance().getSource(EvenSourceKind::FD);

	REQUIRE_MESSAGE(FDSource != nullptr,
					"Ensure that the instance of FDEventSource exits");

	REQUIRE_MESSAGE(!FDSource->isOnDuty(),
					"At the time of creation, FDSource shouldn't perform any "
					"actions or activities");

	FDSource->startDetection();

	REQUIRE_MESSAGE(
		FDSource->isOnDuty(),
		"After startDetection, we put FDEventSource into working mode ");

	FDSource->stopDetection();

	REQUIRE_MESSAGE(
		!FDSource->isOnDuty(),
		"After stopDetection, FDSourceEvent no longer raise any activities ");
}

TEST_CASE("Ensure that we received the event when it's about to be happened") {
	DispatcherCollection &dispatcherInstance =
		DispatcherCollection::getInstance();

	auto FDSource =
		EventSourceCollection::getInstance().getSource(EvenSourceKind::FD);

	auto FDDispatcher = dispatcherInstance.getSource(DispatcherKind::FD);

	int sv[2];
	socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sv);

	WrapFD wraps[2] = {WrapFD{sv[0]}, WrapFD{sv[1]}};

	CustomHandlerStruct handler;
	REQUIRE_MESSAGE(
		!handler.m_hasEventInvoked,
		"At the beginning, the Handler appears to have no event received");

	FDDispatcher->assign_event_source(FDSource);
	FDDispatcher->register_event_listener(FDEvent{wraps[0].fd, EPOLLIN},
										  &handler);

	FDSource->register_dispatcher(FDDispatcher);

	FDSource->startDetection();

	char buf[] = "Hello world";

	int nBytesWrite = write(wraps[1].fd, buf, sizeof(buf));

	CHECK(nBytesWrite != 0);

	usleep(2000);

	REQUIRE_MESSAGE(handler.m_hasEventInvoked,
					"Since we write into the other end of socket, now at the "
					"listened FD of "
					"that socket, it should receive the EPOLLIN event");
}