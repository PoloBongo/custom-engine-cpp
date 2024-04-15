//#pragma once
//
//#include "../Packet.h" 
//#include "../Datagram.h"
//
//#include <vector>
//#include <set>
//
//namespace Bousk
//{
//	namespace Network
//	{
//		namespace UDP
//		{
//			namespace Protocols
//			{
//				class Multiplexer
//				{
//				public:
//					Multiplexer() = default;
//					~Multiplexer() = default;
//
//					void queue(std::vector<uint8_t>&& msgData);
//					uint16_t serialize(uint8_t* buffer, const uint16_t buffersize, const Bousk::UDP::Datagram::ID datagramId
//					#if BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
//											, bool connectionInterrupted
//					#endif // BOUSKNET_ALLOW_NETWORK_INTERRUPTION == BOUSKNET_SETTINGS_ENABLED
//					);
//
//					void onDatagramAcked(const Bousk::UDP::Datagram::ID datagramId);
//					void onDatagramLost(const Bousk::UDP::Datagram::ID datagramId);
//
//				private:
//					class ReliablePacket
//					{
//					public:
//						Packet& packet() { return mPacket; }
//						const Packet& packet() const { return mPacket; }
//
//						bool shouldSend() const { return mShouldSend; }
//						void resend() { mShouldSend = true; }
//						void onSent(const Bousk::UDP::Datagram::ID datagramId) { mDatagramsIncluding.insert(datagramId); mShouldSend = false; }
//						bool isIncludedIn(const Bousk::UDP::Datagram::ID datagramId) const { return mDatagramsIncluding.find(datagramId) != mDatagramsIncluding.cend(); }
//
//					private:
//						Packet mPacket;
//						std::set<Bousk::UDP::Datagram::ID> mDatagramsIncluding;
//						bool mShouldSend{ true };
//					};
//					std::vector<ReliablePacket> mQueue;
//					Packet::Id mNextId{ 0 };
//					Packet::Id mFirstAllowedPacket{ 0 };
//				} mMultiplexer;
//				class Demultiplexer
//				{
//					friend class ReliableOrdered_Demultiplexer_Test;
//				public:
//					static constexpr size_t QueueSize = 2 * Packet::MaxPacketsPerMessage;
//				public:
//					Demultiplexer() = default;
//					~Demultiplexer() = default;
//
//					void onDataReceived(const uint8_t* data, const uint16_t datasize);
//					std::vector<std::vector<uint8_t>> process();
//
//				private:
//					void onPacketReceived(const Packet* pckt);
//
//				private:
//					std::array<Packet, QueueSize> mPendingQueue;
//					Packet::Id mLastProcessed{ std::numeric_limits<Packet::Id>::max() };
//				} mDemultiplexer;
//			}
//		}
//	}
//}
//
