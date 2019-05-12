#pragma once

#ifdef _WIN64
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#endif

#define G_NAMESPACE_NAME SnowMPM
#define G_NAMESPACE_BEGIN namespace G_NAMESPACE_NAME{
#define G_NAMESPACE_END }

#define SafeReleaseVAO(a,x) if(x>0) glDeleteVertexArrays(a, &x);
#define SafeReleaseBuffer(a,x) if(x>0) glDeleteBuffers(a, &x);
#define SafeReleaseTextures(a,x) if(x>0) glDeleteTextures(a, &x);


G_NAMESPACE_BEGIN

using float32 = float;
using float64 = double;
using real = float64;
using uint = unsigned int;

// from https://github.com/hi2p-perim/lightmetrica-v2
real constexpr operator"" _f(long double v) {
    return real(v);
}

real constexpr operator"" _f(unsigned long long v) {
    return real(v);
}

G_NAMESPACE_END