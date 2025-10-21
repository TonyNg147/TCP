#pragma once
#include <optional>
#include <stdexcept>

#include "FDInstance.h"
#include "NetworkAddrRep.h"

#include <sys/socket.h>

struct SocketAttrs {
	int family;
	int type;
	int protocol = 0;
};

struct NetworkSocketBuilder;

class ActiveSocket : public FDInstance {
public:
	ActiveSocket(SocketAttrs attr);
	ActiveSocket(ActiveSocket &&o);
	ActiveSocket &operator=(ActiveSocket &&o);
	~ActiveSocket() = default;

private:
	SocketAttrs m_attr;
	friend class NetworkSocketBuilder;
};

struct NetworkSocketBuilder;

class NetworkSocket : ActiveSocket {
private:
	NetworkSocket(ActiveSocket &&socket, NetworkAddrRep &&networkAddr);
	NetworkSocket() = delete;

private:
	bool _bind();

private:
	friend class NetworkSocketBuilder;
	NetworkAddrRep m_networkAddr;
	std::optional<std::string> m_lastErr;
};

class NetworkSocketBuilder {
public:
	NetworkSocketBuilder() = default;
	NetworkSocketBuilder &setActiveSocket(ActiveSocket &&socket);
	NetworkSocketBuilder &setNetworkAddr(NetworkAddrRep &&networkAddr);
	bool still_valid(const std::optional<ActiveSocket> &_socket,
					 const std::optional<NetworkAddrRep> &_addr) const;
	std::optional<NetworkSocket> build();

private:
	std::optional<ActiveSocket> m_socket;
	std::optional<NetworkAddrRep> m_networkAddr;
};
