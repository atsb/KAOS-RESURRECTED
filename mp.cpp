 






#include "std.hpp"
#include "platform_compat.hpp"
#include <fcntl.h>
#include "platform_compat.hpp"

#include "platform_compat.hpp"

#define	MAXTRACKS	24
#define MAXCHANNELS	10
 


























 
#define	alFMStatus		0x388	 
#define	alFMAddr		0x388	 
#define	alFMData		0x389	 

 
#define alTimer1		2
#define	alTimer2		3
#define	alTimerControl	4
 
#define	alAVEKM			0x20
#define	alKOutLev		0x40
#define	alAttDec		0x60
#define	alSusRel		0x80
#define	alWave			0xe0
 
#define	alFreqL			0xa0
#define	alFreqH			0xb0
#define	alFeedCon		0xc0
 
#define	alAMVDR			0xbd	 

 
#define	TCC_STARTT1		0x21
#define	TCC_STARTT2		0x42
#define	TCC_IRQRESET	0x80
#define TCC_HALT		0xe0
#define TCC_STOP		0x00
#define TCC_CONTINUET1	0x01
#define TCC_CONTINUET2	0x02
#define	TCC_RESET		0x60
 








 
#define	TCM_IRQ			0x80
#define	TCM_IRQT1		0xc0
#define	TCM_IRQT2		0xa0

#define	ULONG		unsigned long
#define BOOL		char
#define FALSE		0
#define TRUE		1

 
#define	status()			inportb(alFMStatus)
#define	MakeFirm(a,b,c,d)	((ULONG)a+((ULONG)b<<8)+((ULONG)c<<16)+((ULONG)d<<24))

const ULONG	MThd = MakeFirm('M','T','h','d'),
			MTrk = MakeFirm('M','T','r','k');

typedef	struct {
			byte 	mAVEKM,cAVEKM,
					mKOutLev,cKOutLev,
					mAttDec,cAttDec,
					mSusRel,cSusRel,
					mWave,cWave,
					nFeedCon,
					unused[5];	 
		} TInstrument;

typedef struct {
			 
			long 		curpos;
			long		wait;
			byte		oldevent;
			char		oldmeta;
		} TTrack;

BOOL mp_active = FALSE,
	mp_playing = FALSE,
	mp_changetempo = FALSE;
int mp_handle = -1,
	mp_numtracks = 0;

long mp_totalwait = 0l,
	mp_waitdown = 0l,
	mp_timeslice = 0l,
	mp_tickperquarter = 1l,
	mp_microperquarter = 500000l,
	mp_micropertick = 1l;	 
void interrupt (*mp_oldtimer)(...);

 
static	byte 	modulators[9] = { 0, 1, 2, 8, 9,10,16,17,18},
				carriers[9]  = { 3, 4, 5,11,12,13,19,20,21},
 
				pmodulators[5] = {16,  17,  18,  20,  21},
				pcarriers[5]  = {19,0xff,0xff,0xff,0xff};
word fnums[12] = {363,385,408,432,458,485,514,544,577,611,647,686};
TTrack track[MAXTRACKS];

const TInstrument Instrument[10] = {
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
}}

char mp_DetectAdLib() {
	byte	status1,status2;
	int		i;

	al_out(alTimerControl,0x60);	 
	al_out(alTimerControl,0x80);	 
	status1 = status();
	al_out(alTimer1,0xff);		 
	al_out(alTimerControl,0x21);	 
	asm	{
		MOV	DX, 0x388
		MOV CX,	100
	} waitloop: asm {
		IN AL, DX
		LOOP waitloop
	}
	status2 = status();
	al_out(alTimerControl,0x60);	 
	al_out(alTimerControl,0x80);	 
	if ((!(status1 & 0xe0)) && ((status2 & 0xe0) == 0xc0)) {
		for (i=1;i<=0xf5;i++)	 
			al_out(i,0);
		al_out(1,0x20);		 
		al_out(8,0);		 
		return 1;
	}
	return 0;
}

 

































