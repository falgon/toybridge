// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_TOYBRIDGE_DETAIL_CONFIG_HPP
#define INCLUDED_TOYBRIDGE_DETAIL_CONFIG_HPP

#if defined(__linux__) && defined(__GNUC__)
#   include <srook/config.hpp>
#   include <srook/cstdint.hpp>
#   include <arpa/inet.h>
#   include <netinet/if_ether.h>
#   include <sys/socket.h>
#   ifndef ETHERTYPE_IPV6
#       define ETHERTYPE_IPV6 0x86dd
#   endif
#   ifndef ETH_P_ERSPAN
#       define ETH_P_ERSPAN 0x88be
#   endif
#   ifndef ETH_P_MACSEC
#       define ETH_P_MACSEC 0x88e5
#   endif
#   ifndef ETH_P_NCSI
#       define ETH_P_NCSI 0x88f8
#   endif
#   ifndef ETH_P_IBOE
#       define ETH_P_IBOE 0x8915
#   endif
#   ifndef ETH_P_HSR
#       define ETH_P_HSR 0x892f
#   endif
#   ifndef ETH_P_NSH
#       define ETH_P_NSH 0x894f
#   endif
#   ifndef ETH_P_IFE
#       define ETH_P_IFE 0xed3e
#   endif
#   ifndef ETH_P_MAP
#       define ETH_P_MAP 0x00f9
#   endif
#else
#   error "This feature needs to linux headers and the gcc compiler."
#endif

#endif
