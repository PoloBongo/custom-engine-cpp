#pragma once

#include "Settings.h"
#include "Datagram.h"

#include <vector>
#include <memory>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			namespace Protocols
			{
				class IProtocol;
			}

			class ChannelsHandler
			{
			public:
				ChannelsHandler();
				~ChannelsHandler();

				template<class T>
				void registerChannel(uint8_t channelId);

				// Multiplexer
				void queue(std::vector<uint8_t>&& msgData, uint32_t channelIndex);
				uint16_t serialize(uint8_t* buffer, uint16_t buffersize, Datagram::ID datagramId
				#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
									, bool connectionInterrupted
				#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
				);

				void onDatagramAcked(Datagram::ID datagramId);
				void onDatagramLost(Datagram::ID datagramId);

				// Demultiplexer
				void onDataReceived(const uint8_t* data, uint16_t datasize);
				std::vector<std::tuple<uint8_t, std::vector<uint8_t>>> process(bool isConnected);

			private:
				std::vector<std::unique_ptr<Protocols::IProtocol>> mChannels;
			};
		}
	}
}
