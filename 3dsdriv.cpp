#include "std.hpp"
#include "3dsdriv.hpp"

 






unsigned SBdefault[MIXER_DEVICE_NUM] = {};

int sb_ioaddr = 0;
byte sb_DMA8ch = 1;
byte sb_DMA16ch = 5;
byte sb_IRQnum = 5;
byte sb_type = SBT_SB16;
word DMAbuffer_size = 0;
byte *DMAbuffer = nullptr;
byte *DMAbufhalf = nullptr;
byte Old8259Mask = 0;

char sb_init(byte, byte, byte)
{
    return 0;
}

void sb_done() {}
void sb_enableDMA() {}
void sb_stopDMA() {}
void sb_continueDMA() {}
void sb_ackDMA() {}

char sbmix_setvol(byte dev, unsigned data)
{
    if (dev >= MIXER_DEVICE_NUM)
        return 0;
    SBdefault[dev] = data;
    return 1;
}
