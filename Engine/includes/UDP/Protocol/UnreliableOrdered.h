#pragma once

#include "../Packet.h" 
#include "../Datagram.h"
#include "ProtocolInterface.h"

class Multiplexer_Test;
class Demultiplexer_Test;
namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			namespace Protocols
			{
				class UnreliableOrdered : IProtocol
				{
					friend class Multiplexer_Test;
					friend class Demultiplexer_Test;

				public:
					using IProtocol::IProtocol;
					~UnreliableOrdered() override = default;

					void queue(std::vector<uint8_t>&& msgData) override { mMultiplexer.queue(std::move(msgData)); }
					uint16_t serialize(uint8_t* buffer, uint16_t buffersize, Datagram::ID datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
											, bool /*connectionInterrupted*/
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					) override {
						return mMultiplexer.serialize(buffer, buffersize, datagramId);
					}

					void onDataReceived(const uint8_t* data, uint16_t datasize) override { mDemultiplexer.onDataReceived(data, datasize); }
					std::vector<std::vector<uint8_t>> process() override { return mDemultiplexer.process(); }

					virtual bool isReliable() const { return false; }

				private:
					class Multiplexer
					{
						friend class Multiplexer_Test;
					public:
						Multiplexer() = default;
						~Multiplexer() = default;

						void queue(std::vector<uint8_t>&& msgData);
						uint16_t serialize(uint8_t* buffer, uint16_t buffersize, Datagram::ID);

					private:
						std::vector<Packet> mQueue;
						Packet::Id mNextId{ 0 };
					} mMultiplexer;
					class Demultiplexer
					{
						friend class Demultiplexer_Test;
					public:
						Demultiplexer() = default;
						~Demultiplexer() = default;

						void onDataReceived(const uint8_t* data, uint16_t datasize);
						std::vector<std::vector<uint8_t>> process();

					private:
						void onPacketReceived(const Packet* pckt);

					private:
						std::vector<Packet> mPendingQueue;
						Packet::Id mLastProcessed{ std::numeric_limits<Packet::Id>::max() };
					} mDemultiplexer;
				};
			}
		}
	}
}

