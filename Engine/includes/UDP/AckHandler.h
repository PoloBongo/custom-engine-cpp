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

			void update(uint16_t newAck, uint64_t previousAcks, bool trackLoss = false);
			bool isAcked(uint16_t ack) const;
			bool isNewlyAcked(uint16_t ack) const;

			uint16_t lastAck() const;
			uint64_t previousAcksMask() const;
			std::vector<uint16_t> getNewAcks() const;
			std::vector<uint16_t>&& loss();

		private:
			uint16_t mLastAck = -1;
			uint64_t mPreviousAcks = -1;
			uint64_t mNewAcks = 0;
			std::vector<uint16_t> mLoss;
			bool mLastAckIsNew { false };
		};
	}
}