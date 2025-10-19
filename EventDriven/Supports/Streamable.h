#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>

bool likely_pollable(int fd) {
	static std::unordered_map<int, bool> sPollsMap = {
		{S_IFREG, false},
		{S_IFSOCK, true},
		{S_IFIFO, true},
		{S_IFCHR, true},
	};
	struct stat st;
	if (fstat(fd, &st) == -1) {
		return false;
	} else {
		if (auto it = sPollsMap.find(st.st_mode & S_IFMT);
			it != sPollsMap.cend()) {
			return it->second;
		}
		return false;
	}
}