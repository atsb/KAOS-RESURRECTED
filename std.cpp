 


#include <cstdlib>
#include <cstdio>
#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif
#include "std.hpp"

#define ID_LEN 32

const char NicMark[ID_LEN+1] = "Programmed by DOMENICO VALENTINI";
const std::uint32_t SerialNo = 0x00000000u;
const word NMEvenCRC = 1339, NMOddCRC = 1286;
const word SNEvenCRC = 0, SNOddCRC = 0;
char ShutDown = 0;

void error(const char *err_fnc, const char *err_on)
{
    const char *fn = err_fnc ? err_fnc : "SYSTEM";
    const char *why = err_on ? err_on : "unknown";

    std::fprintf(stderr, "error: %s() - %s\n", fn, why);

#if defined(_WIN32)
    char message[512];
    std::snprintf(message, sizeof(message), "KAOS error: %s() - %s", fn, why);
    MessageBoxA(nullptr, message, "KAOS Restoration", MB_OK | MB_ICONERROR);
#endif

    ShutDown = 1;
    std::exit(EXIT_FAILURE);
}

static void CMP_CRC(const byte *code, int count, word even, word odd)
{
    word evencrc = 0, oddcrc = 0;

    for (count >>= 1; count--; ) {
        oddcrc = static_cast<word>(oddcrc + *(code++));
        evencrc = static_cast<word>(evencrc + *(code++));
    }

    if (evencrc != even || oddcrc != odd)
        error("SYSTEM", "Programmer HALT");
}

void CheckCRC(void)
{
    CMP_CRC(reinterpret_cast<const byte *>(NicMark), ID_LEN,
            NMEvenCRC, NMOddCRC);
    CMP_CRC(reinterpret_cast<const byte *>(&SerialNo), 4,
            SNEvenCRC, SNOddCRC);
}
