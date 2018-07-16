#pragma once

// returns value closest to v which is in [lower, upper]
template <class value_type>
inline value_type clamped(const value_type &v, const value_type &lower, const value_type &upper)
{
	return v > upper ? upper : (v < lower ? lower : v);
}

template <class value_type>
inline value_type wrap(const value_type &v, const value_type &lower, const value_type &upper)
{
	value_type d = fabs(upper - lower);
	return (v < lower) ? (upper - fmod((lower - v), d)) : (lower + fmod((v - lower), d));
}
