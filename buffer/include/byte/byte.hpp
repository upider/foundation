#ifndef __BYTE_HPP__
#define __BYTE_HPP__

#include <cstring>
#include <type_traits>

enum class byte : unsigned char {};

template <typename I>
constexpr auto to_byte(I value) noexcept -> typename std::enable_if<std::is_integral<I>::value, byte>::type {
  return static_cast<byte>(value);
}

template <typename I = unsigned char>
constexpr auto to_integer(byte b) noexcept -> typename std::enable_if<std::is_integral<I>::value, I>::type {
  return static_cast<I>(b);
}

constexpr byte operator~(byte b) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(~static_cast<unsigned int>(b)));
}

constexpr byte operator|(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs)));
}

constexpr byte operator&(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)));
}

constexpr byte operator^(byte lhs, byte rhs) noexcept {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs)));
}

template <typename I>
constexpr auto operator<<(byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, byte>::type {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) << shift));
}

template <typename I>
constexpr auto operator>>(byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, byte>::type {
  return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(b) >> shift));
}

byte& operator|=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs | rhs;
}

byte& operator&=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs & rhs;
}

byte& operator^=(byte& lhs, byte rhs) noexcept {
  return lhs = lhs ^ rhs;
}

template <typename I>
constexpr auto operator<<=(byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, byte&>::type {
  return b = b << shift;
}

template <typename I>
constexpr auto operator>>=(byte& b, I shift) noexcept -> typename std::enable_if<std::is_integral<I>::value, byte&>::type {
  return b = b >> shift;
}

template <typename T>
auto to_bytes(byte* dst, const T& src) -> typename std::enable_if<std::is_trivially_copyable<T>::value>::type {
  static_assert(std::is_trivially_copyable<byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable<T>::value,    "nstd::to_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, &src, sizeof(T)));
}

template <typename T>
auto to_bytes(byte* dst, const T* src, std::size_t count) -> typename std::enable_if<std::is_trivially_copyable<T>::value>::type {
  static_assert(std::is_trivially_copyable<byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable<T>::value,    "nstd::to_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, src, count * sizeof(byte)));
}

template <typename T>
auto from_bytes(const byte* src) -> typename std::enable_if<std::is_trivial<T>::value, T>::type {
  static_assert(std::is_trivially_copyable<byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivial<T>::value,               "nstd::from_bytes requires T is trivial.");
  T dst;
  static_cast<void>(std::memcpy(&dst, src, sizeof(T)));
  return dst;
}

template <typename T>
auto from_bytes(T* dst, const byte* src, std::size_t count) -> typename std::enable_if<std::is_trivially_copyable<T>::value>::type {
  static_assert(std::is_trivially_copyable<byte>::value, "nstd::from_bytes requires byte is trivially copyable.");
  static_assert(std::is_trivially_copyable<T>::value,    "nstd::from_bytes requires T is trivially copyable.");
  static_cast<void>(std::memcpy(dst, src, count * sizeof(byte)));
}

#endif // __BYTE_HPP__