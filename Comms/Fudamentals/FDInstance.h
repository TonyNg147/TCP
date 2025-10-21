#pragma once
#include <unistd.h>

struct FDInstance {
	FDInstance(int fd) : fd{fd} {}
	FDInstance() = default;
	int fd;
	virtual ~FDInstance() { close(fd); }
};