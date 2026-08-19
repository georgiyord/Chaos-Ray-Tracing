#ifndef RENDERENGINE_UTILS_HPP
#define RENDERENGINE_UTILS_HPP

#include <cstdint>
#include <limits>
#include <type_traits>

#define RENDERENGINE_HITPOINT_BIAS 1e-6f
#define RENDERENGINE_SHADOW_BIAS 5e-2f

namespace RenderEngine {
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

constexpr float floatInf = std::numeric_limits<float>::infinity();
constexpr float floatNaN = std::numeric_limits<float>::quiet_NaN();

enum class RenderMode {
  Default,
  NormalShade,
  DistanceShade,
  GoochShade,
  BarycentricShade
};

} // namespace RenderEngine

#endif // RENDERENGINE_UTILS_HPP