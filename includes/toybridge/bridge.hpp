// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_TOYBRIDGE_BRIDGE_HPP
#define INCLUDED_TOYBRIDGE_BRIDGE_HPP

#include <toybridge/devinfo.hpp>
#include <toybridge/detail/init.hpp>
#include <toybridge/detail/out.hpp>
#include <srook/algorithm/for_each.hpp>
#include <srook/process/perror.hpp>
#include <srook/array.hpp>
#include <srook/type_traits/disjunction.hpp>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>

namespace toybridge {
SROOK_INLINE_NAMESPACE(v1)

class bridge {
public:
    SROOK_FORCE_INLINE bridge(const devinfo& di, srook::uint32_t filter = ETH_P_ALL, bool is_promiscous = false, bool is_verbose = false)
        : bridged_(false), verbose_(srook::move(is_verbose)),
        sock1_(detail::init(get<0>(di), filter, is_promiscous)), 
        sock2_(detail::init(get<1>(di), filter, is_promiscous)) 
    {}

    SROOK_FORCE_INLINE bool flip_verbose() SROOK_NOEXCEPT_TRUE
    {
        verbose_ = !verbose_;
        return !bridged_;
    }

    SROOK_FORCE_INLINE bool run(std::ostream& os)
    {
        bridged_ = true;
        if (!sock1_) return { "sock1: Failed to initialize descriptor." };
        else if (!sock2_) return { "sock2: Failed to initialize descriptor." };

        bool(sock1_ >>= [&os, this](int soc1) -> srook::optional<int> {
            return sock2_ >>= [&os, &soc1, this](int soc2) -> srook::optional<int> {
                srook::array<int, std::tuple_size<devinfo>::value> socks { soc1, soc2 };
                srook::array<::pollfd, std::tuple_size<devinfo>::value> targets;
                srook::algorithm::for_each(srook::algorithm::make_counter(socks), [&targets](int s, std::size_t i) { 
                    targets[i].fd = std::move(s);
                    targets[i].events = POLLIN | POLLERR;
                });

                ::u_char buf[2048];
                for (int nready = ::poll(targets.data(), std::tuple_size<devinfo>::value, 100); 
                        !end; 
                        nready = ::poll(targets.data(), std::tuple_size<devinfo>::value, 100)) {
                    switch (nready) {
                        case -1:
                            if (errno != EINTR) {
                                srook::process::perror("poll");
                                return srook::nullopt;
                            }
                            break;
                        default: {
                            bool bt = false;
                            srook::algorithm::for_each(srook::algorithm::make_counter(targets), [&bt, &buf, &socks, &os, this](const ::pollfd& t, std::size_t i) {
                                if (t.revents & (POLLIN | POLLERR)) {
                                    srook::optional<int> ops = this->read(socks[i], buf, sizeof(buf));
                                    if (!ops) {
                                        bt = true;
                                        srook::process::perror("read");
                                        return;
                                    }
                                    ops = ops >>= [&bt, &buf, &i, &socks, &os, this](int s) -> srook::optional<int> {
                                        if (detail::dump(os, i, buf, s, verbose_)){
                                            if (!this->write(socks[!i], buf, s)) { // TODO: This implementation allow only two devices.
                                                bt = true;
                                                srook::process::perror("write");
                                                return srook::nullopt;
                                            }
                                        }
                                        return { s };
                                    };
                                    if (!ops) return; 
                                }
                            });
                            if (bt) return srook::nullopt;
                            break;
                        }
                    }
                }
            };
        });
    }
private:
    SROOK_FORCE_INLINE srook::optional<int> 
    read(int soc, ::u_char* buf, std::size_t len) SROOK_NOEXCEPT_TRUE
    {
        int size = ::read(srook::move(soc), buf, srook::move(len));
        return size <= 0 ? srook::nullopt : srook::make_optional(size);
    }

    SROOK_FORCE_INLINE srook::optional<int>
    write(int soc, ::u_char* buf, std::size_t len) SROOK_NOEXCEPT_TRUE
    {
        int size = ::write(srook::move(soc), buf, srook::move(len));
        return size <= 0 ? srook::nullopt : srook::make_optional(size);
    }

    SROOK_FORCE_INLINE bool 
    disable_ip_forward()
    {
        {
            auto ifs = make_fst<std::ifstream>();
            if (!ifs) return false;
            std::getline(ifs.value(), ip_forward_backup);
        }
        {
            auto ofs = make_fst<std::ofstream>();
            if (!ofs) return false;
            ofs.value() << "0";
        }
        return true;
    }

    SROOK_FORCE_INLINE bool
    undo_ip_forward() // call run tail
    {
        auto ofs = make_fst<std::ofstream>();
        if (!ofs) return false;
        ofs.value() << ip_forward_backup;
        return true;
    }

    template <class T, SROOK_REQUIRES(srook::disjunction<srook::is_same<T, std::ofstream>, srook::is_same<T, std::ifstream>>::value)>
    SROOK_FORCE_INLINE srook::optional<T> make_fst()
    {
        T fs("/proc/sys/net/ipv4/ip_forward");
        if (fs.fail()) return srook::nullopt;
        return { fs };
    }

    bool bridged_, verbose_;
    srook::optional<int> sock1_, sock2_;
    std::string ip_forward_backup;

    static bool end;
};

bool bridge::end = false;

SROOK_INLINE_NAMESPACE_END
} // namespace toybridge

#endif
