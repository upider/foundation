#ifndef __POSIX_HPP__
#define __POSIX_HPP__

#include <arpa/inet.h>
#include <fcntl.h>

#include <cstring>
#include <string>

#include "net/address.hpp"

namespace net
{
    class Posix
    {
    public:
        static inline void non_blocking(int fd, bool non_blocking)
        {
            if (non_blocking)
            {
                fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
            }
            else
            {
                int flags = fcntl(fd, F_GETFL, 0);
                flags &= ~O_NONBLOCK;
                fcntl(fd, F_SETFL, flags);
            }
        }

        static inline sockaddr_in sock_address(const std::string &ip, uint16_t port, int family)
        {
            struct sockaddr_in address;
            bzero(&address, sizeof(struct sockaddr_in));
            address.sin_family = family;
            address.sin_port = htons(port);
            if (ip.empty())
            {
                address.sin_addr.s_addr = htonl(INADDR_ANY);
            }
            else
            {
                address.sin_addr.s_addr = inet_addr(ip.c_str());
            }
            return address;
        }

        static inline Address address(const sockaddr_in &storage)
        {
            Address address;
            socklen_t sock_len = sizeof(storage);
            if (storage.sin_family == AF_INET)
            {
                sockaddr_in *addr = (sockaddr_in *)&storage;
                char ip[INET_ADDRSTRLEN];
                ::inet_ntop(AF_INET, &addr->sin_addr, ip, sock_len);
                address.ip(ip);
                address.port(ntohs(addr->sin_port));
            }
            else if (storage.sin_family == AF_INET6)
            {
                sockaddr_in6 *addr = (sockaddr_in6 *)&storage;
                char ip[INET6_ADDRSTRLEN];
                ::inet_ntop(AF_INET6, &addr->sin6_addr, ip, sock_len);
                address.ip(ip);
                address.port(ntohs(addr->sin6_port));
            }
            return address;
        }

        // static inline sockaddr_in remote_address(int fd)
        // {
        //     Address* address;
        //     sockaddr_storage storage;
        //     socklen_t sock_len = sizeof(storage); // 必须给初值
        //     int ret = ::getpeername(fd, (sockaddr *)&storage, &sock_len);
        //     if (ret == 0)
        //     {
        //         if (storage.ss_family == AF_INET)
        //         {
        //             sockaddr_in *addr = (sockaddr_in *)&storage;
        //             char ip[INET_ADDRSTRLEN];
        //             ::inet_ntop(AF_INET, &addr->sin_addr, ip, sock_len);
        //             address.ip(ip);
        //             address.port(ntohs(addr->sin_port));
        //         }
        //         else if (storage.ss_family == AF_INET6)
        //         {
        //             sockaddr_in6 *addr = (sockaddr_in6 *)&storage;
        //             char ip[INET6_ADDRSTRLEN];
        //             ::inet_ntop(AF_INET6, &addr->sin6_addr, ip, sock_len);
        //             address.ip(ip);
        //             address.port(ntohs(addr->sin6_port));
        //         }
        //     }
        //     return address;
        // }
    };

} // namespace net
#endif /* __POSIX_HPP__ */
