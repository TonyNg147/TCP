#include "Socket.h"

#include <string.h>
ActiveSocket::ActiveSocket(SocketAttrs attr) {
	fd = socket(attr.family, attr.type, attr.protocol);
	if (fd == -1) {
		throw std::runtime_error("Cannot create socket");
	}
}

ActiveSocket::ActiveSocket(ActiveSocket &&o)
	: FDInstance{o.fd}, m_attr{o.m_attr} {
	fd = o.fd;
}

ActiveSocket &ActiveSocket::operator=(ActiveSocket &&o) {
	// Since this is the assignment, we're able to have the chance to refer to
	// another FD What if we've already pointed to a valid one. In that case, we
	// need to close it first.
	if (fd != -1 && fd != o.fd) {
		close(fd);
	}
	fd = o.fd;
	m_attr = o.m_attr;
	return *this;
}

NetworkSocket::NetworkSocket(ActiveSocket &&socket,
							 NetworkAddrRep &&networkAddr)
	: ActiveSocket{std::move(socket)}, m_networkAddr{std::move(networkAddr)} {
	// Validation here. May raise an exception or sth else
	(void)_bind();
}

NetworkSocketBuilder &
NetworkSocketBuilder::setActiveSocket(ActiveSocket &&socket) {
	std::optional<ActiveSocket> attentive_socket(std::move(socket));
	if (still_valid(attentive_socket, m_networkAddr)) {
		m_socket = std::move(attentive_socket);
	}
	return *this;
}

NetworkSocketBuilder &
NetworkSocketBuilder::setNetworkAddr(NetworkAddrRep &&networkAddr) {
	std::optional<NetworkAddrRep> attentive_networkAddr(std::move(networkAddr));
	if (still_valid(m_socket, attentive_networkAddr)) {
		m_networkAddr = std::move(attentive_networkAddr);
	}
	return *this;
}

std::optional<NetworkSocket> NetworkSocketBuilder::build() {
	if (m_socket && m_networkAddr) {

		return NetworkSocket(std::move(*m_socket), std::move(*m_networkAddr));
	}
	return std::nullopt;
}

bool NetworkSocketBuilder::still_valid(
	const std::optional<ActiveSocket> &_socket,
	const std::optional<NetworkAddrRep> &_addr) const {
	if (_socket && _addr) {
		return _socket->m_attr.family == _addr->getFamily();
	}
	return true;
}

bool NetworkSocket::_bind() {
	if (bind(fd, (struct sockaddr *)&m_networkAddr.m_addr,
			 sizeof(m_networkAddr.m_addr)) == -1) {
		m_lastErr.emplace(strerror(errno));
		return true;
	}
	return false;
}