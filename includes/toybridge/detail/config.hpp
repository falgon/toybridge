// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_TOYBRIDGE_DETAIL_CONFIG_HPP
#define INCLUDED_TOYBRIDGE_DETAIL_CONFIG_HPP

#if defined(__linux__) && defined(__GNUC__)
#   include <srook/config.hpp>
#   if SROOK_CPP_LAMBDAS
#       include <srook/cstdint.hpp>
#       include <arpa/inet.h>
#       include <netinet/if_ether.h>
#       include <sys/socket.h>
#       ifndef ETHERTYPE_IPV6
#           define ETHERTYPE_IPV6 0x86dd
#       endif
#   else
#       error "This feature needs to lambda-expression."
#   endif
#else
#   error "This feature needs to linux headers and the gcc compiler."
#endif

#endif
