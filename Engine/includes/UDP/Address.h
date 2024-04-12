#pragma once

#include <string>
#include <winSock2.h>

namespace Bousk
{
	namespace Network
	{
		class Address
		{
		public:
			enum class Type {
				None,
				IPv4,
				IPv6,
			};

		public:
			Address() = default;
			Address(const Address&);
			Address(Address&&);
			Address& operator=(const Address&);
			Address& operator=(Address&&);
			~Address() = default;

			Address(const std::string& ip, uint16_t port);
			Address(const sockaddr_storage& addr);

			Type type() const { return mType; }
			std::string address() const;
			uint16_t port() const { return mPort; }
			std::string toString() const;

		private:
			sockaddr_storage mStorage{ 0 };
			uint16_t mPort{ 0 };
			Type mType{ Type::None };
		};
	}
}



