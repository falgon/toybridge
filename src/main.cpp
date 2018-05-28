#include <toybridge/bridge.hpp>

int main()
{
    toybridge::devinfo devs ("enp0s3", "enp0s9");
    toybridge::bridge br { devs };
    br.flip_verbose();
    br.run(std::cout);
}
