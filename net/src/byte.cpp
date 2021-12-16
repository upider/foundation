#include "net/buffer/byte.hpp"

namespace net
{
  namespace buffer
  {
    Byte &operator|=(Byte &lhs, Byte rhs) noexcept
    {
      return lhs = lhs | rhs;
    }

    Byte &operator&=(Byte &lhs, Byte rhs) noexcept
    {
      return lhs = lhs & rhs;
    }

    Byte &operator^=(Byte &lhs, Byte rhs) noexcept
    {
      return lhs = lhs ^ rhs;
    }

    constexpr Byte operator~(Byte b) noexcept
    {
      return static_cast<Byte>(static_cast<unsigned char>(~static_cast<unsigned int>(b)));
    }

    constexpr Byte operator|(Byte lhs, Byte rhs) noexcept
    {
      return static_cast<Byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs)));
    }

    constexpr Byte operator&(Byte lhs, Byte rhs) noexcept
    {
      return static_cast<Byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)));
    }

    constexpr Byte operator^(Byte lhs, Byte rhs) noexcept
    {
      return static_cast<Byte>(static_cast<unsigned char>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs)));
    }
  } // namespace buffer
} // namespace net