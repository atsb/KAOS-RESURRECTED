#pragma once
#ifndef KAOS_GAME_HPP
#define KAOS_GAME_HPP

#include "std.hpp"
#include "fixed.h"
 






extern std::uint64_t kaos_sim_tic;

#define	T_WALL1	 	 0
#define	T_WALL1M	 1
#define	T_WALL1S     2
#define	T_WALL1MS	 3
#define	T_WALL1SS	 4
#define	T_WALL1MSS	 5
#define T_TAP		 6
#define T_TAPM		 7
#define T_TAPS		 8
#define T_TAPMS		 9
#define T_TAPSS		10
#define T_TAPMSS	11
#define T_SCUDO		12
#define T_SCUDOM	13
#define T_SCUDOS	14
#define T_SCUDOMS	15
#define T_SCUDOSS	16
#define T_SCUDOMSS	17
#define T_ARC		18
#define T_ARCM		19
#define T_PIETR1	20
#define T_PIETR1M 	21
#define T_PIETR1S	22
#define T_PIETR1MS 	23
#define T_PIETR1SS	24
#define T_PIETR1MSS	25
#define T_ASCE		26
#define T_ASCEM		27
#define T_ASCES		28
#define T_ASCEMS	29
#define T_ASCESS	30
#define T_ASCEMSS	31
#define T_ROCK		32
#define T_ROCKS		33
#define T_ROCKSS	34
#define T_WOOD1		35
#define T_WOOD1S	36
#define T_WOOD1SS	37
#define T_WBOX		38
#define T_WBOXS		39
#define T_WBOXSS	40
#define T_DOOR1		41
#define T_DOOR1S	42
#define T_DOOR2		43
#define T_DOOR2S	44
#define T_DOORY		45
#define T_DOORYS	46
#define T_DOORR		47
#define T_DOORRS	48
#define T_WOOD2		49
#define T_WOOD2S	50
#define T_WOOD2SS	51
#define T_SBARRE	52
#define T_SBARRES	53
#define T_SWITCHU	54

#define T_WALL1Z	55
#define T_WALL1MZ	56
#define T_PIETR1Z	57
#define T_PIETR1MZ	58
#define T_ROCKZ		59
#define T_WOOD1Z	60
#define T_WBOXZ  	61
#define	T_WOOD2Z	62
#define	T_WALL2 	63
#define T_WALL2M	64
#define T_WALL2Z	65
#define T_WALL2MZ	66
#define T_PIETR2	67
#define T_PIETR2M	68
#define T_PIETR2Z	69
#define T_PIETR2MZ	70
#define T_EARTH		71
#define T_EARTHZ	72
#define T_SOFT		73
#define T_SOFTM		74
#define T_SOFTZ		75
#define T_SOFTMZ	76

#define T_DRAGON	77	 

 
#define T_GRATAW	86
#define T_GRATAWZ	87
#define T_GRATAB	88
#define T_GRATABZ	89
#define	T_FLROCK	90
#define	T_FLROCKZ	91
#define T_OVERDOOR	92
#define T_DOORSLOT	93
#define T_SWITCHD	94
#define	T_NULL		T_DOORSLOT

#define T_TRANSP	T_GRATAW
#define TEXTNUM		(T_SWITCHD+1)

#define	T_TEXTURES	(TEXTNUM)
#define	T_OBJECTS	(T_TEXTURES+36+18)	 
#define	T_ENEMIES	(T_OBJECTS+12)
#define	T_PLAYERS	(T_ENEMIES+12)

