#pragma once

#include <cstdint>
#include <array>

namespace Bousk
{
	namespace UDP
	{
		struct Datagram
		{
			using ID = uint16_t;
			struct Header
			{
				ID id;
				ID ack;
				uint16_t previousAcks;
			};
			static constexpr size_t BufferMaxSize = 1400;
			static constexpr size_t DataMaxSize = BufferMaxSize - sizeof(Header);
			static constexpr uint16_t HeaderSize = sizeof(Header);

			Header header;
			std::array<uint8_t, DataMaxSize> data;

			// permet de contenir la taille effective des données du datagram
			// utilisation local uniquement
			uint16_t datasize{ 0 };
		}; 
	}
}