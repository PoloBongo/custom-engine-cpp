#pragma once

#include "../Types.h"

#include <chrono>

#define UNUSED(x) (void)(x)

namespace Bousk
{
	namespace Utils
	{
		inline std::chrono::milliseconds Now();

		inline bool IsSequenceNewer(uint16_t sNew, uint16_t sLast);
		inline uint16_t SequenceDiff(uint16_t sNew, uint16_t sLast);

		inline void SetBit(uint64_t& bitfield, uint8_t n);
		inline void UnsetBit(uint64_t& bitfield, uint8_t n);
		inline bool HasBit(uint64_t bitfield, uint8_t n);

		using Bousk::Bit;

		uint8_t CountNeededBits(uint64_t v);

		uint8_t CreateRightBitsMask(uint8_t rightBits);
		uint8_t CreateBitsMask(uint8_t nbBits, uint8_t rightBitsToSkip);
	}
}

#include "Utils.inl"
