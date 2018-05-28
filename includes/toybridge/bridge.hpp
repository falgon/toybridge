// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_TOYBRIDGE_BRIDGE_HPP
#define INCLUDED_TOYBRIDGE_BRIDGE_HPP

#include <toybridge/devinfo.hpp>
#include <toybridge/detail/init.hpp>
#include <toybridge/detail/out.hpp>
#include <srook/algorithm/for_each.hpp>
#include <srook/process/perror.hpp>
#include <srook/scope/unique_resource.hpp>
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
    SROOK_FORCE_INLINE bridge(const devinfo& di, srook::uint32_t filter = ETH_P_ALL, bool is_promiscous = true, bool is_verbose = false)
        :bridged_(false), verbose_(srook::move(is_verbose)),
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
        ipfwd_config ipfwd;

        return bool(ipfwd.disable() >> [&]() -> srook::optional<int> {
            regist_signal();
            return sock1_ >>= [&ipfwd, &os, this](int soc1) -> srook::optional<int> {
                return (sock2_ >>= [&os, &soc1, this](int soc2) -> srook::optional<int> {
                    SROOK_ATTRIBUTE_UNUSED const auto rs1 = srook::scope::make_unique_resource(soc1, ::close);
                    SROOK_ATTRIBUTE_UNUSED const auto rs2 = srook::scope::make_unique_resource(soc2, ::close);

                    srook::array<int, std::tuple_size<devinfo>::value> socks { soc1, soc2 };
                    srook::array<::pollfd, std::tuple_size<devinfo>::value> targets;
                    srook::algorithm::for_each(srook::algorithm::make_counter(socks), [&targets](int s, std::size_t i) { 
                        targets[i].fd = std::move(s);
                        targets[i].events = POLLIN | POLLERR;
                    });

                    ::u_char buf[2048]{};
                    SROOK_CONSTEXPR_OR_CONST int timeout = 100;
                    for (int nready = ::poll(targets.data(), targets.size(), timeout); 
                            !end; 
                            nready = ::poll(targets.data(), targets.size(), timeout)) {
                        switch (nready) {
                            case -1:
                                if (errno != EINTR) {
                                    srook::process::perror("poll");
                                    return srook::nullopt;
                                }
                                break;
                            case 0:
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
                                            if (detail::dump(os, i, buf, s, verbose_)) {
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
                            }
                        }
                    }
                    return { soc1 };
                }) >>= [&ipfwd](int sock) -> srook::optional<int> {
                    return ipfwd.undo() ? srook::make_optional(sock) : srook::nullopt;
                };
            };
        });
    }
private:
    SROOK_FORCE_INLINE void regist_signal() SROOK_NOEXCEPT_TRUE
    {
        ::signal(SIGINT, end_signal);
        ::signal(SIGTERM, end_signal);
        ::signal(SIGQUIT, end_signal);
        ::signal(SIGPIPE, SIG_IGN);
        ::signal(SIGTTIN, SIG_IGN);
        ::signal(SIGTTOU, SIG_IGN);
    }
    
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

    struct ipfwd_config SROOK_FINAL {
        SROOK_CONSTEXPR SROOK_FORCE_INLINE ipfwd_config() SROOK_NOEXCEPT_TRUE
            : ipfwd_backup('1') {}

        SROOK_FORCE_INLINE srook::optional<std::ifstream> disable()
        {
            return (make_fst<std::ifstream>() >>= [this](std::ifstream ifs) -> srook::optional<std::ifstream> {
                ipfwd_backup = ifs.get();
                ifs.close();
                return { srook::move(ifs) };
            }) >>= [this](std::ifstream ifs) -> srook::optional<std::ifstream> {
                if (ipfwd_backup != '0') {
                    return bool(make_fst<std::ofstream>() >>= [this](std::ofstream ofs) -> srook::optional<std::ofstream> {
                        ofs << "0";
                        ofs.close();
                        return { srook::move(ofs) };
                    }) ? srook::make_optional(srook::move(ifs)) : srook::nullopt;
                } else {
                    return { srook::move(ifs) };
                }
                SROOK_UNREACHABLE();
            };
        }

        SROOK_FORCE_INLINE bool undo()
        {
            return 
                ipfwd_backup != '0' ? 
                    bool(make_fst<std::ofstream>() >>= [this](std::ofstream ofs) -> srook::optional<std::ofstream> {
                        ofs << ipfwd_backup;
                        ofs.close();
                        return { srook::move(ofs) };
                    })
                : true;
        }
    private:
        template <class T, SROOK_REQUIRES(srook::disjunction<srook::is_same<T, std::ofstream>, srook::is_same<T, std::ifstream>>::value)>
        SROOK_FORCE_INLINE srook::optional<T> make_fst()
        {
            T fs("/proc/sys/net/ipv4/ip_forward");
            if (fs.fail()) return srook::nullopt;
            return { srook::move(fs) };
        }
        
        char ipfwd_backup;
    };


    bool bridged_, verbose_;
    srook::optional<int> sock1_, sock2_;
    
    static bool end;
    static void end_signal(int) SROOK_NOEXCEPT_TRUE { end = true; }
};

bool bridge::end = false;

SROOK_INLINE_NAMESPACE_END
} // namespace toybridge

#endif
