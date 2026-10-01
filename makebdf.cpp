 






 

#include "std.hpp"
#include "errorstr.hpp"
#include "xms.hpp"
#include "mm4.hpp"
#include "fastmem.hpp"
#include "fixed.h"
#include "lgraph.hpp"
#include "lfont2.hpp"
#include "kaos.hpp"
#include "strlist.hpp"
#include "crcio.hpp"
#include "platform_compat.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <SDL3/SDL.h>
#include <cstdint>
#include <cstddef>

#define FIRMdata	((unsigned long)'d'+((unsigned long)'a'<<8)+((unsigned long)'t'<<16)+((unsigned long)'a'<<24))

typedef char *PPal;
int none[20];



void loadsound(int sndnum, const char *wavname)
{
	if (sndnum < 0 || sndnum >= 53 || !wavname)
		return;

	SDL_AudioSpec spec{};
	Uint8 *data = nullptr;
	Uint32 length = 0;
	if (!SDL_LoadWAV(wavname, &spec, &data, &length)) {
		error("loadsound", wavname);
		return;
	}

	const int channels = static_cast<int>(spec.channels);
	const int bytes_per_sample = SDL_AUDIO_BYTESIZE(spec.format);
	if (channels <= 0 || bytes_per_sample <= 0) {
		SDL_free(data);
		error("loadsound", "unsupported WAV format");
		return;
	}

	const std::size_t frame_bytes = static_cast<std::size_t>(channels) *
		static_cast<std::size_t>(bytes_per_sample);
	const std::size_t frames = length / frame_bytes;
	if (!frames || frames > 65534u) {
		SDL_free(data);
		error("loadsound", "sample too large for DOS BDF");
		return;
	}

	const int id = mm_alloc(static_cast<word>(frames));
	byte *dst = static_cast<byte *>(mm_recall(id));
	if (!dst) {
		SDL_free(data);
		error("loadsound", "out of memory");
		return;
	}

	for (std::size_t i = 0; i < frames; ++i) {
		const Uint8 *frame = data + i * frame_bytes;
		long sample = 0;
		for (int ch = 0; ch < channels; ++ch) {
			const Uint8 *src = frame + ch * bytes_per_sample;
			long v;
			if (spec.format == SDL_AUDIO_U8) {
				v = static_cast<long>(src[0]) - 128L;
			} else if (spec.format == SDL_AUDIO_S16LE) {
				const std::int16_t v16 = static_cast<std::int16_t>(
					static_cast<std::uint16_t>(src[0]) |
					(static_cast<std::uint16_t>(src[1]) << 8));
				v = static_cast<long>(v16) >> 8;
			} else if (spec.format == SDL_AUDIO_S16BE) {
				const std::int16_t v16 = static_cast<std::int16_t>(
					(static_cast<std::uint16_t>(src[0]) << 8) |
					static_cast<std::uint16_t>(src[1]));
				v = static_cast<long>(v16) >> 8;
			} else {
				v = static_cast<long>(static_cast<signed char>(
					src[bytes_per_sample - 1]));
			}
			sample += v;
		}
		dst[i] = static_cast<byte>(static_cast<std::int8_t>(sample / channels));
	}

	mm_unlock(id);
	SDL_free(data);
}























byte ChangeWallText[MAXTEXTURES] = {
T_WALL1S 		,T_WALL1MS ,
T_WALL1SS 	,T_WALL1MSS ,
T_WALL1SS 	,T_WALL1MSS ,
T_TAPS 			,T_TAPMS ,
T_TAPSS 		,T_TAPMSS ,
T_TAPSS 		,T_TAPMSS ,
T_SCUDOS 		,T_SCUDOMS ,
T_SCUDOSS 	,T_SCUDOMSS ,
T_SCUDOSS 	,T_SCUDOMSS ,
T_ARC 			,T_ARCM ,
T_PIETR1S 	,T_PIETR1MS ,
T_PIETR1SS 	,T_PIETR1MSS ,
T_PIETR1SS ,T_PIETR1MSS ,
T_ASCES 		,T_ASCEMS ,
T_ASCESS 		,T_ASCEMSS ,
T_ASCESS 	,T_ASCEMSS ,
T_ROCKS 		,T_ROCKSS ,
T_ROCKSS 	,
T_WOOD1S 		,T_WOOD1SS ,
T_WOOD1SS 	,
T_WBOXS 		,T_WBOXSS ,
T_WBOXSS 	,
T_DOOR1S 		,T_DOOR1S ,
T_DOOR2S 		,T_DOOR2S ,
T_DOORYS 		,T_DOORYS ,
T_DOORRS 		,T_DOORRS ,
T_WOOD2S 		,T_WOOD2SS ,
T_WOOD2SS 	,
T_SBARRES 	,T_SBARRES ,
T_SWITCHU 	,

T_WALL1Z,T_WALL1MZ,T_PIETR1Z,T_PIETR1MZ,T_ROCKZ,T_WOOD1Z,T_WBOXZ,T_WOOD2Z,
T_WALL2Z 		,T_WALL2MZ ,
T_WALL2Z 	,T_WALL2MZ ,
T_PIETR2Z 	,T_PIETR2MZ ,
T_PIETR2Z 	,T_PIETR2MZ ,
T_EARTHZ 		,T_EARTHZ ,
T_SOFTZ 		,T_SOFTMZ ,
T_SOFTZ 		,T_SOFTMZ ,

T_DRAGON  ,T_DRAGON+1,T_DRAGON+2,T_DRAGON+3,T_DRAGON+4,
T_DRAGON+5,T_DRAGON+6,T_DRAGON+7,T_DRAGON+8,

T_GRATAWZ 	,T_GRATAWZ ,
T_GRATABZ 	,T_GRATABZ ,
T_FLROCKZ 	,T_FLROCKZ ,
 
T_OVERDOOR,T_DOORSLOT,T_SWITCHD
	};

