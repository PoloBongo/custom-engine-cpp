#pragma once

#include "Datagram.h"

#include <cstdint>
#include <vector>

namespace Bousk
{
	namespace Network
	{
		namespace UDP
		{
			struct Packet
			{
				using Id = uint16_t;
				enum class Type : uint16_t
				{
					Packet,
					FirstFragment,
					Fragment,
					LastFragment,
				};

				struct Header
				{
					Id id;
					uint16_t size{ 0 };
					Type type;
				};

				static constexpr uint16_t PacketMaxSize = Bousk::UDP::Datagram::DataMaxSize;
				static constexpr uint16_t HeaderSize = sizeof(Header);
				static constexpr uint16_t DataMaxSize = PacketMaxSize - HeaderSize;
				static constexpr size_t MaxPacketsPerMessage = 32;
				static constexpr size_t MaxMessageSize = MaxPacketsPerMessage * DataMaxSize;

				Header header;
				std::array<uint8_t, DataMaxSize> dataBuffer;

				inline Id id() const { return header.id; }
				inline Type type() const { return header.type; }
				inline uint8_t* data() { return dataBuffer.data(); }
				inline const uint8_t* data() const { return dataBuffer.data(); }
				inline uint16_t datasize() const { return header.size; }
				//!< Full buffer : header + data
				inline const uint8_t* buffer() const { return reinterpret_cast<const uint8_t*>(this); }
				//!< Packet full size : header + data
				inline uint16_t size() const { return HeaderSize + header.size; }
			};

			class Multiplexer
			{
			public:
				Multiplexer() = default;
				~Multiplexer() = default;

				void queue(std::vector<uint8_t>&& messageData);
				size_t serialize(uint8_t* buffer, const size_t buffersize);
			//private:
				std::vector<Packet> mQueue;
				Packet::Id mNextId{ 0 };
			};

			class Demultiplexer
			{
			public:
				Demultiplexer() = default;
				~Demultiplexer() = default;

				void onDataReceived(const uint8_t* data, const size_t datasize);
				std::vector<std::vector<uint8_t>> process();
			//private:
				void onPacketReceived(const Packet* pckt);

				static constexpr size_t QueueSize = 2 * Packet::MaxPacketsPerMessage;
				std::vector<Packet> mPendingQueue;
				Packet::Id mLastProcessed{ std::numeric_limits<Packet::Id>::max() };
			};
		}
	}
}

