#pragma once

#include "Types.h"
#include "RangedInteger.h"

namespace Bousk
{
	template<class FLOATTYPE, int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	class Float : public Serialization::Serializable
	{
		static_assert(std::is_same_v<FLOATTYPE, float32> || std::is_same_v<FLOATTYPE, float64>, "Float peut seulement être utilisé avec float32 ou float64");
		static_assert(NBDECIMALS > 0, "Au moins 1 décimale est nécessaire");
		static_assert(NBDECIMALS < 10, "Maximum de 10 décimales");
		static_assert(STEP != 0, "Le pas ne peut être nul");
		static_assert(STEP % 10 != 0, "Le pas ne peut être un multiple de 10. Retirez une décimale");
		using FloatType = FLOATTYPE;
		static constexpr int32 Min = MIN;
		static constexpr int32 Max = MAX;
		static constexpr uint32 Diff = Max - Min;
		static constexpr uint8 NbDecimals = NBDECIMALS;
		static constexpr uint32 Multiple = Pow<10, NbDecimals>::Value;
		static constexpr uint8 Step = STEP;
		static constexpr uint32 Domain = (MAX - MIN) * Multiple / STEP;

	public:
		Float() = default;
		Float(FloatType value)
		{
			mQuantizedValue = Quantize(value);
		}

		static uint32 Quantize(FloatType value)
		{
			assert(value >= Min && value <= Max);
			return static_cast<uint32>(((value - Min) * Multiple) / Step);
		}

		inline FloatType get() const { return static_cast<FloatType>((mQuantizedValue.get() * Step * 1.) / Multiple + Min); }
		inline operator FloatType() const { return get(); }

		bool write(Serialization::Serializer& serializer) const override { return mQuantizedValue.write(serializer); }
		bool read(Serialization::Deserializer& deserializer) override { return mQuantizedValue.read(deserializer); }

	private:
		RangedInteger<0, Domain> mQuantizedValue;
	};

	template<int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	using Float32 = Float<float32, MIN, MAX, NBDECIMALS, STEP>;
	template<int32 MIN, int32 MAX, uint8 NBDECIMALS, uint8 STEP = 1>
	using Float64 = Float<float64, MIN, MAX, NBDECIMALS, STEP>;
}