byte ChangeFloorText[MAXTEXTURES] = {
T_WALL1Z 		,T_WALL1MZ ,
T_WALL1SS 	,T_WALL1MSS ,
T_WALL1SS 	,T_WALL1MSS ,
T_TAPS 			,T_TAPMS ,
T_TAPSS 		,T_TAPMSS ,
T_TAPSS 		,T_TAPMSS ,
T_SCUDOS 		,T_SCUDOMS ,
T_SCUDOSS 	,T_SCUDOMSS ,
T_SCUDOSS 	,T_SCUDOMSS ,
T_ARC 			,T_ARCM ,
T_PIETR1Z 	,T_PIETR1MZ ,
T_PIETR1SS 	,T_PIETR1MSS ,
T_PIETR1SS ,T_PIETR1MSS ,
T_ASCES 		,T_ASCEMS ,
T_ASCESS 		,T_ASCEMSS ,
T_ASCESS 	,T_ASCEMSS ,
T_ROCKS 		,T_ROCKSS ,
T_ROCKSS 	,
T_WOOD1Z 		,T_WOOD1SS ,
T_WOOD1SS 	,
T_WBOXZ 		,T_WBOXSS ,
T_WBOXSS 	,
T_DOOR1S 		,T_DOOR1S ,
T_DOOR2S 		,T_DOOR2S ,
T_DOORYS 		,T_DOORYS ,
T_DOORRS 		,T_DOORRS ,
T_WOOD2Z 		,T_WOOD2SS ,
T_WOOD2SS 	,
T_SBARRES 	,T_SBARRES ,
T_SWITCHU 	,

T_WALL1Z,T_WALL1MZ,T_PIETR1Z,T_PIETR1MZ,T_ROCKZ,T_WOOD1Z,T_WBOXZ,T_WOOD2Z,
T_WALL2Z 		,T_WALL2MZ ,
T_WALL2Z 	,T_WALL2MZ ,
T_PIETR2Z 	,T_PIETR2MZ ,
T_PIETR2Z 	,T_PIETR2MZ ,
T_EARTHZ 		,T_EARTHZ ,
T_SOFTZ 		,T_SOFTMZ ,
T_SOFTZ 		,T_SOFTMZ ,

T_DRAGON  ,T_DRAGON+1,T_DRAGON+2,T_DRAGON+3,T_DRAGON+4,
T_DRAGON+5,T_DRAGON+6,T_DRAGON+7,T_DRAGON+8,

T_GRATAWZ 	,T_GRATAWZ ,
T_GRATABZ 	,T_GRATABZ ,
T_FLROCKZ 	,T_FLROCKZ ,
 
T_OVERDOOR,T_DOORSLOT,T_SWITCHD
	};


#pragma warn -par
void allocfig(int fignum, int x, int y, int dx, int dy, int h) {
	void far *data;

	if ((data = mm_recall(mm_alloc(dx*dy))) != NULL)
		getrfig(x,y,dx,dy,data);
	else error("allocfig",err_notenoughmemory);
}

void adjsample(char far *sample, unsigned len) {
    unsigned i;
    unsigned char *bytes = reinterpret_cast<unsigned char *>(sample);

    
    for (i = 0; i < len; ++i)
        bytes[i] = static_cast<unsigned char>(bytes[i] - 128u);
}

#pragma warn +par

