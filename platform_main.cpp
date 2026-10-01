#include "std.hpp"
#include "kaos.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <cstdlib>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return kaos_main(__argc, __argv);
}
#else

int main(int argc, char **argv)
{
    return kaos_main(argc, argv);
}

#endif
