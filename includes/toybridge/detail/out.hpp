#ifndef INCLUDED_TOYBRIDGE_DETAIL_OUT_HPP
#define INCLUDED_TOYBRIDGE_DETAIL_OUT_HPP

#include <toybridge/detail/config.hpp>
#include <srook/algorithm/copy.hpp>
#include <srook/cstring/memcpy.hpp>
#include <srook/iterator/ostream_joiner.hpp>
#include <srook/range/adaptor/transformed.hpp>
#include <srook/string/string_view.hpp>
#include <linux/if_ether.h>
#include <sstream>

namespace toybridge {
SROOK_INLINE_NAMESPACE(v1)
namespace detail {

SROOK_FORCE_INLINE std::ostream& title(std::ostream& os, const char* t, const char* l, std::size_t n = 20)
{
    os << t << ' ';
    while (--n) os << l;
    return os;
}

SROOK_FORCE_INLINE std::ostream& macaddr_n2a(std::ostream& os, const ::u_char* hwaddr)
{
    std::stringstream ostr;
#if SROOK_CPP_LAMBDAS
    SROOK_CXX17_CONSTEXPR auto l = [](::u_char c) SROOK_CXX17_CONSTEXPR -> int { return static_cast<int>(c); };
#else
    struct l_ {
        SROOK_CXX17_CONSTEXPR SROOK_FORCE_INLINE int
        operator()(::u_char c) const SROOK_NOEXCEPT_TRUE { return static_cast<int>(c); }
    } l;
#endif
    srook::algorithm::copy(srook::string::basic_string_view<::u_char>(hwaddr, ETH_ALEN) | 
            srook::range::adaptors::transformed(srook::move(l)),
            srook::make_ostream_joiner(ostr << std::hex, ":"));
    return os << ostr.str();
}

std::ostream& out(std::ostream& os, const ::ether_header& eh)
{
    title(os, "ether_header", "-") << '\n';
    os << "\tdhost = ";
    macaddr_n2a(os, eh.ether_dhost) << '\n';
    macaddr_n2a(os, eh.ether_shost) << '\n';
    os << "\ttype = " << std::hex << ::ntohs(eh.ether_type) << std::dec;

#define TOYBRIDGE_ETHER_HEADER_OUT_CASE(CASE_, MESSAGE_)\
    case CASE_:\
        os << '(' << #MESSAGE_ << ")\n";\
        break

    switch (::ntohs(eh.ether_type)) {
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_LOOP, LOOP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PUP, PUP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PUPAT, PUPAT);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_TSN, TSN);    
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IP, IP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_X25, X25);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ARP, ARP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_BPQ, BPQ);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IEEEPUP, IEEEPUP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IEEEPUPAT, IEEEPUPAT);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_BATMAN, BATMAN);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DEC, DEC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DNA_DL, DNA_DL);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DNA_RC, DNA_RC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DNA_RT, DNA_RT);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_LAT, LAT);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DIAG, DIAG);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_CUST, CUST);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_SCA, SCA);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_TEB, TEB);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_RARP, RARP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ATALK, ATALK);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_AARP, AARP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_8021Q, 8021Q);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ERSPAN, ERSPAN);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IPX, IPX);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IPV6, IPV6);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PAUSE, PAUSE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_SLOW, SLOW);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_WCCP, WCCP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_MPLS_UC, MPLS_UC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_MPLS_MC, MPLS_MC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ATMMPOA, ATMMPOA);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PPP_DISC, PPP_DISC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PPP_SES, PPP_SES);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_LINK_CTL, LINK_CTL);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ATMFATE, ATMFATE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PAE, PAE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_AOE, AOE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_8021AD, 8021AD);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_802_EX1, 802_EX1);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_TIPC, TIPC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_MACSEC, MACSEC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_8021AH, 8021AH);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_MVRP, MVRP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_1588, P1588);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_NCSI, NCSI);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PRP, PRP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_FCOE, FCOE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IBOE, IBOE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_TDLS, TDLS);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_FIP, FIP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_80221, P80221);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_HSR, HSR);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_NSH, NSH);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_LOOPBACK, LOOPBACK);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_QINQ1, QINQ1);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_QINQ2, QINQ2);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_QINQ3, QINQ3);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_EDSA, EDSA);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IFE, IFE);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_AF_IUCV  , AF_IUCV  );
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_802_3_MIN, 802_3_MIN);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_802_3, 802_3);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_AX25, AX25);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ALL, ALL);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_802_2, 802_2);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_SNAP, SNAP);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DDCMP, DDCMP    );
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_WAN_PPP, WAN_PPP  );
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PPP_MP, PPP_MP   );
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_LOCALTALK, LOCALTALK);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_CAN, CAN);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_CANFD, CANFD);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PPPTALK, PPPTALK);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_TR_802_2, TR_802_2);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_MOBITEX, MOBITEX);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_CONTROL, CONTROL);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IRDA, IRDA);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ECONET, ECONET);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_HDLC, HDLC);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_ARCNET, ARCNET);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_DSA, DSA);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_TRAILER, TRAILER);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_PHONET, PHONET);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_IEEE802154, IEEE802154);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_CAIF, CAIF);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_XDSA, XDSA);
        TOYBRIDGE_ETHER_HEADER_OUT_CASE(ETH_P_MAP, MAP);
        default:
            os << "(unknown)\n";
            break;
    }
#undef TOYBRIDGE_ETHER_HEADER_OUT_CASE
    return os;
}

SROOK_FORCE_INLINE bool dump(std::ostream& os, int devno, const ::u_char* data, std::size_t size, bool is_verbose)
{
    if (size < sizeof(::ether_header)) {
        if (is_verbose) os << '[' << devno << "]: size(" << size << ") < sizeof(::ether_header)\n";
        return false;
    }

    ::ether_header eh {};
    srook::cstring::memcpy(&eh, &data, sizeof(eh));
    data += sizeof(::ether_header);
    size -= sizeof(::ether_header);
    if (is_verbose) {
        os << '[' << devno << ']';
        out(os, eh) << '\n';
    }
    return true;
}

} // namespace detail
SROOK_INLINE_NAMESPACE_END
} // namespace toybridge
#endif