int main() {
	int i,j, idx=0;
	long l;
	char *text;
	void far *data;

	mm_init();
	setvbuff(vpage);

	for (i=0;i<MAXTEXTURES;i++) mm_alloc(4096);	 
	loadLBM("lbm\\walls1.lbm",NULL);
	for (i=0;i<3;i++)
	 for (j=0;j<5;j++,idx++)
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));
	loadLBM("lbm\\walls2.lbm",NULL);
	for (i=0;i<3;i++)
	 for (j=0;j<5;j++,idx++)
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));
	loadLBM("lbm\\walls3.lbm",NULL);
	for (i=0;i<3;i++)
	 for (j=0;j<5;j++,idx++)
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));
	loadLBM("lbm\\walls4.lbm",NULL);
	for (i=0;i<2;i++)
	 for (j=0;j<5;j++,idx++)
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));
	 
	 
	j=0; getrfig(j << 6,i << 6,64,64,mm_recall(T_SWITCHD));
	j++; getrfig(j << 6,i << 6,64,64,mm_recall(T_OVERDOOR));
	j++; getrfig(j << 6,i << 6,64,64,mm_recall(T_DOORSLOT));

	puts("walls... done");

	loadLBM("lbm\\floor1.lbm",NULL);
	for (i=0;i<3;i++)
	 for (j=0;j<5;j++,idx++)
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));
	loadLBM("lbm\\floor2.lbm",NULL);
	for (i=0;i<3;i++)
	 for (j=0;j<5;j++,idx++) {
	   if (idx<T_OVERDOOR) {
		if (idx==T_DRAGON) idx+=9;
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));
	   }
	 }
	loadLBM("lbm\\floor3.lbm",NULL);
	idx=T_DRAGON;
	for (i=0;i<3;i++)
	 for (j=0;j<3;j++,idx++)
		getrfig(j << 6,i << 6,64,64,mm_recall(idx));

	none[0] = mm_alloc(MAXTEXTURES);
	none[1] = mm_alloc(MAXTEXTURES);
	fdmove(mm_recall(none[0]),ChangeWallText,MAXTEXTURES);
	fdmove(mm_recall(none[1]),ChangeFloorText,MAXTEXTURES);

	puts("floors... done");

	loadLBM("lbm\\objects.lbm",NULL);
	for (i=F_TORCIA;i<F_TORCIA+3;i++)
		allocfig(i,221+9*(i-F_TORCIA),133,8,27,-20);
	allocfig(F_TORCIA+3,212,150,8,17,-20);	 
	for (i=F_TORCIAM;i<F_TORCIAM+3;i++)
		allocfig(i,141+9*(i-F_TORCIAM),133,8,29,-22);
	allocfig(F_TORCIAM+3,203,150,8,18,-21);	 

	allocfig(F_BARILE  ,232, 28,20,30,-24);
	allocfig(F_BARILE+1,  1,133,24,30,-24);
	allocfig(F_BARILE+2, 26,133,41,35,-24);
	allocfig(F_BARILE+3,149, 91,51,39,-24);
	allocfig(F_BARILE+4,254, 95,53,46,-28);
	allocfig(F_BARILX , 222,162,33,15,-24);
	allocfig(F_FIREBOOM  ,  2, 41, 7, 6,-3);
	allocfig(F_FIREBOOM+1,222,178,11,10,-2);
	allocfig(F_FIREBOOM+2,235,178,20,18, 0);
	allocfig(F_FIREBOOM+3,200,133,19,16, 3);
	allocfig(F_CARTUCCIA1,1,1,11,13,0);
	allocfig(F_CARTUCCIA2,10,36,14,14,0);
	allocfig(F_CARTUCCIA3,11,22,13,13,0);
	allocfig(F_CARTUCCIA4,13,1,11,20,0);
	allocfig(F_FUCILE,64,0,49,8,0);
	allocfig(F_MITRA,224,1,35,12,0);
	allocfig(F_BAZOOKA,179,49,52,11,0);
	allocfig(F_KEYRED,114,2,31,13,0);
	allocfig(F_KEYELLOW,146,2,31,13,0);
	allocfig(F_VASO,69,74,21,30,-16);
	allocfig(F_PIANTA,94,72,54,59,-16);
	allocfig(F_FARMALIT,207,32,18,14,0);
	allocfig(F_FARMABIG,179,32,27,16,0);
	allocfig(F_SOLDOLIT,9,51,15,12,0);
	allocfig(F_SOLDOBIG,66,105,20,17,0);
	allocfig(F_BONUS  ,175,138,18,17,-8);
	allocfig(F_BONUS+1,175,159,18,17,-8);

	allocfig(F_LAMPOFF,258,142,49,22,0);
	allocfig(F_TRESPOLO  , 64,  9,49,58,-32);
	allocfig(F_TRESPOLO+1,269, 13,49,58,-32);
	allocfig(F_TRESPOLOFF, 69,133,49,57,-32);

	none[0] = mm_alloc(15*16);
	getlfig(15,169,15,16,mm_recall(none[0]));
	none[0]= mm_alloc(15*16);
	getlfig(0,169,15,16,mm_recall(none[0]));
	for (i=0;i<10;i++) {
		if (i==1) {
			none[0] = mm_alloc(55);
			getlfig(116+(i<<3),16,5,11,mm_recall(none[0]));
		} else {
			none[0] = mm_alloc(77);
			getlfig(114+(i<<3),16,7,11,mm_recall(none[0]));
		  }
	}

	puts("objects1... done");

	loadLBM("lbm\\objects2.lbm",NULL);
	allocfig(F_OSSALIT,95,134,47,22,0);
	allocfig(F_OSSABIG,205,107,73,53,-25);
	allocfig(F_TESCHIO  , 95,157,11,11,0);
	allocfig(F_TESCHIO+1,107,157,10,11,0);
	allocfig(F_LAMP+2, 49,  1,46,22,0);
	allocfig(F_LAMP+3, 49, 24,49,22,0);
	allocfig(F_LAMPX , 49, 47,49,22,0);
	allocfig(F_VASO+1  ,100,  1,33,37,-16);
	allocfig(F_VASO+2  ,134,  1,44,37,-16);
	allocfig(F_VASO+3  ,179,  1,48,41,-16);
	allocfig(F_VASO+4  ,228, 23,33,20,-16);
	allocfig(F_VASO+5  ,228, 10,25,12,-16);
	allocfig(F_PIANTA+1,100, 44,50,62,-16);
	allocfig(F_PIANTA+2,151, 44,61,51,-16);
	allocfig(F_PIANTA+3,213, 44,55,33,-16);
	allocfig(F_PIANTA+4,215, 78,53,28,-16);
	allocfig(F_ALBERO  ,151, 96,53,73,-32);
	allocfig(F_ALBERO+1,269, 34,46,72,-32);
	allocfig(F_BRAZIER  ,  1,124,37,46,-24);
	allocfig(F_BRAZIER+1,279,109,37,46,-24);
	allocfig(F_BRAZIER+2, 39,136,37,46,-24);
	allocfig(F_BRAZIER+3,262,  1,37,32,-24);
	for (i=0; i<15; i++)
		allocfig(F_BOMB+i,i*13,188,12,12,-4);

	puts("objects2... done");

	none[0] = mm_alloc(27*33);
	none[1] = mm_alloc(24*37);
	none[2] = mm_alloc(25*33);
	none[3] = mm_alloc(22*33);
	none[4] = mm_alloc(67*12);
	none[5] = mm_alloc(7*10);
	none[6] = mm_alloc(12*12);
	none[7] = mm_alloc(12*12);
	loadLBM("lbm\\misc.lbm",NULL);
	getrfig(128,0,27,33,mm_recall(none[0]));
	getrfig(161,0,24,37,mm_recall(none[1]));
	getrfig(194,0,25,33,mm_recall(none[2]));
	getrfig(227,0,22,33,mm_recall(none[3]));
	getlfig( 0, 0,67,12,mm_recall(none[4]));
	getlfig(67, 1, 7,10,mm_recall(none[5]));
	getlfig(74, 0,12,12,mm_recall(none[7]));
	getlfig(86, 0,12,12,mm_recall(none[6]));

	allocfig(F_COLUMN , 0,45,38,128,-32);
	allocfig(F_COLUMNM,39,45,38,128,-32);

	allocfig(F_LAMP  , 84,45,49,60,-16);
	allocfig(F_LAMP+1,134,45,49,60,-16);

	allocfig(F_CIRCLIGHT ,184,45,48,40,-20);
	allocfig(F_SPIKELIGHT,233,45,48,40,-20);
	allocfig(F_TORCLIGHT ,184,86,48,40,-20);
	allocfig(F_LAMPLIGHT ,233,97,66,24,-12);

	allocfig(F_SMOG  ,282,28,30,22,-11);
	allocfig(F_SMOG+1,282,51,30,22,-11);
	allocfig(F_SMOG+2,282,74,30,22,-11);

	for (i=0;i<9;i++)
		allocfig(F_ANTIKAOS+i,i*25,175,24,16,-8);

	puts("misc... done");

	loadLBM("lbm\\slimer.lbm",NULL);
	allocfig(F_SLIMER   ,241,  6,51,35,-12-5);
	allocfig(F_SLIMER+ 1,  2, 47,62,36,-12-2);
	allocfig(F_SLIMER+ 2, 65, 50,63,36,-12-5);
	allocfig(F_SLIMER+ 3,129, 52,59,38,-17-3);
	allocfig(F_SLIMER+ 4,  1,  2,51,38,-12-4);
	allocfig(F_SLIMER+ 5,133, 96,51,30,-12);
	allocfig(F_SLIMER+ 6,187, 93,61,33,-12);
	allocfig(F_SLIMER+ 7,249, 95,68,31,-12);
	allocfig(F_SLIMER+ 8,  1,144,64,33,-17);
	allocfig(F_SLIMER+ 9,189, 48,52,33,-12);

	 
	allocfig(F_SLIMER+11,128,143,51,34,-12);
	 
	allocfig(F_SLIMER+13,180,140,48,36,-12);

	 
	allocfig(F_SLIMER+15, 66,144,61,27,-12);

	loadLBM("lbm\\bub1.lbm",NULL);
	allocfig(F_BUB  ,  1, 69,70,60,-30);
	allocfig(F_BUB+1, 72, 69,66,60,-30);
	allocfig(F_BUB+2,139, 69,61,60,-30);
	allocfig(F_BUB+3,201, 69,58,60,-30);
	allocfig(F_BUB+4,  1,  9,70,59,-29);

	 
	allocfig(F_BUB+11, 1,130,70,56,-32);

	 
	allocfig(F_BUB+13,72,130,73,56,-32);
	loadLBM("lbm\\bub2.lbm",NULL);
	allocfig(F_BUB+5,  1, 64,70,62,-32);
	allocfig(F_BUB+6, 72, 64,66,62,-32);
	allocfig(F_BUB+7,139, 64,63,62,-32);
	allocfig(F_BUB+8,203, 64,58,62,-32);
	allocfig(F_BUB+9,  1,  1,70,62,-32);
	loadLBM("lbm\\face.lbm",NULL);
	allocfig(F_FACE   ,153,  1,34,42,-21);
	allocfig(F_FACE+ 1,188,  1,37,42,-21);
	allocfig(F_FACE+ 2,226,  1,41,42,-21);
	allocfig(F_FACE+ 3,268,  1,36,42,-21);
	allocfig(F_FACE+ 4,  1,  1,34,42,-21);
	allocfig(F_FACE+ 5,155, 44,36,47,-26);
	allocfig(F_FACE+ 6,192, 44,37,47,-26);
	allocfig(F_FACE+ 7,230, 44,42,47,-26);
	allocfig(F_FACE+ 8,273, 44,37,47,-26);
	allocfig(F_FACE+ 9,  1, 44,34,47,-26);

	 
	allocfig(F_FACE+11,  1, 92,34,45,-24);

	puts("mostri1... done");

	loadLBM("lbm\\morti.lbm",NULL);
	allocfig(F_SLIMER+16,133, 56,62,36,-12);
	allocfig(F_SLIMER+17,196, 56,84,33,-16);
	allocfig(F_SLIMER+18,  1,108,87,38,-24);
	allocfig(F_SLIMER+19, 89,108,85,41,-32);
	allocfig(F_SLIMER+20,175, 93,82,36,-38);
	allocfig(F_SLIMER+21,175,135,76,31,-40);

	allocfig(F_BUB+14,  1,  2,73,53,-32);
	allocfig(F_BUB+15, 75,  1,79,54,-33);
	allocfig(F_BUB+16,155,  1,77,54,-34);
	allocfig(F_BUB+17,233,  5,69,50,-35);
	allocfig(F_BUB+18,  1, 56,66,50,-36);
	allocfig(F_BUB+19, 68, 59,64,48,-37);

	allocfig(F_FACE+12,  1,150,36,42,-26);
	allocfig(F_FACE+13, 38,150,46,46,-28);
	allocfig(F_FACE+14, 85,150,54,44,-30);
	allocfig(F_FACE+15,258, 90,57,37,-32);
	allocfig(F_FACE+16,258,134,52,34,-35);
 








	loadLBM("lbm\\player1.lbm",NULL);
	allocfig(F_PLAYER   ,137,  1,29,51,-30);
	allocfig(F_PLAYER+ 1,167,  1,30,51,-32);
	allocfig(F_PLAYER+ 2,198,  1,40,51,-32);
	allocfig(F_PLAYER+ 3,239,  1,32,51,-32);
	allocfig(F_PLAYER+ 4,  1,  1,26,51,-30);
	allocfig(F_PLAYER+ 5, 28,  1,32,51,-32);
	allocfig(F_PLAYER+ 6, 61,  1,40,51,-32);
	allocfig(F_PLAYER+ 7,102,  1,34,51,-32);
	allocfig(F_PLAYER+ 8,157, 53,29,51,-30);
	allocfig(F_PLAYER+ 9,187, 53,30,51,-32);
	allocfig(F_PLAYER+10,218, 53,48,51,-32);
	allocfig(F_PLAYER+11,  1,105,40,51,-32);
	allocfig(F_PLAYER+12,  1, 53,26,51,-30);
	allocfig(F_PLAYER+13, 28, 53,40,51,-32);
	allocfig(F_PLAYER+14, 69, 53,48,51,-32);
	allocfig(F_PLAYER+15,118, 53,38,51,-32);
	loadLBM("lbm\\player2.lbm",NULL);
	allocfig(F_PLAYER+16,145,  1,29,54,-32);
	allocfig(F_PLAYER+17,175,  1,37,51,-32);
	allocfig(F_PLAYER+18,213,  1,48,50,-32);
	allocfig(F_PLAYER+19,262,  1,33,53,-32);
	allocfig(F_PLAYER+20,  1,  1,31,54,-32);
	allocfig(F_PLAYER+21, 33,  1,29,53,-32);
	allocfig(F_PLAYER+22, 63,  1,48,50,-32);
	allocfig(F_PLAYER+23,112,  1,32,55,-32);
	allocfig(F_PLAYER+24,151, 58,29,54,-32);
	allocfig(F_PLAYER+25,181, 58,35,55,-32);
	allocfig(F_PLAYER+26,217, 58,49,50,-32);
	allocfig(F_PLAYER+27,267, 58,30,53,-32);
	allocfig(F_PLAYER+28,  1, 58,29,54,-32);
	allocfig(F_PLAYER+29, 31, 58,33,53,-32);
	allocfig(F_PLAYER+30, 65, 58,47,50,-32);
	allocfig(F_PLAYER+31,113, 58,37,51,-32);

	 
	allocfig(F_PLAYER+33,  1,114,35,53,-32);
	allocfig(F_PLAYER+34, 37,114,35,53,-32);
	allocfig(F_PLAYER+35, 73,114,51,45,-26);
	allocfig(F_PLAYER+36,125,132,50,27,-33);
	allocfig(F_PLAYER+37,176,132,50,27,-37);
	allocfig(F_PLAYER+38,227,139,50,20,-40);
	loadLBM("lbm\\ragno.lbm",NULL);
	allocfig(F_SPIDER   ,  1, 31, 68, 28,-20);
	allocfig(F_SPIDER+ 1, 70, 33, 67, 29,-20);
	allocfig(F_SPIDER+ 2,138, 34, 69, 28,-20);
	allocfig(F_SPIDER+ 3,208, 33, 62, 29,-20);
	allocfig(F_SPIDER+ 4,  1,  1, 67, 29,-20);
	allocfig(F_SPIDER+ 5,  1, 93, 68, 28,-20);
	allocfig(F_SPIDER+ 6, 70, 93, 62, 29,-16);
	allocfig(F_SPIDER+ 7,133, 93, 62, 29,-20);
	allocfig(F_SPIDER+ 8,198, 97, 65, 27,-20);
	allocfig(F_SPIDER+ 9,  1, 63, 67, 29,-20);

	 
	allocfig(F_SPIDER+11,  1,122, 67, 29,-20);

	 
	allocfig(F_SPIDER+13, 69,125, 68, 45,-20);

	allocfig(F_SPIDER+14,  1,152, 67, 45,-18);
	allocfig(F_SPIDER+15,138,125, 70, 40,-18);
	allocfig(F_SPIDER+16,209,125, 69, 33,-20);
	allocfig(F_SPIDER+17,138,166, 69, 30,-22);
	allocfig(F_SPIDER+18,208,166, 69, 26,-22);

	puts("mostri2... done");

	loadLBM("lbm\\other.lbm",NULL);
	allocfig(F_MISSILE  ,141,  2,28,24,-12);
	allocfig(F_MISSILE+1,102, 27,38,25,-12);
	allocfig(F_MISSILE+2, 56, 23,45,17, -8);
	allocfig(F_MISSILE+3, 29, 22,26,16, -8);
	allocfig(F_MISSILE+4,  1,  2,27,23,-11);

	allocfig(F_MOUSE  , 77,53,12,15,-8);
	allocfig(F_MOUSE+1, 90,53,18,15,-9);
	allocfig(F_MOUSE+2,109,55,27, 9,-4);
	allocfig(F_MOUSE+3,137,53,16,11,-6);
	allocfig(F_MOUSE+4,  0,53,12,11,-4);
	allocfig(F_MOUSE+5,  0,81,27,15,-1);
	allocfig(F_MOUSE+6, 28,74,31,19, 2);
	allocfig(F_MOUSE+7, 60,69,27,15, 7);
	allocfig(F_MOUSE+8, 92,75,29,18, 3);
	allocfig(F_MOUSE+9,125,87,24, 9,-4);

	getlfig(176,0,16,16,mm_recall(mm_alloc(16*16)));
	getlfig(176,16,16,16,mm_recall(mm_alloc(16*16)));
	for (i=0; i<11; i++) {
		none[0] = mm_alloc(32*24);
		getrfig(192+((i & 3)<<5),(i>>2)*24,32,24,mm_recall(none[0]));
	}

	none[0] = mm_alloc(152*20);
	getlfig(166,74,152,20,mm_recall(none[0]));

	for (i=0;i<10;i++) {
		none[0] = mm_alloc(15);
		getlfig(179+(i<<2),129,3,5,mm_recall(none[0]));
	}
	for (i=2;i<10;i++) {
		none[0] = mm_alloc(63);
		getlfig(172+(i<<3),137,7,9,mm_recall(none[0]));
	}
	none[0] = mm_alloc(63);
	getlfig(188+(8<<3),137,7,9,mm_recall(none[0]));
	none[0] = mm_alloc(36);
	getlfig(183,137,4,9,mm_recall(none[0]));
	for (i=0;i<10;i++) {
		none[0] = mm_alloc(49);
		getlfig(176+(i<<3),149,7,7,mm_recall(none[0]));
	}
	for (i=0;i<17;i++) {
		none[0] = mm_alloc(24*29);
		getlfig(1+(i%10)*25,97+(i/10)*30,24,29,mm_recall(none[0]));
	}
	none[0] = mm_alloc(219*20);
	none[1] = mm_alloc(64*16);
	none[2] = mm_alloc(64*16);
	getlfig(1,157,219,20,mm_recall(none[0]));
	getlfig(1,178,64,16,mm_recall(none[1]));
	getlfig(66,178,64,16,mm_recall(none[2]));

	loadLBM("lbm\\sold1.lbm",NULL);
	allocfig(F_SOLDIER   ,149,4,32,63,-40);
	allocfig(F_SOLDIER+1 ,182,3,31,64,-40);
	allocfig(F_SOLDIER+2 ,214,2,33,65,-40);
	allocfig(F_SOLDIER+3 ,248,2,45,65,-40);
	allocfig(F_SOLDIER+4 ,  1,1,35,66,-40);
	allocfig(F_SOLDIER+5 , 37,2,29,65,-40);
	allocfig(F_SOLDIER+6 , 67,3,36,64,-40);
	allocfig(F_SOLDIER+7 ,104,3,44,64,-40);
	allocfig(F_SOLDIER+8 ,179,69,42,66,-40);
	allocfig(F_SOLDIER+9 ,222,68,36,64,-40);
	allocfig(F_SOLDIER+10,1,135,41,63,-40);
	allocfig(F_SOLDIER+11,43,135,50,65,-40);
	allocfig(F_SOLDIER+12,1,68,48,66,-40);
	allocfig(F_SOLDIER+13,50,68,34,66,-40);
	allocfig(F_SOLDIER+14,85,70,43,62,-40);
	allocfig(F_SOLDIER+15,129,68,49,66,-40);
	loadLBM("lbm\\sold2.lbm",NULL);
	allocfig(F_SOLDIER+16,155,1,33,66,-40);
	allocfig(F_SOLDIER+17,189,1,32,66,-40);
	allocfig(F_SOLDIER+18,222,1,57,65,-40);
	allocfig(F_SOLDIER+19,1,67,35,64,-40);
	allocfig(F_SOLDIER+20,1,1,32,65,-40);
	allocfig(F_SOLDIER+21,34,1,29,65,-40);
	allocfig(F_SOLDIER+22,64,1,53,65,-40);
	allocfig(F_SOLDIER+23,118,1,36,65,-40);

	 
	allocfig(F_SOLDIER+25,37,67,34,66,-40);
	 
	allocfig(F_SOLDIER+27,72,67,30,66,-40);

	 
	allocfig(F_SOLDIER+29,103,69,55,68,-40);

	allocfig(F_SOLDIER+30,159,69,55,68,-36);
	allocfig(F_SOLDIER+31,215,69,68,56,-32);
	allocfig(F_SOLDIER+32,1,138,58,28,-36);
	allocfig(F_SOLDIER+33,60,142,58,24,-40);
	allocfig(F_SOLDIER+34,119,145,58,21,-42);
	loadLBM("lbm\\balls.lbm",NULL);
	 
	allocfig(F_BOOM   ,  0, 0,15,15,-7);
	 
	allocfig(F_BOOM+ 1,  0, 64,21,18,-9);
	allocfig(F_BOOM+ 2, 16,  0,37,33,-16);
	allocfig(F_BOOM+ 3, 54,  0,50,42,-21);
	allocfig(F_BOOM+ 4,105,  0,53,47,-23);
	allocfig(F_BOOM+ 5, 22, 64,45,39,-19);
	allocfig(F_BOOM+ 6,161,  2,12, 9,-4);
	allocfig(F_BOOM+ 7,175,  0,37,31,-15);
	allocfig(F_BOOM+ 8,213,  0,49,40,-20);
	allocfig(F_BOOM+ 9, 50,120,12, 9, -4);
	allocfig(F_BOOM+10, 63,120,37,31,-15);
	allocfig(F_BOOM+11,101,120,49,40,-20);
	 
	allocfig(F_SLIMFIRE+1,162,15,9,9,-4);

	allocfig(F_SPIDFIRE  ,263,0,16,16,-8);
	allocfig(F_SPIDFIRE+1,280,0,16,16,-8);
	allocfig(F_SPIDFIRE+2,263,17,16,16,-8);
	allocfig(F_SPIDFIRE+3,280,17,16,16,-8);

	allocfig(F_BLOOD  ,132,101, 5, 4,-2);
	allocfig(F_BLOOD+1,147, 99,11, 9,-5);
	allocfig(F_BLOOD+2,168, 94,18,18,-9);
	allocfig(F_BLOOD+3,159,129,31,27,-13);
	allocfig(F_BLOOD+4,197,128,42,33,-17);

	 
	 

	for (i=0;i<4;i++) none[i] = mm_alloc(4*4);
	for (i=4;i<8;i++) none[i] = mm_alloc(24*4);
	getlfig(0,133,4,4,mm_recall(none[0]));
	getlfig(5,133,4,4,mm_recall(none[1]));
	getlfig(0,138,4,4,mm_recall(none[2]));
	getlfig(5,138,4,4,mm_recall(none[3]));
	getlfig(1,128,24,4,mm_recall(none[4]));
	getlfig(31,123,4,24,mm_recall(none[5]));
	getlfig(26,123,4,24,mm_recall(none[6]));
	getlfig(1,123,24,4,mm_recall(none[7]));

	none[0] = mm_alloc(28*11);
	none[1] = mm_alloc(20*12);
	none[2] = mm_alloc(33*9);
	none[3] = mm_alloc(29*12);
	none[4] = mm_alloc(30*8);
	none[5] = mm_alloc(9*12);
	getlfig(120,52,28,11,mm_recall(none[0]));
	getlfig(120,64,20,12,mm_recall(none[1]));
	getlfig(141,64,33, 9,mm_recall(none[2]));
	getlfig(120,77,29,12,mm_recall(none[3]));
	getlfig(150,77,30, 8,mm_recall(none[4]));
	getlfig(149,51, 9,12,mm_recall(none[5]));
	none[0] = mm_alloc(320*32);
	fcopyarea(168,0,32,mm_recall(none[0]));
	none[0] = mm_alloc(68*32);
	none[1] = mm_alloc(76*32);
	none[2] = mm_alloc(68*32);
	getlfig(250,54,68,32,mm_recall(none[0]));
	getlfig(242,90,76,32,mm_recall(none[1]));
	getlfig(250,126,68,32,mm_recall(none[2]));

	for (i=0;i<3;i++) {
		none[0] = mm_alloc(16*16);
		getlfig(196+i*17,56,16,16,mm_recall(none[0]));
	}
	none[0] = mm_alloc(14*14);
	none[1] = mm_alloc(14*14);
	getlfig(196,73,14,14,mm_recall(none[0]));
	getlfig(211,73,14,14,mm_recall(none[1]));
	none[0] = mm_alloc(32*28);
	getlfig(205,91,32,28,mm_recall(none[0]));

	none[0] = mm_alloc(32000);
	none[1] = mm_alloc(32000);
	PPal apppal = (PPal)mm_recall(mm_alloc(768));
	loadLBM("lbm\\bck_c0.lbm",apppal);
	fcopyarea(0,0,100,mm_recall(none[1]));
	fcopyarea(100,0,100,mm_recall(none[0]));
	fwfill(apppal,0,3);
	none[0] = mm_alloc(32000);
	none[1] = mm_alloc(32000);
	loadLBM("lbm\\bck_f0.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(none[0]));
	fcopyarea(100,0,100,mm_recall(none[1]));

	none[0] = mm_alloc(32000);
	none[1] = mm_alloc(32000);
	loadLBM("lbm\\bck_c1.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(none[1]));
	fcopyarea(100,0,100,mm_recall(none[0]));
	none[0] = mm_alloc(32000);
	none[1] = mm_alloc(32000);
	loadLBM("lbm\\bck_f1.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(none[0]));
	fcopyarea(100,0,100,mm_recall(none[1]));

	none[0] = mm_alloc(32000);
	none[1] = mm_alloc(32000);
	loadLBM("lbm\\bck_c2.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(none[1]));
	fcopyarea(100,0,100,mm_recall(none[0]));
	none[0] = mm_alloc(32000);
	none[1] = mm_alloc(32000);
	loadLBM("lbm\\bck_f2.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(none[0]));
	fcopyarea(100,0,100,mm_recall(none[1]));

	none[0] = mm_alloc(50*115);
	none[1] = mm_alloc(85*150);
	none[2] = mm_alloc(106*123);
	loadLBM("lbm\\Ascia.lbm",NULL);
	getrfig(0,0,50,115,mm_recall(none[0]));
	getrfig(51,0,85,150,mm_recall(none[1]));
	getrfig(137,0,106,123,mm_recall(none[2]));
	none[0] = mm_alloc(55*61);
	none[1] = mm_alloc(55*67);
	none[2] = mm_alloc(42*39);
	none[3] = mm_alloc(80*128);
	loadLBM("lbm\\Mano.lbm",NULL);
	getrfig(0,0,55,61,mm_recall(none[0]));
	getrfig(0,62,55,67,mm_recall(none[1]));
	getrfig(56,0,42,39,mm_recall(none[2]));
	getrfig(124,40,80,128,mm_recall(none[3]));
	none[0] = mm_alloc(65*53);
	none[1] = mm_alloc(99*108);
	none[2] = mm_alloc(40*31);
	loadLBM("lbm\\Fucile.lbm",NULL);
	getrfig(0,0,65,53,mm_recall(none[0]));
	getrfig(0,54,99,108,mm_recall(none[1]));
	getrfig(66,0,40,31,mm_recall(none[2]));
	none[0] = mm_alloc(53*77);
	none[1] = mm_alloc(53*31);
	none[2] = mm_alloc(53*31);
	none[3] = mm_alloc(53*31);
	none[4] = mm_alloc(42*41);
	none[5] = mm_alloc(83*68);
	loadLBM("lbm\\Mitra.lbm",NULL);
	getrfig(0,0,53,77,mm_recall(none[0]));
	getrfig(169,0,53,31,mm_recall(none[1]));
	getrfig(54,0,53,31,mm_recall(none[2]));
	getrfig(112,0,53,31,mm_recall(none[3]));
	getrfig(223,0,42,41,mm_recall(none[4]));
	getrfig(0,88,83,68,mm_recall(none[5]));
	loadsound(SND_THUNDER1,	"wav\\thunder.wav");
	loadsound(SND_THUNDER2,	"wav\\tuono.wav");
	loadsound(SND_GOCCIA,	"wav\\goccia.wav");
	loadsound(SND_BOOM,		"wav\\esplos.wav");
	loadsound(SND_BUB,		"wav\\bub3.wav");
	loadsound(SND_TOPO,		"wav\\topo.wav");
	loadsound(SND_KEYS,		"wav\\keys.wav");
	loadsound(SND_PISTOLA,	"wav\\fucil2.wav");
	loadsound(SND_FUCILE,	"wav\\fucil1.wav");
	loadsound(SND_MITRA,	"wav\\pist1.wav");
	loadsound(SND_MISSILE,	"wav\\missile.wav");
	loadsound(SND_SLIME1,	"wav\\slime01.wav");
	loadsound(SND_SLIME2,	"wav\\slime02.wav");
	loadsound(SND_PASSO,	"wav\\passo.wav");
	loadsound(SND_PORTA1,	"wav\\porta2.wav");
	loadsound(SND_PORTA2,	"wav\\porta1.wav");
	loadsound(SND_SCODE,	"wav\\beam.wav");
	loadsound(SND_PROIETTILE,"wav\\proiett.wav");
	loadsound(SND_VASO1,	"wav\\vaso1.wav");
	loadsound(SND_VASO2,	"wav\\vaso2.wav");
	loadsound(SND_COLPO1,	"wav\\colpo1.wav");
	loadsound(SND_COLPO2,	"wav\\colpo3.wav");
	loadsound(SND_TOGGLE,	"wav\\fineliv.wav");
	loadsound(SND_WIND,		"wav\\wind.wav");
	loadsound(SND_FIRE,		"wav\\fire.wav");
	loadsound(SND_MISSILGO,	"wav\\missile2.wav");
	loadsound(SND_UNLOCK,	"wav\\unlock.wav");
	loadsound(SND_CLOSE,	"wav\\close.wav");
	loadsound(SND_GLASS,	"wav\\glass.wav");
	loadsound(SND_PASSO2,	"wav\\passo2.wav");
	loadsound(SND_SOLDDEAD,	"wav\\solddead.wav");
	loadsound(SND_SLIMDEAD,	"wav\\slimdead.wav");
	loadsound(SND_SPIDDEAD,	"wav\\spiddead.wav");
	loadsound(SND_SOLDATT,	"wav\\soldatt.wav");
	loadsound(SND_SLIMATT,	"wav\\slimatt.wav");
	loadsound(SND_SPIDATT,	"wav\\spidatt.wav");
	loadsound(SND_CLOCK,	"wav\\clock.wav");
	loadsound(SND_CLOCK2,	"wav\\clock2.wav");
	loadsound(SND_SPIDNEAR,	"wav\\spidnear.wav");
	loadsound(SND_CHARGE,	"wav\\charge.wav");
	loadsound(SND_CHARGE2,	"wav\\charge2.wav");
	loadsound(SND_SOLDDOWN,	"wav\\solddown.wav");
	loadsound(SND_SLIMDOWN,	"wav\\slimdown.wav");
	loadsound(SND_SPIDDOWN,	"wav\\spiddown.wav");
	loadsound(SND_OSSA,		"wav\\ossa.wav");
	loadsound(SND_VENOM,	"wav\\venom.wav");
	loadsound(SND_RISATA,	"wav\\risata.wav");
	loadsound(SND_BONUS,	"wav\\bonus.wav");
	loadsound(SND_ASCIAHIT,	"wav\\asciahit.wav");
	loadsound(SND_SOLDHIT,	"wav\\soldhit.wav");
	loadsound(SND_SLIMHIT,	"wav\\slimhit.wav");
	loadsound(SND_SPIDHIT,	"wav\\spidhit.wav");
	loadsound(SND_BBOUNCE,	"wav\\bbounce.wav");

	Font *Std = new Font("std"); Std->setdispl(-1,0);
	Font *Big = new Font("big"); Big->setdispl(-1,0);
	Std->packdata();
	Big->packdata();

	sl_convert("GREETING.SLF");
	sl_convert("QUOTES.SLF");
	sl_convert("ORDERING.SLF");
	sl_convert("HELP.SLF");
	sl_convert("INFO.SLF");
	sl_convert("MESSAGES.SLF");
	sl_convert("Keyboard.slf");	

	for (i=0;i<4;i++)
		none[i] = mm_alloc(16000);
	loadLBM("lbm\\dragon2.lbm",NULL);
	for (i=0;i<4;i++) fcopyarea(i*50,0,50,mm_recall(none[i]));
	for (i=0;i<4;i++)
		none[i] = mm_alloc(16000);
	loadLBM("lbm\\dragon1.lbm",NULL);
	for (i=0;i<4;i++) fcopyarea(i*50,0,50,mm_recall(none[i]));

	none[4] = mm_alloc(768);
	for (i=0;i<4;i++) none[i] = mm_alloc(16000);
	loadLBM("lbm\\pres.lbm",mm_recall(none[4]));
	for (i=0;i<4;i++) fcopyarea(i*50,0,50,mm_recall(none[i]));

	mm_makedatafile();

	mm_done();
	return 0;
}