#define F_PLAYER		0				 
#define	F_SOLDIER		(F_PLAYER+39)	 
#define	F_SLIMER		(F_SOLDIER+35)	 
#define	F_BUB	 	 	(F_SLIMER+22)  	 
#define	F_FACE			(F_BUB+20)		 
#define	F_SPIDER	    (F_FACE+17)  	 
#define	F_MOUSE			(F_SPIDER+19)	 
#define	F_MISSILE		(F_MOUSE+10)	 
#define	F_FIREBALL		(F_MISSILE+5)
#define	F_SLIMFIRE		(F_FIREBALL+1)   
#define	F_SPIDFIRE		(F_SLIMFIRE+2)   
#define	F_TORCIA  		(F_SPIDFIRE+4)	 
#define	F_TORCIAM  		(F_TORCIA+4)	 
#define	F_COLUMN		(F_TORCIAM+4)
#define	F_COLUMNM		(F_COLUMN+1)
#define	F_BARILE		(F_COLUMNM+1)	 
#define	F_BARILX		(F_BARILE+5)
#define	F_FIREBOOM		(F_BARILX+1)     
#define	F_KEYRED      	(F_FIREBOOM+4)
#define	F_KEYELLOW   	(F_KEYRED+1)
#define	F_OSSALIT     	(F_KEYELLOW+1)
#define	F_OSSABIG     	(F_OSSALIT+1)
#define	F_VASO        	(F_OSSABIG+1)    
#define	F_PIANTA  		(F_VASO+6) 		 
#define	F_ALBERO		(F_PIANTA+5)	 
#define	F_LAMP			(F_ALBERO+2)	 
#define	F_LAMPX			(F_LAMP+4)
#define	F_LAMPOFF		(F_LAMPX+1)
#define	F_TRESPOLO     	(F_LAMPOFF+1)	 
#define	F_TRESPOLOX    	(F_TRESPOLO+2)	 
#define	F_TRESPOLOFF	(F_TRESPOLOX+1)
#define	F_BRAZIER		(F_TRESPOLOFF+1) 	 
#define	F_FUCILE       	(F_BRAZIER+4)
#define	F_MITRA       	(F_FUCILE+1)
#define	F_BAZOOKA       (F_MITRA+1)
#define	F_FARMALIT		(F_BAZOOKA+1)
#define	F_FARMABIG    	(F_FARMALIT+1)
#define	F_BONUS       	(F_FARMABIG+1)		 
#define	F_SOLDOLIT    	(F_BONUS+2)
#define	F_SOLDOBIG    	(F_SOLDOLIT+1)
#define F_CARTUCCIA1	(F_SOLDOBIG+1)
#define F_CARTUCCIA2	(F_CARTUCCIA1+1)
#define F_CARTUCCIA3	(F_CARTUCCIA2+1)
#define F_CARTUCCIA4	(F_CARTUCCIA3+1)
#define	F_BOOM			(F_CARTUCCIA4+1) 	 
#define	F_BLOOD         (F_BOOM+12)			 
#define F_SMOG			(F_BLOOD+5)		  	 
#define F_TESCHIO		(F_SMOG+3)		  	 
#define F_BOMB			(F_TESCHIO+2)		 
#define F_CIRCLIGHT		(F_BOMB+15)
#define F_SPIKELIGHT	(F_CIRCLIGHT+1)
#define F_TORCLIGHT     (F_SPIKELIGHT+1)
#define F_LAMPLIGHT		(F_TORCLIGHT+1)
#define	F_ANTIKAOS		(F_LAMPLIGHT+1)		 
#define	FIGNUM			(F_ANTIKAOS+9)		 

 
#define	GRID_DX 		64
#define	GRID_DY 		64
#define MAP_DX			64
#define MAP_DY			64
#define	MAPDIM  		(MAP_DX*MAP_DY)
 
#define bxytoidx(_bx,_by)	((_bx << 6) + _by)

#define MAX_X		(MAP_DX << 6)
#define	MAX_Y		(MAP_DY << 6)
#define LMAX_X		(fixed(MAX_X) << FIXSHIFT)
#define	LMAX_Y		(fixed(MAX_Y) << FIXSHIFT)

