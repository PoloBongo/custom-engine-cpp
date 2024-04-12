#include "UDP/Address.h"

Bousk::Network::Address::Address(const std::string& ip, uint16_t port)
{
}

Bousk::Network::Address::Address(const sockaddr_storage& addr)
{
}

std::string Bousk::Network::Address::address() const
{
	return std::string();
}

std::string Bousk::Network::Address::toString() const
{
	return std::string();
}
