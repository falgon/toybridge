// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_TOYBRIDGE_DETAIL_INIT_HPP
#define INCLUDED_TOYBRIDGE_DETAIL_INIT_HPP
#include <toybridge/detail/config.hpp>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <srook/algorithm/copy.hpp>
#include <srook/iterator/ostream_joiner.hpp>
#include <srook/optional.hpp>
#include <srook/process/perror.hpp>
#include <srook/range/adaptor/transformed.hpp>
#include <srook/string/string_view.hpp>

namespace toybridge {
SROOK_INLINE_NAMESPACE(v1)

namespace detail {

SROOK_FORCE_INLINE void error_close(const char* s, int soc)
SROOK_NOEXCEPT(srook::process::perror(s))
{
    srook::process::perror(s);
    ::close(soc);
}

srook::optional<int> ioctl(int soc, int flag, ::ifreq* ifr) 
SROOK_NOEXCEPT_TRUE
{
    return ::ioctl(soc, flag, ifr) < 0 ? error_close("ioctl", soc), srook::nullopt : srook::make_optional(soc);
}

srook::optional<int> socket(int packet_type, int sock_type, uint16_t ht)
SROOK_NOEXCEPT_TRUE
{
    int soc = ::socket(packet_type, sock_type, ht);
    return soc < 0 ? error_close("socket", soc), srook::nullopt : srook::make_optional(soc);
}

srook::optional<int> bind(int sockfd, const ::sockaddr* addr, ::socklen_t addrlen)
SROOK_NOEXCEPT_TRUE
{
    return ::bind(sockfd, addr, addrlen) < 0 ? error_close("bind", sockfd), srook::nullopt : srook::make_optional(sockfd);
}

srook::optional<int> init(srook::string_view device, srook::uint32_t filter = ETH_P_ALL, bool pb = false) 
SROOK_NOEXCEPT_TRUE
{
    return toybridge::detail::socket(PF_PACKET, SOCK_RAW, htons(filter)) >>= [&](int soc) -> srook::optional<int> {
        ::ifreq ifr{};
        std::size_t devsize = device.size() < sizeof(ifr.ifr_name) - 1 ? device.size() : sizeof(ifr.ifr_name) - 1;
        std::copy_n(device.cbegin(), srook::move(devsize), ifr.ifr_name);

        return toybridge::detail::ioctl(soc, SIOCGIFINDEX, &ifr) >>= [&](int soc) -> srook::optional<int> {
            union address {
                ::sockaddr_ll sal;
                ::sockaddr a;
            } addr{};
            addr.sal.sll_family = PF_PACKET;
            addr.sal.sll_protocol = htons(filter);
            addr.sal.sll_ifindex = ifr.ifr_ifindex;

            return toybridge::detail::bind(soc, &addr.a, sizeof(addr.sal)) >>= [&](int soc) -> srook::optional<int> {
                return !pb ? srook::make_optional(soc) : toybridge::detail::ioctl(soc, SIOCGIFFLAGS, &ifr) >>= [&ifr](int soc) -> srook::optional<int> {
                    ifr.ifr_flags = ifr.ifr_flags | IFF_PROMISC;
                    return toybridge::detail::ioctl(soc, SIOCSIFFLAGS, &ifr);
                };
            };
        };
    };
}

} // namespace detail

SROOK_INLINE_NAMESPACE_END
} // namespace toybridge

#endif
