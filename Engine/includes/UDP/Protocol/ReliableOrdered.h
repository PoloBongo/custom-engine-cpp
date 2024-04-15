#pragma once

#include "../Packet.h" 
#include "../Datagram.h"
#include "ProtocolInterface.h"

#include <vector>
#include <set>

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
				class ReliableOrdered : public IProtocol
				{
					friend class Multiplexer_Test;
					friend class Demultiplexer_Test;
				public:
					using IProtocol::IProtocol;
					~ReliableOrdered() override = default;

					void queue(std::vector<uint8_t>&& msgData) override { mMultiplexer.queue(std::move(msgData)); }
					uint16_t serialize(uint8_t* buffer, const uint16_t buffersize, const Datagram::ID datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
											, const bool connectionInterrupted
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
										) override {
											return mMultiplexer.serialize(buffer, buffersize, datagramId
					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
												, connectionInterrupted
					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
					);}

					void onDatagramAcked(const Datagram::ID datagramId) override { mMultiplexer.onDatagramAcked(datagramId); }
					void onDatagramLost(const Datagram::ID datagramId) override { mMultiplexer.onDatagramLost(datagramId); }

					void onDataReceived(const uint8_t* data, const uint16_t datasize) override { mDemultiplexer.onDataReceived(data, datasize); }
					std::vector<std::vector<uint8_t>> process() override { return mDemultiplexer.process(); }

					virtual bool isReliable() const { return true; }
					
				private:
					class Multiplexer
					{
						friend class Multiplexer_Test;
					public:
						Multiplexer() = default;
						~Multiplexer() = default;

						void queue(std::vector<uint8_t>&& msgData);
						uint16_t serialize(uint8_t* buffer, const uint16_t buffersize, const Datagram::ID datagramId
						#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
													, bool connectionInterrupted
						#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
						);

						void onDatagramAcked(const Datagram::ID datagramId);
						void onDatagramLost(const Datagram::ID datagramId);

					private:
						class ReliablePacket
						{
						public:
							Packet& packet() { return mPacket; }
							const Packet& packet() const { return mPacket; }

							bool shouldSend() const { return mShouldSend; }
							void resend() { mShouldSend = true; }
							void onSent(const Datagram::ID datagramId) { mDatagramsIncluding.insert(datagramId); mShouldSend = false; }
							bool isIncludedIn(const Datagram::ID datagramId) const { return mDatagramsIncluding.find(datagramId) != mDatagramsIncluding.cend(); }

						private:
							Packet mPacket;
							std::set<Datagram::ID> mDatagramsIncluding;
							bool mShouldSend{ true };
						};
						std::vector<ReliablePacket> mQueue;
						Packet::Id mNextId{ 0 };
						Packet::Id mFirstAllowedPacket{ 0 };
					} mMultiplexer;
					class Demultiplexer
					{
						friend class Demultiplexer_Test;
					public:
						static constexpr size_t QueueSize = 2 * Packet::MaxPacketsPerMessage;
					public:
						Demultiplexer() = default;
						~Demultiplexer() = default;

						void onDataReceived(const uint8_t* data, const uint16_t datasize);
						std::vector<std::vector<uint8_t>> process();

					private:
						void onPacketReceived(const Packet* pckt);

					private:
						std::array<Packet, QueueSize> mPendingQueue;
						Packet::Id mLastProcessed{ std::numeric_limits<Packet::Id>::max() };
					} mDemultiplexer;
				};
			}
		}
	}
}

