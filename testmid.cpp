 






#include "std.hpp"
#include "platform_compat.hpp"
#include <cstdint>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "platform_compat.hpp"
#include "platform_compat.hpp"

using midi_u32 = std::uint32_t;
#define	MakeFirm(a,b,c,d)	((midi_u32)(a)+((midi_u32)(b)<<8)+((midi_u32)(c)<<16)+((midi_u32)(d)<<24))

const midi_u32	MThd = MakeFirm('M','T','h','d'),
			MTrk = MakeFirm('M','T','r','k');

const int Freq4[12] = {277,294,311,330,349,370,392,415,440,466,494,523};

midi_u32 adjustulong(midi_u32 num) {
	return ((num & 0x000000ffu) << 24) | ((num & 0x0000ff00u) << 8) |
	       ((num & 0x00ff0000u) >> 8) | ((num & 0xff000000u) >> 24);
}

midi_u32 adjusttribyte(midi_u32 num) {
	return ((num & 0x000000ffu) << 16) | (num & 0x0000ff00u) |
	       ((num & 0x00ff0000u) >> 16);
}

unsigned adjustint(unsigned num) {
	return ((num & 0xffu) << 8) | ((num >> 8) & 0xffu);
}

char findfirm(int handle, const midi_u32 realfirm) {
	midi_u32 firm, data = 0u;

	_read(handle,&firm,4);
	while (firm != realfirm && !eof(handle)) {
		_read(handle,&data,1);	 
		firm = (firm<<8) | data;
	}
	return (firm == realfirm);
}

void play(int note, int vel) {
	if (!vel)
		nosound();
	else {
		int oct = note/12;
		if (oct==4)
			sound(Freq4[note % 12], 0);
		else
		if (oct>4)
			sound(Freq4[note % 12]*(oct-4), 0);
		else
			sound(Freq4[note % 12]/(4-oct), 0);
	}
}

void parsetrack(int handle) {
	midi_u32 data;
	int i;
	byte event,oldevent,
		 meta, oldmeta,
		 channel;
	midi_u32 totdelay;
	byte bdat,bdat2;
	char car = 0, end = 0;

	_read(handle,&data,4);
	data = adjustulong(data);
	printf("  track size: %lu\n",data);
	do {
		totdelay=0l;
		 
		_read(handle,&bdat,1);
		totdelay = bdat & 0x7f;
		for (i=3;i--;)
			if (bdat & 0x80) {
				_read(handle,&bdat,1);
				totdelay = (totdelay<<7) + (bdat & 0x7f);
			} else break;
		printf("  - delay: %6lu ",totdelay);
		 
		_read(handle,&event,1);
		if (!(event & 0x80)) {
			printf("*");
			bdat = event;
			meta = oldmeta;
			event = oldevent;
		} else {
			printf(" ");
			if ((meta = (event == 0xff)) != 0)
				_read(handle,&event,1);
			oldmeta = meta;
			oldevent = event;
			if ((event & 0xf0) != 0xf0) _read(handle,&bdat,1);
		  }
		if (!meta) {
			channel = event & 0x0f;
			printf("  e: ch(%2d) ",channel);
			switch(event & 0xf0) {
			case 0x80:
				_read(handle,&bdat2,1);
				printf("note %d off (vel.%d)\n",bdat,bdat2);
				play(bdat,0);
				break;
			case 0x90:
				_read(handle,&bdat2,1);
				printf("note %d on (vel.%d)\n",bdat,bdat2);
				play(bdat,bdat2);
				break;
			case 0xa0:
				_read(handle,&bdat2,1);
				printf("note %d after-touch (vel.%d)\n",bdat,bdat2);
				play(bdat,bdat2);
				break;
			case 0xb0:
				_read(handle,&bdat2,1);
				printf("controller %d change to %d\n",bdat,bdat2);
				break;
			case 0xc0:
				printf("program change to %d\n",bdat);
				break;
			case 0xd0:
				printf("channel after-touch to %d\n",bdat);
				break;
			case 0xe0:
				_read(handle,&bdat2,1);
				printf("pitch wheel change of %d\n",(int)bdat+((int)bdat2<<7));
				break;
			case 0xf0:	 
				switch(event) {
				case 0xf8:
					printf("time sync\n");
					break;
				case 0xfa:
					printf("start sequence\n");
					break;
				case 0xfb:
					printf("continue sequence\n");
					break;
				case 0xfc:
					printf("stop sequence\n");
					break;
				}
				break;
			default:
				printf("???\n");
				break;
			}
		} else {
			printf(" me: ");
			switch(event) {
			case 0x2f:	 
				printf("end of track\n");
				end++;
				break;
			case 0x51:
				totdelay = 0l;
				_read(handle,&totdelay,bdat);
				if (bdat==3) totdelay = adjusttribyte(totdelay); else
				 if (bdat==4) totdelay = adjustulong(totdelay); else
				  if (bdat==2) totdelay = adjustint(totdelay);
				printf("set tempo: (%d) %lu\n",bdat,totdelay);
				break;
			case 0x58:
				printf("time signature: (%d) ...\n",bdat);
				for (i=0;i<bdat;i++) _read(handle,&bdat2,1);
				break;
			case 0x59:
				printf("key signature: (%d) ...\n",bdat);
				for (i=0;i<bdat;i++) _read(handle,&bdat2,1);
				break;
			case 0x7f:
				printf("sequencer info: (%d) ...\n",bdat);
				for (i=0;i<bdat;i++) _read(handle,&bdat2,1);
				break;
			default:
				printf("%u\n",event);
				for (i=0;i<bdat;i++) _read(handle,&bdat2,1);
			}
		}
		if (car != 'q') {
			car = getch();
			if (car == 27) {
				_close(handle);
				nosound();
				exit(0);
			}
		}
	} while (!end);
	printf(" ----\n");
	nosound();
}

int main(int argc, char **argv) {
	int handle, i, numtracks;
	midi_u32 data;

	const char *filename = argc > 1 ? argv[1] : "c:\\music\\mid\\furelise2.mid";
	if ((handle = _open(filename,O_RDONLY|O_BINARY)) >= 0) {
		 
		if (findfirm(handle,MThd)) {
			printf("MThd\n");
			_read(handle,&data,4);
			data = adjustulong(data);
			if (data == 6) {
				data = 0l;
				_read(handle,&data,2);
				data = adjustint(data);
				if (data <= 1) {
					_read(handle,&numtracks,2);
					numtracks = adjustint(numtracks);
					printf("Number of tracks: %d\n",numtracks);
					_read(handle,&data,2);
					data = adjustint(data);
					printf("Delta-time/tick : %u\n",data);
					for(i=1;i<=numtracks;i++) {
						if (findfirm(handle,MTrk)) {
							printf("MTrk #%d\n",i);
							parsetrack(handle);
						} else error("main","no more tracks");
					}
				} else error("main","no synchronized tracks");
			} else error("main","non-standard 6 bytes header");
		} else error("main","no file header");
		_close(handle);
	} else error("main","file not found");
	return 0;
}