// Copyright (C) 2011-2018 Roki. Distributed under the MIT License
#ifndef INCLUDED_TOYBRIDGE_DEVINFO_HPP
#define INCLUDED_TOYBRIDGE_DEVINFO_HPP

#include <toybridge/detail/config.hpp>
#include <srook/string/string_view.hpp>
#include <srook/type_traits/detail/logical.hpp>
#include <srook/type_traits/decay.hpp>
#include <srook/type_traits/is_constructible.hpp>
#include <srook/type_traits/bool_constant.hpp>
#include <srook/type_traits/type_constant.hpp>
#include <srook/tmpl/vt/transfer.hpp>
#include <srook/tmpl/vt/replicate.hpp>
#include <tuple>

namespace toybridge {
SROOK_INLINE_NAMESPACE(v1)

class devinfo {
public:
    // TODO: This implementation allowed only 2 devices.
    SROOK_INLINE_VARIABLE static SROOK_CONSTEXPR_OR_CONST std::size_t max_devices = 2; 
    typedef srook::string::string_view string_type;
    typedef SROOK_DEDUCED_TYPENAME srook::tmpl::vt::transfer<std::tuple, SROOK_DEDUCED_TYPENAME srook::tmpl::vt::replicate<max_devices, string_type>::type>::type tuple_type;

    template <class... Ts, 
    SROOK_REQUIRES(srook::type_traits::detail::Land</*srook::is_constructible<string_type, SROOK_DEDUCED_TYPENAME decay<Ts>::type>...,*/ srook::bool_constant<sizeof...(Ts) == max_devices>>::value)>
    SROOK_FORCE_INLINE SROOK_CONSTEXPR devinfo(Ts&&... ts) SROOK_NOEXCEPT_TRUE 
        : devices { srook::forward<Ts>(ts)... } 
    {}
    
    tuple_type devices;
};

template <std::size_t n>
SROOK_CONSTEXPR SROOK_DECLTYPE(auto) 
get(const devinfo& d) SROOK_NOEXCEPT_TRUE 
{ 
    return std::get<n>(d.devices); 
}

SROOK_INLINE_NAMESPACE_END
} // namespace toybridge

namespace std {

template <>
struct tuple_size<toybridge::devinfo> 
    : integral_constant<std::size_t, 2> {};

template <std::size_t n>
struct tuple_element<n, toybridge::devinfo>
    : srook::type_constant<toybridge::devinfo::string_type> {};

} // namespace std

#endif