void mp_setinstrument(byte channel, TInstrument &inst) {
	byte c, m;

	if (channel >= MAXCHANNELS)
		...channel -= 9;
	if (percussive) {
		c = pcarriers[channel];
		m = pmodulators[channel];
	} else {
		c = carriers[channel];
		m = modulators[channel];
	  }

	al_out(alAVEKM  +m,inst.mAVEKM);
	al_out(alKOutLev+m,inst.mKOutLev);
	al_out(alAttDec +m,inst.mAttDec);
	al_out(alSusRel +m,inst.mSusRel);
	al_out(alWave   +m,inst.mWave);

	 
	if (c != 0xff) {
		al_out(alAVEKM  +c,inst.cAVEKM);
		al_out(alKOutLev+c,inst.cKOutLev);
		al_out(alAttDec +c,inst.cAttDec);
		al_out(alSusRel +c,inst.cSusRel);
		al_out(alWave   +c,inst.cWave);
	}
	al_out(alFeedCon+channel,inst.nFeedCon);	 
}

void mp_NoteOn(byte channel, byte octave, byte note) {
	word note2;
	byte bl,bh;

	note2 = fnums[note];
	bl = (byte)(note2 & 0x00ff);
	bh = (byte)(note2 >> 8)+(octave << 2) | 0x20;
	al_out(alFreqL + channel,bl);
	al_out(alFreqH + channel,bh);
}

void mp_NoteOff(byte channel, byte octave, byte note) {
	word note2;
	byte bl,bh;

	note2 = fnums[note];
	bl = (byte)(note2 & 0x00ff);
	bh = (byte)(note2 >> 8)+(octave << 2);
	al_out(alFreqL + channel,bl);
	al_out(alFreqH + channel,bh);
}

void mp_MusicOn(void) {
	if (mp_handle >= 0) {
		al_out(alAMVDR,0);
		mp_waitdown = 0l;
		mp_totalwait = 0l;
		mp_timeslice = 0l;
		mp_playing = TRUE;
		al_out(alTimerControl,0x60);	 
		al_out(alTimerControl,0x80);	 
		al_out(alTimer1,0xff);			 
		al_out(alTimerControl,TCC_STARTT1);	 
	}
}

void mp_MusicOff(void) {
	if (mp_playing) {
		al_out(alAMVDR,0);
		for (int i=0; i<MAXCHANNELS; i++)
			al_out(alFreqH + i + 1,0);

		al_out(alTimerControl,TCC_RESET);
		al_out(alTimerControl,TCC_IRQRESET);
		mp_playing = FALSE;
		mp_changetempo = FALSE;
		mp_totalwait = 0l;
		mp_waitdown = 0l;
		mp_timeslice = 0l;
	}
	if (mp_handle >= 0) {
		close(mp_handle);
		mp_handle = -1;
	}
}

#pragma warn -rvl
ULONG adjustulong(ULONG num) {
asm {
	MOV DX, WORD PTR num
	MOV AX, WORD PTR num[2]
	XCHG AH, AL
	XCHG DH, DL
} }

ULONG adjusttribyte(ULONG num) {
asm {
	MOV AL, BYTE PTR num[2]
	MOV AH, BYTE PTR num[1]
	MOV DL, BYTE PTR num
	XOR DH, DH
} }

unsigned adjustint(unsigned num) {
asm {
	MOV AH, BYTE PTR num
	MOV AL, BYTE PTR num[1]
} }
#pragma warn +rvl

BOOL findfirm(const ULONG realfirm) {
	ULONG firm, data = 0l;

	_read(mp_handle,&firm,4);
	while (firm != realfirm && !eof(mp_handle)) {
		_read(mp_handle,&data,1);	 
		firm = (firm<<8) | data;
	}
	return (firm == realfirm);
}

