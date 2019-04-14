#pragma once

#ifdef _WIN64
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#endif

#define G_NAMESPACE_NAME SnowMPM
#define G_NAMESPACE_BEGIN namespace G_NAMESPACE_NAME{
#define G_NAMESPACE_END }

G_NAMESPACE_BEGIN

using float32 = float;
using float64 = double;
using real = float64;
using uint = unsigned int;

G_NAMESPACE_END