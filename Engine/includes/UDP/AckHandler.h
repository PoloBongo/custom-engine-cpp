#pragma once

#include "UDP/Utils/Utils.h"
#include "UDP/Utils/Utils.inl"

#include <cstdint>
#include <vector>

namespace Bousk
{
	namespace UDP
	{
		class AckHandler
		{
		public:
			AckHandler() = default;
			AckHandler(const AckHandler&) = default;
			AckHandler& operator=(const AckHandler&) = default;
			AckHandler(AckHandler&&) = default;
			AckHandler& operator=(AckHandler&&) = default;
			~AckHandler() = default;

			void update(uint16 newAck, uint64 previousAcks, bool trackLoss = false);
			bool isAcked(uint16 ack) const;
			bool isNewlyAcked(uint16 ack) const;

			uint16 lastAck() const;
			uint64 previousAcksMask() const;
			std::vector<uint16> getNewAcks() const;
			std::vector<uint16>&& loss();

		private:
			uint16 mLastAck = -1;
			uint64 mPreviousAcks = -1;
			uint64 mNewAcks = 0;
			std::vector<uint16> mLoss;
			bool mLastAckIsNew { false };
		};
	}
}