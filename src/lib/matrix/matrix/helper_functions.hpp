#pragma once

#include "math.hpp"

#if defined (__PX4_NUTTX) || defined (__PX4_QURT)
#include <px4_defines.h>
#endif

namespace matrix
{

template<typename Type>
bool is_finite(Type x)
{
#if defined (__PX4_NUTTX)
	return PX4_ISFINITE(x);
#elif defined (__PX4_QURT)
	return __builtin_isfinite(x);
#else
	return std::isfinite(x);
#endif
}

/**
 * Compare if two floating point numbers are equal
 *
 * NAN is considered equal to NAN and -NAN
 * INFINITY is considered equal INFINITY but not -INFINITY
 *
 * @param x right side of equality check
 * @param y left side of equality check
 * @param eps numerical tolerance of the check
 * @return true if the two values are considered equal, false otherwise
 */
template<typename Type>
bool isEqualF(const Type x, const Type y, const Type eps = Type(1e-4f))
{
	return (matrix::fabs(x - y) <= eps)
	       || (isnan(x) && isnan(y))
	       || (isinf(x) && isinf(y) && isnan(x - y));
}

namespace detail
{

template<typename Floating>
Floating wrap_floating(Floating x, Floating low, Floating high)
{
	// already in range
	if (low <= x && x < high) {
		return x;
	}

	const auto range = high - low;
	const auto inv_range = Floating(1) / range; // should evaluate at compile time, multiplies below at runtime
	const auto num_wraps = floor((x - low) * inv_range);
	return x - range * num_wraps;
}

}  // namespace detail

/**
 * Wrap single precision floating point value to stay in range [low, high)
 *
 * @param x input possibly outside of the range
 * @param low lower limit of the allowed range
 * @param high upper limit of the allowed range
 * @return wrapped value inside the range
 */
inline float wrap(float x, float low, float high)
{
	return matrix::detail::wrap_floating(x, low, high);
}

/**
 * Wrap double precision floating point value to stay in range [low, high)
 *
 * @param x input possibly outside of the range
 * @param low lower limit of the allowed range
 * @param high upper limit of the allowed range
 * @return wrapped value inside the range
 */
inline double wrap(double x, double low, double high)
{
	return matrix::detail::wrap_floating(x, low, high);
}

/**
 * Wrap integer value to stay in range [low, high)
 *
 * @param x input possibly outside of the range
 * @param low lower limit of the allowed range
 * @param high upper limit of the allowed range
 * @return wrapped value inside the range
 */
template<typename Integer>
Integer wrap(Integer x, Integer low, Integer high)
{
	const auto range = high - low;

	if (x < low) {
		x += range * ((low - x) / range + 1);
	}

	return low + (x - low) % range;
}

/**
 * Wrap value in range [-π, π)
 */
template<typename Type>
Type wrap_pi(Type x)
{
	return wrap(x, Type(-M_PI), Type(M_PI));
}

/**
 * Wrap value in range [0, 2π)
 */
template<typename Type>
Type wrap_2pi(Type x)
{
	return wrap(x, Type(0), Type(M_TWOPI));
}

/**
 * Unwrap value that was wrapped with range [low, high)
 *
 * @param[in] last_x Last unwrapped value
 * @param[in] new_x New value in range
 * @param low lower limit of the wrapping range
 * @param high upper limit of the wrapping range
 * @return New unwrapped value
 */
template<typename Type>
Type unwrap(const Type last_x, const Type new_x, const Type low, const Type high)
{
	return last_x + wrap(new_x - last_x, low, high);
}

/**
 * Unwrap value with range [-π, π)
 *
 * @param[in] last_angle Last unwrapped angle [rad]
 * @param[in] new_angle New angle in [-pi, pi] [rad]
 * @param
 * @return New unwrapped angle [rad]
 */
template<typename Type>
Type unwrap_pi(const Type last_angle, const Type new_angle)
{
	return unwrap(last_angle, new_angle, Type(-M_PI), Type(M_PI));
}

template<typename T>
int sign(T val)
{
	return (T(FLT_EPSILON) < val) - (val < T(FLT_EPSILON));
}

} // namespace matrix
