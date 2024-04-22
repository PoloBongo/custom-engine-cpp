#pragma once

//#include <cstdint>
#include <array>

#include "UDP/Types.h"

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			struct Datagram
			{
				using ID = uint16_t;
				enum class Type : uint8_t {
					ConnectedData,
					KeepAlive,
					Disconnection,
				};
				struct Header
				{
					ID id;
					ID ack;
					uint16_t previousAcks;
					Type type;
				};
				static constexpr uint16_t BufferMaxSize = 1400;
				static constexpr uint16_t HeaderSize = sizeof(Header);
				static constexpr uint16_t DataMaxSize = BufferMaxSize - HeaderSize;

				Header header;
				std::array<uint8_t, DataMaxSize> data;

				// permet de contenir la taille effective des données du datagram
				// utilisation local uniquement
				uint16_t datasize{ 0 };
				uint16_t size() const { return HeaderSize + datasize; }
			};
		}
	}
}