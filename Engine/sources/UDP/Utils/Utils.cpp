#include "UDP/Utils/Utils.h"

namespace Bousk
{
	namespace Utils
	{
		uint8_t CountNeededBits(uint64_t v)
		{
			assert(v != 0);
			uint8_t bits = 0;
			while (v)
			{
				++bits;
				v /= 2;
			}
			return bits;
		}

		uint8_t CreateRightBitsMask(uint8_t rightBits)
		{
			assert(rightBits >= 1 && rightBits <= 8);
			switch (rightBits)
			{
			case 1: return 0b00000001;
			case 2: return 0b00000011;
			case 3: return 0b00000111;
			case 4: return 0b00001111;
			case 5: return 0b00011111;
			case 6: return 0b00111111;
			case 7: return 0b01111111;
			case 8: return 0b11111111;
			}
			return 0;
		}
		uint8_t CreateBitsMask(uint8_t nbBits, uint8_t rightBitsToSkip)
		{
			assert(rightBitsToSkip < 8);
			assert(rightBitsToSkip + nbBits <= 8);
			return CreateRightBitsMask(nbBits) << rightBitsToSkip;
		}
	}
}