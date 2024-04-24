#pragma once

#include "../Settings.h"

#include <optional>
#include <vector>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			namespace Protocols
			{
				class IProtocol
				{
				public:
					IProtocol(uint8_t channelId) : mChannelId(channelId) {}
					virtual ~IProtocol() = default;

					uint8_t channelId() const { return mChannelId; }

					virtual void queue(std::vector<uint8_t>&& msgData) = 0;
					virtual uint16_t serialize(uint8_t* buffer, uint16_t buffersize, Datagram::ID datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
											, bool connectionInterrupted
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					) = 0;

					virtual void onDatagramAcked(Datagram::ID /*datagramId*/) {}
					virtual void onDatagramLost(Datagram::ID /*datagramId*/) {}

					virtual void onDataReceived(const uint8_t* data, uint16_t datasize) = 0;
					virtual std::vector<std::vector<uint8_t>> process() = 0;

					virtual bool isReliable() const = 0;

				private:
					uint8_t mChannelId;
				};
			}
		}
	}
}