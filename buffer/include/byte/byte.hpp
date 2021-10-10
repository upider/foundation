#ifndef __BYTE_HPP__
#define __BYTE_HPP__

#include <cstring>
#include <type_traits>

enum class Byte : unsigned char {};

Byte& operator|=(Byte& lhs, Byte rhs) noexcept;
Byte& operator&=(Byte& lhs, Byte rhs) noexcept;
Byte& operator^=(Byte& lhs, Byte rhs) noexcept;
constexpr Byte operator~(Byte b) noexcept;
constexpr Byte operator|(Byte lhs, Byte rhs) noexcept;
constexpr Byte operator&(Byte lhs, Byte rhs) noexcept;
constexpr Byte operator^(Byte lhs, Byte rhs) noexcept;

template <typename I>
constexpr auto to_byte(I value) noexcept -> typename std::enable_if<std::is_integral<I>::value, Byte>::type {
  return static_cast<Byte>(value);
}

template <typename I = unsigned char>
constexpr auto to_integer(Byte b) noexcept -> typename std::enable_if<std::is_integral<I>::value, I>::type {
  return static_cast<I>(b);
}

template <typename I>
constexpr auto operator<<(Byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, Byte>::type {
  return static_cast<Byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) << shift));
}

template <typename I>
constexpr auto operator>>(Byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, Byte>::type {
  return static_cast<Byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) >> shift));
}

template <typename I>
constexpr auto operator<<=(Byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, Byte&>::type {
  return b = b << shift;
}

template <typename I>
constexpr auto operator>>=(Byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, Byte&>::type {
  return b = b >> shift;
}

template <typename T>
auto to_bytes(Byte* dst, const T& src) -> typename std::enable_if<std::is_trivially_copyable<T>::value>::type {
  static_assert(std::is_trivially_copyable<Byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable<T>::value,    "nstd::to_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, &src, sizeof(T)));
}

template <typename T>
auto to_bytes(Byte* dst, const T* src, std::size_t count) -> typename std::enable_if<std::is_trivially_copyable<T>::value>::type {
  static_assert(std::is_trivially_copyable<Byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable<T>::value,    "nstd::to_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, src, count * sizeof(Byte)));
}

template <typename T>
auto from_bytes(const Byte* src) -> typename std::enable_if<std::is_trivial<T>::value, T>::type {
  static_assert(std::is_trivially_copyable<Byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivial<T>::value,               "nstd::from_bytes requires T is trivial.");
  T dst;
  static_cast<void>(std::memcpy(&dst, src, sizeof(T)));
  return dst;
}

template <typename T>
auto from_bytes(T* dst, const Byte* src, std::size_t count) -> typename std::enable_if<std::is_trivially_copyable<T>::value>::type {
  static_assert(std::is_trivially_copyable<Byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable<T>::value,    "nstd::from_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, src, count * sizeof(Byte)));
}

#endif // __BYTE_HPP__