 






#define	MAXTRACKS	10

 
#define	sbReset			0x06	 
#define	sbFMStatus		0x08	 
#define	sbFMAddr		0x08	 
#define	sbFMData		0x09	 
#define	sbReadData		0x0a	 
#define	sbWriteCmd		0x0c	 
#define	sbWriteData		0x0c	 
#define	sbWriteStat		0x0c	 
#define	sbDataAvail		0x0e	 

 
#define	sbpLFMStatus	0x00	 
#define	sbpLFMAddr		0x00	 
#define	sbpLFMData		0x01	 
#define	sbpRFMStatus	0x02	 
#define	sbpRFMAddr		0x02	 
#define	sbpRFMData		0x03	 
#define	sbpMixerAddr	0x04	 
#define	sbpMixerData	0x05	 
#define	sbpCDData		0x10	 
#define	sbpCDCommand	0x10	 
#define	sbpCDStatus		0x11	 
#define	sbpCDReset		0x12	 

 
#define	alFMStatus		0x388	 
#define	alFMAddr		0x388	 
#define	alFMData		0x389	 

 
 
#define	alAVEKM			0x20
#define	alKOutLev		0x40
#define	alAttDec		0x60
#define	alSusRel		0x80
#define	alWave			0xe0
 
#define	alFreqL			0xa0
#define	alFreqH			0xb0
#define	alFeedCon		0xc0
 
#define	alAMVDR			0xbd

typedef	struct {
			byte 	mAVEKM,cAVEKM,
					mKOutLev,cKOutLev,
					mAttDec,cAttDec,
					mSusRel,cSusRel,
					mWave,cWave,
					nFeedCon
					 
		} TInstrument;

typedef struct {
			TInstrument	*inst;
			char		percussive;
			word		far *seq;
			longword	nextevent;
		} TTrack;

byte mp_StatusReg();
 









void mp_InitCard();
 

void mp_SetTimer1(byte count);
 





void mp_SetTimer2(byte count);
 

void mp_TimerControl(byte control);
 






















void mp_CSMMode_KbdSplit(byte mks);
 










































void mp_AM_VIB_EG_KSR_multiple(byte instnum, byte modulator, byte carrier);
 















































void mp_KSL_TotalLevel(byte instnum, byte modulator, byte carrier);
 
































void mp_Attack_Decay(byte instnum, byte modulator, byte carrier);
 










void mp_Sustain_Release(byte instnum, byte modulator, byte carrier);
 
















void mp_Block_FNum(byte instnum, byte modulator, byte carrier);
 











































void mp_AMVIBDepth_Rhythm(byte data);
 






























void mp_WaveSelect(byte instnum, byte modulator, byte carrier);
 



























void mp_Feedback_Connect(byte instnum, byte data);
 

























void mp_LoadInst(byte instnum, TInstrument &instdata);
 



void mp_NoteOn(byte instnum, byte octave, byte note);
 



void mp_NoteOff(byte instnum, byte octave, byte note);
 



 

#include "std.hpp"

char mp_mactive = 0,
	mp_mplaying = 0;
int mp_mmemidx = -1;
word mp_mcurpos = 0;

 
static	byte			carriers[9]  = { 3, 4, 5,11,12,13,19,20,21},
						modifiers[9] = { 0, 1, 2, 8, 9,10,16,17,18},
 
						pcarriers[5]  = {19,0xff,0xff,0xff,0xff},
						pmodifiers[5] = {16,  17,  18,  20,  21};
byte choffs[9] = {0x01,0x02,0x03,0x09,0x0a,0x0b,0x11,0x12,0x13};
word fnums[12] = {363,385,408,432,458,485,514,544,577,611,647,686};

const TInstrument Inst[10] = {
{0x21,0x11,0x4C,0x00,0xF1,0xF2,0x63,0x72,          
 0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00},
{0xA5,0xB1,0xD2,0x80,0x81,0xF1,0x03,0x05,          
 0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00},
{0x72,0x62,0x1C,0x05,0x51,0x52,0x03,0x13,          
 0x00,0x00,0x0E,0x00,0x00,0x00,0x00,0x00},
{0x11,0x01,0x8A,0x40,0xF1,0xF1,0x11,0xB3,          
 0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00},
{0x21,0x11,0x11,0x00,0xA3,0xC4,0x43,0x22,          
 0x02,0x00,0x0D,0x00,0x00,0x00,0x00,0x00},
{0x31,0xA1,0x1C,0x80,0x41,0x92,0x0B,0x3B,          
 0x00,0x00,0x0E,0x00,0x00,0x00,0x00,0x00},
{0x71,0x62,0xC5,0x05,0x6E,0x8B,0x17,0x0E,          
 0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00},
{0x41,0x91,0x83,0x00,0x65,0x32,0x05,0x74,          
 0x00,0x00,0x0A,0x00,0x00,0x00,0x00,0x00},
{0x32,0x16,0x87,0x80,0xA1,0x7D,0x10,0x33,          
 0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00},
{0x01,0x13,0x8D,0x00,0x51,0x52,0x53,0x7C,          
 0x01,0x00,0x0C,0x00,0x00,0x00,0x00,0x00}};