#define	MAXOBJECTS		640  
#define	MAXACTORS		400  
#define	MAXDRAWING		128
#define MAXTEXTURES		TEXTNUM
#define	MAXFIGURES		FIGNUM

 
 











 
#define SC_MNNico		1
#define SC_MNMaxy		2
#define SC_MNAndy		3
#define SC_MNLev 		4
#define SC_MNStrg		5
#define SC_MNTMap 		6
#define SC_MNBest		7
#define SC_MNDark		8
#define SC_MNBoom		9
#define SC_MNBGod		10
#define	SC_MNHard		11
#define	SC_MNInvi		12

 
#define	GETFLAG(data,flag)		((data & flag) != 0)
#define	GETEFLAG(data,flag)		((data & flag) == flag)
#define	SETFLAG(data,flag)		(data |= flag)
#define	CLRFLAG(data,flag)		(data &= ~flag)
#define	CHGFLAG(data,flag)		(data ^= flag)
#define	MAKEFLAG(data,flag,bit) data = bit ? (data | flag) : (data & ~flag)

 
 
#define MSH_SQUARE		0
#define	MSH_VERT		1
#define	MSH_HORIZ		2
#define	MSH_DIAG1		3
#define	MSH_DIAG2		4
#define	MSH_DOOR		5
#define	MAPSHAPE(X)		(X<<8)
 
#define MTP_WALL		0
#define	MTP_DOOR1		1	 
#define	MTP_DOOR2		2    
#define MTP_GRID1 		3	 
#define	MTP_SLOT		4
#define MAPTYPE(X)		(X<<11)
 
#define	MFL_NONE	0x4000
#define	MFL_STOP 	0x8000
typedef struct {		   
			byte data  		: 8;
			byte shape 		: 3;
			byte type  		: 3;
			byte none		: 1;	 
			byte stop		: 1; 	 
		} TMapInfo;
 
#define	OMF_UNWALKABLE	0x1000
#define	OMF_VISIBLE		0x0100
#define	OMF_HITABLE		0x0010
#define	OMF_SOUNDABLE	0x0001
#define OMF_STANDARD	0x1111
 
#define	OMM_UNWALKABLE	0xF000
#define	OMM_VISIBLE		0x0F00
#define	OMM_HITABLE		0x00F0
#define	OMM_SOUNDABLE	0x000F
typedef struct {		   
			byte soundable	: 4;
			byte hitable	: 4;
			byte visible	: 4;
			byte unwalkable	: 4;
		} TObjInfo;

