#pragma once

#include "BaseEvents.h"
#include <optional>
#include <unordered_map>

struct FDEvent : IEvent {
	FDEvent() = default;
	FDEvent(int fd, int eventKind) : fd{fd}, event_kind{eventKind} {}
	FDEvent(const FDEvent &o) : fd{o.fd}, event_kind{o.event_kind} {}
	FDEvent &operator=(const FDEvent &o) {
		fd = o.fd;
		event_kind = o.event_kind;
		return *this;
	}
	bool operator==(const FDEvent &o) const { return fd == o.fd; }
	int fd;
	int event_kind = 0;
};

template <> struct std::hash<FDEvent> {
	std::size_t operator()(const FDEvent &s) const noexcept {
		return std::hash<int>{}(s.fd);
	}
};