void al_out(byte port, byte data) {
asm	{
	 
	 
	MOV	DX,	0x388
	MOV	AL, port
	OUT	DX, AL
	IN	AL, DX
	IN	AL, DX
	IN	AL, DX
	IN	AL, DX
	IN	AL, DX
	IN	AL, DX
	INC	DX
	MOV	AL, data
	OUT	DX, AL
	 
	DEC	DX

	MOV CX, 35	 
} c1: asm {
	IN AL, DX
	LOOP c1
}

byte mp_StatusReg() {
	return inportb(0x388);
}

void mp_InitCard() {
   
	al_out(1,0);
   
   
   
}

void mp_SetTimer1(byte count) {
	al_out(2,count);
}

void mp_SetTimer2(byte count) {
	al_out(3,count);
}

void mp_TimerControl(byte control) {
	al_out(4,control);
}

void mp_CSMMode_KbdSplit(byte mks) {
	al_out(8,mks);
}

void mp_AM_VIB_EG_KSR_multiple(byte instnum, byte modulator, byte carrier) {
	al_out(choffs[instnum]+0x1f,modulator);
	al_out(choffs[instnum]+0x1f+3,carrier);
}

void mp_KSL_TotalLevel(byte instnum, byte modulator, byte carrier) {
	al_out(choffs[instnum]+0x3f,modulator);
	al_out(choffs[instnum]+0x3f+3,carrier);
}

void mp_Attack_Decay(byte instnum, byte modulator, byte carrier) {
	al_out(choffs[instnum]+0x5f,modulator);
	al_out(choffs[instnum]+0x5f+3,carrier);
}

void mp_Sustain_Release(byte instnum, byte modulator, byte carrier) {
	al_out(choffs[instnum]+0x7f,modulator);
	al_out(choffs[instnum]+0x7f+3,carrier);
}

void mp_Block_FNum(byte instnum, byte modulator, byte carrier) {
	al_out(instnum+0x9f,modulator);
	al_out(instnum+0xaf,carrier);
}

void mp_AMVIBDepth_Rhythm(byte data) {
	al_out(0xbd,data);
}

void mp_WaveSelect(byte instnum, byte modulator, byte carrier) {
	al_out(choffs[instnum]+0xdf,modulator);
	al_out(choffs[instnum]+0xdf+3,carrier);
}

void mp_Feedback_Connect(byte instnum, byte data) {
	al_out(instnum+0xbf,data);
}

void mp_LoadInst(byte instnum, TInstrument &instdata) {
	mp_AM_VIB_EG_KSR_Multiple(instnum,instdata[0],instdata[1]);
	mp_KSL_TotalLevel(instnum,instdata[2],instdata[3]);
	mp_Attack_Decay(instnum,instdata[4],instdata[5]);
	mp_Sustain_Release(instnum,instdata[6],instdata[7]);
	mp_WaveSelect(instnum,instdata[8],instdata[9]);
	mp_Feedback_Connect(instnum,instdata[10]);
}

void mp_NoteOn(byte instnum, byte octave, byte note) {
	word note2;
	byte b1,b2;

	note2 = fnums[note];
	b2 = (byte)(note2 >> 8)+0x20+(octave << 2);
	b1 = (byte)(note2 & 0x00ff);
	mp_Block_FNum(instnum,b1,b2);
}

void mp_NoteOff(byte instnum, byte octave, byte note) {
	word note2;
	byte b1,b2;

	note2 = fnums[note];
	b2 = (byte)(note2 >> 8)+(octave << 2);
	b1 = (byte)(note2 & 0x00ff);
	mp_Block_FNum(instnum,b1,b2);
}

void mp_setinstrument(int tracknum, int chnum, TInstrument *inst,
					  char percussive) {
	byte c, m;
	if (percussive) {
		c = pcarriers[chnum];
		m = pmodifiers[chnum];
	} else {
		c = carriers[chnum];
		m = modifiers[chnum];
	  }

	tracks[track-1]->inst = *inst;
	tracks[track-1]->percussive = percussive;

	alOut(m + alChar,inst->mChar);
	alOut(m + alScale,inst->mScale);
	alOut(m + alAttack,inst->mAttack);
	alOut(m + alSus,inst->mSus);
	alOut(m + alWave,inst->mWave);

	 
	if (c != 0xff) {
		alOut(c + alChar,inst->cChar);
		alOut(c + alScale,inst->cScale);
		alOut(c + alAttack,inst->cAttack);
		alOut(c + alSus,inst->cSus);
		alOut(c + alWave,inst->cWave);
	}

	alOut(chnum + alFeedCon,inst->nFeedCon);	 
}

void mp_MusicOn(void) {
	mp_mactive = 1;
}

void mp_MusicOff(void) {
{
	word i; !!!

	alFXReg = 0;
	alOut(alEffects,0);
	for (i = 0;i < sqMaxTracks;i++)
		alOut(alFreqH + i + 1,0);
	sm_mactive = 0;
}

void mp_StartMusic(MusicGroup far *music) {
	mp_MusicOff();
 
 
	sqHackPtr = sqHack = music->values;
	sqHackSeqLen = sqHackLen = music->length;
	sqHackTime = 0;
	alTimeCount = 0;
	mp_MusicOn();
 
}