void play_note(int channel, byte num, byte spd) {
	byte note = num % 12,
		oct = num / 12,
		ch;
	 



	ch = channel;
	if (!spd) {
		mp_NoteOff(ch,oct,note);
	} else {
		mp_NoteOn(ch,oct,note);
	  }
}

long handle_trackevents(TTrack &curtrack) {
	long totdelay;
	byte channel,event,
		bdat,bdat2;
	BOOL meta;
	int i;

	 
	bdat = 0l;
	_read(mp_handle,&bdat,1);
	curtrack.curpos++;
	totdelay = bdat & 0x7f;
	for (i=3; i--; )
		if (bdat & 0x80) {
			_read(mp_handle,&bdat,1);
			curtrack.curpos++;
			totdelay = (totdelay<<7) + (bdat & 0x7f);
		} else break;
	 
	_read(mp_handle,&event,1);
	curtrack.curpos++;
	if (!(event & 0x80)) {	 
		bdat = event;
		meta = curtrack.oldmeta;
		event = curtrack.oldevent;
	} else {
		if ((meta = (event == 0xff)) != 0) {	 
			_read(mp_handle,&event,1);
			curtrack.curpos++;
		}
		curtrack.oldmeta = meta;
		curtrack.oldevent = event;
		if ((event & 0xf0) != 0xf0)	{  
			_read(mp_handle,&bdat,1);
			curtrack.curpos++;
		}
	  }
	if (!meta) {
		channel = event & 0x0f;
		switch(event & 0xf0) {
		case 0x80:	 
			_read(mp_handle,&bdat2,1);
			curtrack.curpos++;
			play_note(channel,bdat,0);
			break;
		case 0x90:	 
			_read(mp_handle,&bdat2,1);
			curtrack.curpos++;
			play_note(channel,bdat,bdat2);
			break;
		case 0xa0:	 
			_read(mp_handle,&bdat2,1);
			 
			curtrack.curpos++;
			 
			break;
		case 0xb0:	 
			_read(mp_handle,&bdat2,1);
			 
			curtrack.curpos++;
			break;
		case 0xc0:
			 
			break;
		case 0xd0:
			 
			break;
		case 0xe0:
			_read(mp_handle,&bdat2,1);
			curtrack.curpos++;
			 
			break;
		case 0xf0:	 
			 















			break;
		default:
			 
			break;
		}
	} else {	 
		 
		switch(event) {
			case 0x2f:	 
				 
				curtrack.curpos = -1;
				break;
			case 0x51:	 
				mp_microperquarter = 0l;
				_read(mp_handle,&mp_microperquarter,bdat);
				curtrack.curpos += bdat;
				if (bdat==3) mp_microperquarter = adjusttribyte(mp_microperquarter); else
				 if (bdat==4) mp_microperquarter = adjustulong(mp_microperquarter); else
				  if (bdat==2) mp_microperquarter = adjustint(mp_microperquarter);
				mp_changetempo = 1;
				break;
			case 0x58:
				 
				for (i=0;i<bdat;i++) _read(mp_handle,&bdat2,1);
				curtrack.curpos += bdat;
				break;
			case 0x59:
				 
				for (i=0;i<bdat;i++) _read(mp_handle,&bdat2,1);
				curtrack.curpos += bdat;
				break;
			case 0x7f:
				 
				for (i=0;i<bdat;i++) _read(mp_handle,&bdat2,1);
				curtrack.curpos += bdat;
				break;
			default:
				 
				for (i=0;i<bdat;i++) _read(mp_handle,&bdat2,1);
				curtrack.curpos += bdat;
			}
	  }
	if (curtrack.curpos>=0)
		curtrack.curpos = tell(mp_handle);
	return totdelay;
}

