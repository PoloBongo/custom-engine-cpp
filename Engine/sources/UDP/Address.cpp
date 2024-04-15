#include "UDP/Address.h"
#include "UDP/Serialization/Convert.h"

#include <WS2tcpip.h>

namespace Bousk
{
	namespace Network
	{
		Address::Address(const std::string& ip, uint16_t port)
		{
		}

		Address::Address(const sockaddr_storage& addr)
		{
		}

		Address Address::Any(Type type, uint16 port)
		{
			switch (type)
			{
			case Type::IPv4:
			{
				sockaddr_storage storage{ 0 };
				sockaddr_in& addr = reinterpret_cast<sockaddr_in&>(storage);
				addr.sin_addr.s_addr = INADDR_ANY;
				addr.sin_port = htons(port);
				addr.sin_family = AF_INET;
				return Address(storage);
			}
			case Type::IPv6:
			{
				sockaddr_storage storage{ 0 };
				sockaddr_in6& addr = reinterpret_cast<sockaddr_in6&>(storage);
				addr.sin6_addr = in6addr_any;
				addr.sin6_port = htons(port);
				addr.sin6_family = AF_INET6;
				return Address(reinterpret_cast<sockaddr_storage&>(addr));
			}
			default:
				assert(false);
				return Address();
			}
		}
		Address Address::Loopback(Type type, uint16 port)
		{
			switch (type)
			{
			case Type::IPv4:
			{
				sockaddr_storage storage{ 0 };
				sockaddr_in& addr = reinterpret_cast<sockaddr_in&>(storage);
				addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
				addr.sin_port = htons(port);
				addr.sin_family = AF_INET;
				return Address(storage);
			}
			case Type::IPv6:
			{
				// TODO
			}
			default:
				assert(false);
				return Address();
			}
		}

		void Address::set(const sockaddr_storage& src)
		{
			memcpy(&mStorage, &src, sizeof(mStorage));
			if (mStorage.ss_family == AF_INET)
			{
				mType = Type::IPv4;
				const sockaddr_in& addrin = reinterpret_cast<const sockaddr_in&>(mStorage);
				Serialization::Conversion::ToLocal(addrin.sin_port, mPort);
			}
			else if (mStorage.ss_family == AF_INET6)
			{
				mType = Type::IPv6;
				const sockaddr_in6& addrin = reinterpret_cast<const sockaddr_in6&>(mStorage);
				Serialization::Conversion::ToLocal(addrin.sin6_port, mPort);
			}
			else
				mType = Type::None;
		}

		std::string Address::address() const
		{
			return std::string();
		}

		std::string Address::toString() const
		{
			return std::string();
		}

		bool Address::connect(SOCKET sckt) const
		{
			return ::connect(sckt, reinterpret_cast<const sockaddr*>(&mStorage), sizeof(mStorage)) == 0;
		}

		bool Address::accept(SOCKET sckt, SOCKET& newClient)
		{
			sockaddr_storage storage{ 0 };
			sockaddr_in& addr = reinterpret_cast<sockaddr_in&>(storage);
			socklen_t addrlen = sizeof(addr);
			SOCKET newClientSocket = ::accept(sckt, reinterpret_cast<sockaddr*>(&addr), &addrlen);
			if (newClientSocket == INVALID_SOCKET)
			{
				return false;
			}
			set(storage);
			newClient = newClientSocket;
			return true;
		}

		bool Address::bind(SOCKET sckt) const
		{
			return ::bind(sckt, reinterpret_cast<const sockaddr*>(&mStorage), sizeof(mStorage)) == 0;
		}

		int Address::sendTo(SOCKET sckt, const char* data, size_t datalen) const
		{
			return sendto(sckt, data, static_cast<int>(datalen), 0, reinterpret_cast<const sockaddr*>(&mStorage), sizeof(mStorage));
		}

		int Address::recvFrom(SOCKET sckt, uint8_t* buffer, size_t bufferSize)
		{
			sockaddr_storage storage{ 0 };
			sockaddr_in& from = reinterpret_cast<sockaddr_in&>(storage);
			socklen_t fromlen = sizeof(from);
			int ret = recvfrom(sckt, reinterpret_cast<char*>(buffer), static_cast<int>(bufferSize), 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
			if (ret >= 0)
			{
				set(storage);
			}
			return ret;
		}
	}
}