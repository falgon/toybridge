#include <toybridge/bridge.hpp>

SROOK_FORCE_INLINE bool cmdarg_check(const int argc, const char* const progname)
{
    if (argc != 3) {
        std::cerr << "Usage: " << progname << " <interface 1> <interface 2>" << std::endl;
        return false;
    } else if (::getuid() && geteuid()) {
        std::cerr << "Needs to be superuser" << std::endl;
        return false;
    }
    return true;
}

int main(const int argc, const char** const argv)
{
    if (!cmdarg_check(argc, argv[0])) return EXIT_FAILURE;

    toybridge::devinfo devs (argv[1], argv[2]);
    toybridge::bridge br { devs };
    br.flip_verbose();
    br.run(std::cout);
}
