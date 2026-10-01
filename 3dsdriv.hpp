#pragma once
 






 
#define	SBT_SB1		1
#define	SBT_SBPRO	3
#define	SBT_SB16	4

 
#define MIXER_MASTER		0
#define MIXER_BASS			1
#define MIXER_TREBLE		2
#define MIXER_MIDI 			3
#define MIXER_VOICE			4
#define MIXER_SPEAKER		5
#define MIXER_LINE			6
#define MIXER_MIC			7
#define MIXER_CD			8
#define MIXER_IMIX			9
#define MIXER_ALTPCM		10
#define MIXER_AGC			11
#define MIXER_OUTPUTGAIN	12
#define MIXER_INPUTGAIN		13	 
#define MIXER_DEVICE_NUM  	(MIXER_INPUTGAIN+1)

extern unsigned SBdefault[MIXER_DEVICE_NUM];

extern int sb_ioaddr;
extern byte sb_DMA8ch,		 
			sb_DMA16ch,		 
			sb_IRQnum;
extern byte sb_type;
extern word DMAbuffer_size;
extern byte far *DMAbuffer, *DMAbufhalf;
extern byte Old8259Mask;

extern void sb_ackDMA();

extern char sb_init(byte dma8, byte dma16, byte irq);
extern void sb_done();
extern void sb_enableDMA();
extern void sb_stopDMA();
extern void sb_continueDMA();
extern char sbmix_setvol(byte dev, unsigned data);