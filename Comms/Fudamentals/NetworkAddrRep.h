#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

class NetworkSocket;

class NetworkAddrRep {
	friend class NetworkAddrRepBuilder;
	friend class NetworkSocket;

public:
	inline int getFamily() const { return m_addr.sin_family; }
	inline int getPort() const { return ntohs(m_addr.sin_port); }
	inline int getAddress() const { return m_addr.sin_addr.s_addr; }

private:
	NetworkAddrRep() = default;

private:
	sockaddr_in m_addr;
};

struct NetworkAddrRepBuilder {
	NetworkAddrRepBuilder() = default;

	NetworkAddrRepBuilder &setFamily(int family);
	NetworkAddrRepBuilder &setAddress(int address);
	NetworkAddrRepBuilder &setPort(uint16_t port);

	NetworkAddrRep build() {
		NetworkAddrRep instance;
		instance.m_addr.sin_family = m_family;
		instance.m_addr.sin_port = htons(m_port);
		return instance;
	}

protected:
	int m_family;
	int m_address;
	uint16_t m_port;
};