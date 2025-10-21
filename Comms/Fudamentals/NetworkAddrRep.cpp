#include "NetworkAddrRep.h"

NetworkAddrRepBuilder &NetworkAddrRepBuilder::setFamily(int family) {
	m_family = family;
	return *this;
}
NetworkAddrRepBuilder &NetworkAddrRepBuilder::setAddress(int address) {
	m_address = address;
	return *this;
}
NetworkAddrRepBuilder &NetworkAddrRepBuilder::setPort(uint16_t port) {
	m_port = port;
	return *this;
}