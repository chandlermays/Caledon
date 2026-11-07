#pragma once

#include <concepts>
#include <limits>

namespace CaledonEngine::Math
{
	template<typename T>
	concept Arithmetic		=	std::is_arithmetic_v<T>;

	template<typename T>
	concept FloatingPoint	=	std::floating_point<T>;

	//--------------------------------------------------------------------------
	// Numeric limits (any arithmetic type)
	//--------------------------------------------------------------------------
	template<Arithmetic T> inline constexpr T Lowest_v					=			std::numeric_limits<T>::lowest();										// Lowest finite value
	template<Arithmetic T> inline constexpr T Min_v						=			std::numeric_limits<T>::min();											// Smallest positive normal value (floats) / min value (ints)
	template<Arithmetic T> inline constexpr T Max_v						=			std::numeric_limits<T>::max();											// Largest finite value

	//--------------------------------------------------------------------------
	// Floating-point-only limits
	//--------------------------------------------------------------------------
	template<FloatingPoint T> inline constexpr T Infinity_v				=			std::numeric_limits<T>::infinity();										// Positive infinity
	template<FloatingPoint T> inline constexpr T NaN_v					=			std::numeric_limits<T>::quiet_NaN();									// Quiet NaN (not a number)
	template<FloatingPoint T> inline constexpr T Epsilon_v				=			std::numeric_limits<T>::epsilon();										// Machine epsilon (difference between 1 and next representable value)
	template<FloatingPoint T> inline constexpr T Denorm_v				=			std::numeric_limits<T>::denorm_min();									// Smallest positive subnormal value

	template<FloatingPoint T> inline constexpr T TightTolerance_v		=			static_cast<T>(1e-8L);													// 
	template<FloatingPoint T> inline constexpr T LooseTolerance_v		=			static_cast<T>(1e-4L);													//

	//--------------------------------------------------------------------------
	// Mathematical constants
	//--------------------------------------------------------------------------
	template<FloatingPoint T> inline constexpr T Pi_v					=			static_cast<T>(3.141592653589793238462643383279502884L);				// π
	template<FloatingPoint T> inline constexpr T TwoPi_v				=			T(2) * Pi_v<T>;															// 2π (τ)
	template<FloatingPoint T> inline constexpr T HalfPi_v				=			Pi_v<T> / T(2);															// π/2
	template<FloatingPoint T> inline constexpr T QuarterPi_v			=			Pi_v<T> / T(4);															// π/4
	template<FloatingPoint T> inline constexpr T InvPi_v				=			T(1) / Pi_v<T>;															// 1/π
	template<FloatingPoint T> inline constexpr T InvTwoPi_v				=			T(1) / TwoPi_v<T>;														// 1/(2π)
	template<FloatingPoint T> inline constexpr T E_v					=			static_cast<T>(2.718281828459045235360287471352662498L);				// Euler's number
	template<FloatingPoint T> inline constexpr T Sqrt2_v				=			static_cast<T>(1.414213562373095048801688724209698079L);				// √2
	template<FloatingPoint T> inline constexpr T InvSqrt2_v				=			T(1) / Sqrt2_v<T>;														// 1/√2
	template<FloatingPoint T> inline constexpr T Sqrt3_v				=			static_cast<T>(1.732050807568877293527446341505872367L);				// √3
	template<FloatingPoint T> inline constexpr T InvSqrt3_v				=			T(1) / Sqrt3_v<T>;														// 1/√3
	template<FloatingPoint T> inline constexpr T Ln2_v					=			static_cast<T>(0.693147180559945309417232121458176568L);				// ln(2)
	template<FloatingPoint T> inline constexpr T Ln10_v					=			static_cast<T>(2.302585092994045684017991454684364208L);				// ln(10)
	template<FloatingPoint T> inline constexpr T Log2E_v				=			T(1) / Ln2_v<T>;														// log2(e)
	template<FloatingPoint T> inline constexpr T Log10E_v				=			T(1) / Ln10_v<T>;														// log10(e)
	template<FloatingPoint T> inline constexpr T GoldenRatio_v			=			static_cast<T>(1.618033988749894848204586834365638118L);				// φ

	//--------------------------------------------------------------------------
	// Angle conversion
	//--------------------------------------------------------------------------
	template<FloatingPoint T> inline constexpr T Deg2Rad_v				=			Pi_v<T> / T(180);														// π/180
	template<FloatingPoint T> inline constexpr T Rad2Deg_v				=			T(180) / Pi_v<T>;														// 180/π

	template<FloatingPoint T> constexpr T ToRadians(T degrees)						{ return degrees * Deg2Rad_v<T>; }										// Converts degrees to radians
	template<FloatingPoint T> constexpr T ToDegrees(T radians)						{ return radians * Rad2Deg_v<T>; }										// Converts radians to degrees

	//--------------------------------------------------------------------------
	// Float aliases
	//--------------------------------------------------------------------------
	inline constexpr float Infinity										=			Infinity_v<float>;
	inline constexpr float Lowest										=			Lowest_v<float>;
	inline constexpr float MinFloat										=			Min_v<float>;
	inline constexpr float MaxFloat										=			Max_v<float>;
	inline constexpr float Epsilon										=			Epsilon_v<float>;
	inline constexpr float TightTolerance								=			TightTolerance_v<float>;
	inline constexpr float LooseTolerance								=			LooseTolerance_v<float>;

	inline constexpr float Pi											=			Pi_v<float>;
	inline constexpr float TwoPi										=			TwoPi_v<float>;
	inline constexpr float HalfPi										=			HalfPi_v<float>;
	inline constexpr float QuarterPi									=			QuarterPi_v<float>;
	inline constexpr float InvPi										=			InvPi_v<float>;
	inline constexpr float InvTwoPi										=			InvTwoPi_v<float>;
	inline constexpr float E											=			E_v<float>;
	inline constexpr float Sqrt2										=			Sqrt2_v<float>;
	inline constexpr float InvSqrt2										=			InvSqrt2_v<float>;
	inline constexpr float Sqrt3										=			Sqrt3_v<float>;
	inline constexpr float InvSqrt3										=			InvSqrt3_v<float>;
	inline constexpr float Ln2											=			Ln2_v<float>;
	inline constexpr float Ln10											=			Ln10_v<float>;
	inline constexpr float Log2E										=			Log2E_v<float>;
	inline constexpr float Log10E										=			Log10E_v<float>;
	inline constexpr float GoldenRatio									=			GoldenRatio_v<float>;
	inline constexpr float Deg2Rad										=			Deg2Rad_v<float>;
	inline constexpr float Rad2Deg										=			Rad2Deg_v<float>;
}