#define SND_THUNDER1	0
#define SND_THUNDER2	1
#define SND_GOCCIA		2
#define SND_BOOM		3
#define SND_BUB			4
#define SND_TOPO		5
#define SND_KEYS		6
#define SND_PISTOLA		7
#define SND_FUCILE		8
#define SND_MITRA		9
#define SND_MISSILE		10
#define	SND_SLIME1		11
#define	SND_SLIME2		12
#define	SND_PASSO		13
#define	SND_PORTA1		14
#define	SND_PORTA2		15
#define	SND_SCODE		16
#define	SND_PROIETTILE	17
#define	SND_VASO1		18
#define	SND_VASO2		19
#define	SND_COLPO1		20
#define	SND_COLPO2		21
#define	SND_TOGGLE		22
#define	SND_WIND		23
#define	SND_FIRE		24
#define	SND_MISSILGO	25
#define	SND_UNLOCK		26
#define	SND_CLOSE		27
#define	SND_GLASS		28
#define	SND_PASSO2		29
#define	SND_SOLDDEAD	30
#define	SND_SLIMDEAD	31
#define	SND_SPIDDEAD	32
#define	SND_SOLDATT		33
#define	SND_SLIMATT		34
#define	SND_SPIDATT		35
#define	SND_CLOCK		36
#define	SND_CLOCK2		37
#define	SND_SPIDNEAR	38
#define	SND_CHARGE		39
#define	SND_CHARGE2		40
#define	SND_SOLDDOWN	41
#define	SND_SLIMDOWN	42
#define	SND_SPIDDOWN	43
#define	SND_OSSA		44
#define	SND_VENOM		45
#define	SND_RISATA		46
#define	SND_BONUS		47
#define	SND_ASCIAHIT	48
#define	SND_SOLDHIT		49
#define	SND_SLIMHIT		50
#define	SND_SPIDHIT		51
#define SND_BBOUNCE		52
 

 
#define	LCKT_NONE		0x00
#define	LCKT_L1			0x01	 
#define	LCKT_L2			0x02     
 
 

 
#define	PAA_RED		1
#define	PAA_GREEN	2
#define	PAA_GOLD    3
#define	PAA_YKEY	4
#define PAA_RKEY	5
#define	PAA_ARM		6
#define	PAA_GOD		7
#define	PAA_UNGOD	8
#define	PAA_FADEIN	9
#define PAA_FADEOUT	10
#define	PAA_LIGHT	11
#define	PAA_HARD	12
#define	PAA_UNHARD	13

 
#define	OBJT_OBJECT			0
#define	OBJT_ACTOR			1
#define	OBJT_LIGHTACTOR		2
#define	OBJT_CANFIREACTOR 	3
#define OBJT_OSSA			4
#define	OBJT_FLOSSA			5
#define	OBJT_BONUS			6
#define	OBJT_SLIMER			7
#define	OBJT_SPIDER			8
#define	OBJT_SOLDIER		9
#define	OBJT_MOUSE			10
#define	OBJT_FIREBOOM		11
#define	OBJT_FIREBALL		12
#define	OBJT_TORCIA			13
#define	OBJT_LAMPADARIO		14
#define	OBJT_BARILE			15
#define	OBJT_VASO			16
#define	OBJT_PLAYER			17
#define	OBJT_OBJMOVER		18
#define	OBJT_BOMB			19
#define	OBJT_ANTIKAOS		20
#define	OBJT_END			32

 
#define	ARM_ASCIA		0
#define	ARM_PISTOLA		1
#define	ARM_FUCILE  	2
#define	ARM_MITRA		3
#define	ARM_BAZOOKA		4
#define	ARM_BOMB		5
 

 
#define	STT_KILL		0
#define	STT_HIT			1
#define	STT_FIRE		2
#define	STT_BLOOD       3
#define STT_TECNO		4

 
#define	GFL_USESYNC		0x01
#define	GFL_AUTOCENTER	0x02
#define	GFL_SHOWBLOOD	0x04
#define	GFL_KAOSENABLE	0x08
#define	GFL_MESSAGES	0x10
#define	GFL_TOTALMAP	0x20
#define	GFL_NORANDOM    0x40
#define	GFL_COMPETITIVE	0x80

typedef struct {
			byte dx, dy;
			std::int16_t height;
			std::int16_t memidx, xxx;
		} TFigure;
typedef	struct {
			std::int16_t dx, dy, memidx;
		} TFig;
typedef struct {
			char up, down, left, right,
				 open, fire, fast, strafe, chgarm,
				 lookup, lookmid, lookdown, map;
		} TControl;

static_assert(sizeof(TControl) == 13,
			  "DOS control record must remain 13 bytes");
static_assert(sizeof(TFigure) == 8 && sizeof(TFig) == 6,
			  "DOS figure records must retain their 16-bit int fields");

extern TMapInfo *Map;
extern TObjInfo *ObjMap;
extern std::int16_t *IDMap;
extern char *LightMap;
extern TFigure FigList[MAXFIGURES];

extern char players;
extern byte gameflags;
 



extern char quake; 	 
extern int windsound;
extern int cbackg0,fbackg0,cbackg1,fbackg1;
extern TFig *litnumf, *normnumf, *bignumf;
extern TFig *happyface;
extern word	maxdim;
extern int menustate;
extern char viewstate;
extern int PlayerAscia[3],PlayerPist[3],PlayerFuc[3],PlayerMitr[5],
			PlayerBaz;
extern int playbackhandle;
extern word playbackwait;
extern char saveplayback;
extern int numenemies;
 
extern std::int16_t Kaos_count;	 
extern int minibox_I, minibox_F;

extern TControl PControl[4];
extern char s_fullscreen;

extern int xytoidx(fixed x, fixed y);
extern int is_nearPlayer(fixed x, fixed y, fixed maxdist);
extern void drawnum(int x, int y, byte col1, byte col2,
					TFig *numfont, int num);
extern void animpal(byte animnum, int rep);
#endif  
int kaos_main(int argc, char **argv);