void mp_scantracks() {
	BOOL dontstop = FALSE;
	long mindelay,  
		totalmicro;
	int i;

	 
	totalmicro = mp_totalwait*80;  

	 
	 
	mindelay = 0x7fffffffl;
	for (i=0; i<mp_numtracks; i++) {
		TTrack &curtrack = track[i];
		if (curtrack.curpos >= 0) {
			if ((curtrack.wait -= totalmicro) <= 0) {
				lseek(mp_handle,curtrack.curpos,SEEK_SET);
				while (!kbhit() && curtrack.wait <= 0 && curtrack.curpos >= 0)
					curtrack.wait += handle_trackevents(curtrack)*mp_micropertick;
			}
			if (curtrack.curpos >= 0) {
				dontstop = TRUE;
				if (curtrack.wait<mindelay)
					mindelay = curtrack.wait;
			}
		}
	}
	mp_totalwait = 0l;

	if (dontstop) {
		mp_waitdown = (mindelay+79)/80; 
		cprintf("waitdown = %ld\r\n",mp_waitdown);
		if (mp_waitdown<=0)	mp_waitdown=1;	 
			 
		if (mp_changetempo) {
			mp_micropertick = mp_microperquarter*50/mp_tickperquarter;
			 
			cprintf("microsec per tick = %ld\r\n",mp_micropertick);
			mp_changetempo=0;
		}
	} else {
		mp_MusicOff();
		 
	  }
}

void mp_StartMusic(const char *filename) {
	ULONG data;
	int i;

	if (!mp_active) return;
	mp_MusicOff();
 
 
	if ((mp_handle = _open(filename,O_RDONLY|O_BINARY)) >= 0) {
		if (findfirm(MThd)) {
			_read(mp_handle,&data,4);
			data = adjustulong(data);
			if (data == 6) {	 
				data = 0l;
				_read(mp_handle,&data,2);
				data = adjustint(data);
				if (data <= 1) {	 
					data = 0l;
					_read(mp_handle,&data,2);
					mp_numtracks = adjustint(data);
					data = 0l;
					_read(mp_handle,&data,2);
					mp_tickperquarter = adjustint(data);
					for(i=1; i<=mp_numtracks; i++) {
						if (findfirm(MTrk)) {
							_read(mp_handle,&data,4);
							data = adjustulong(data);
							track[i].curpos = tell(mp_handle);
							track[i].wait = 0l;
							lseek(mp_handle,data,SEEK_CUR);
						} else error("main","no more tracks");
					}
					for (i=mp_numtracks; i<MAXTRACKS; i++)
						track[i].curpos = -1;
				} else error("main","no synchronized tracks");
			} else error("main","non standard 6 bytes header");
		} else error("main","no file header");
		 
		 
		mp_scantracks();
		mp_MusicOn();
	}
 
}

void mp_StopMusic() {
	mp_MusicOff();
}

void interrupt mp_newtimer(...) {
	if (status() & TCM_IRQ) {
		 

















		outportb(0x20,0x20);
	} else
		mp_oldtimer();
}

void mp_idle() {
	if ((status() & TCM_IRQ) != 0) {
		al_out(alTimerControl,TCC_RESET);
		al_out(alTimerControl,TCC_IRQRESET);
		 
		mp_totalwait += mp_timeslice;
		if ((mp_waitdown -= mp_timeslice) <= 0)
			mp_scantracks();
		if (mp_playing) {
			if (mp_waitdown >= 256l) {
				mp_timeslice = 256l;
				al_out(alTimer1,0);
			} else {
				al_out(alTimer1,(byte)(256l-(mp_timeslice = mp_waitdown)));
			}
			al_out(alTimerControl,TCC_CONTINUET1);
			 
		}  
	}
}

void mp_init() {
	if ((mp_active = mp_DetectAdLib()) != 0) {
		mp_playing = FALSE;
		 
		mp_oldtimer = getvect(0x08);
		setvect(0x08,mp_newtimer);
		for (int i=0; i<MAXCHANNELS; i++)
			mp_setinstrument(i,Instrument[3],FALSE);
		al_out(alAVEKM,0);
		 









	}
}

void mp_done() {
	if (mp_active) {
		mp_MusicOff();
		 
		setvect(0x08,mp_oldtimer);
	}
}