#include <toybridge/bridge.hpp>

int main()
{
    toybridge::devinfo devs ("enp0s9", "en0");
    toybridge::bridge br { devs };
    br.flip_verbose();
    br.run();
}
