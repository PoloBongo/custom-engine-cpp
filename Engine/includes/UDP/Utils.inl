#pragma once

#include "UDP/Utils.h"

namespace Bousk
{
	namespace Utils
	{
		void SetBit(uint64_t& bitfield, const uint8_t n)
		{
			assert(n < 64);
			bitfield |= (Bit<uint64_t>::Right << n);
		}

		void UnsetBit(uint64_t& bitfield, const uint8_t n)
		{
			assert(n < 64);
			bitfield &= (~(Bit<uint64_t>::Right << n));
		}

		bool HasBit(uint64_t bitfield, const uint8_t n)
		{
			assert(n < 64);
			return (bitfield & (Bit<uint64_t>::Right << n)) != 0;
		}
	}
}