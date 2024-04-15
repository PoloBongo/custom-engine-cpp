#pragma once
#include <cstdint>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			struct ChannelHeader
			{
				static constexpr uint8_t Size = sizeof(uint32_t) + sizeof(uint16_t);

				uint32_t channelIndex;
				uint16_t datasize;
			};
		}
	}
}