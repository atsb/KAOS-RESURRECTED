 






 
 

 
 
#define	__CHECKFIRM__

#define MAXBACKGROUND		3
#define	MAXRESETCNT			36
#define	MAXPLAYBACKS		3
#define MAXPLAYBACKWAIT		360

#define MESSAGE_TIME		54

#include <stdlib.h>
#include <stdio.h>
#include "platform_compat.hpp" 		 
#include "platform_compat.hpp"	 	 
#include "platform_compat.hpp"
#include <atomic>
#include <thread>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <string.h>
#include <ctime>
#include <cctype>
#include "platform_compat.hpp"		 

#include "std.hpp"
#include "kaos_sdl.hpp"
#include "errorstr.hpp"
#include "fastmem.hpp"
#include "fixed.h"
#include "mm4.hpp"
#include "crcio.hpp"
#include "key.hpp"
#include "events.hpp"
#include "lgraph.hpp"
#include "vgatool3.hpp"
#include "kaos.hpp"
#include "doors.hpp"
#include "objects.hpp"
#include "objects2.hpp"
#include "player.hpp"
#include "3dengine.hpp"
#include "3dsdriv.hpp"
#include "3dsound.hpp"
#include "lfont2.hpp"
#include "infobar.hpp"
#include "strlist.hpp"

const char *KaosFirm = "KAOS V0.99 - (C)1997  The Millennium Group";
const int KaosCheck = 3095;  
#define	NicCheck 1304
#define	MaxCheck 1249
#define	AndCheck 1218
const char *NicFirm = "DOMENICO VALENTINI",
	 *MaxFirm = "MASSIMILIANO TESO",
	 *AndFirm = "ANDREA FIORENTINI";

 
#define	MINGAMMA		-6
#define	STDGAMMA		0
#define MAXGAMMA		6

const char *levfile = "KAOS";
const char idDLF[9] = "NICdlf\x7\x1a";
const char idGAM[9] = "NICgam\x7\x1a";
const char idDEM[9] = "NICdem\x7\x1a";

int origpal, castlepal;
PPal startpal = new char[768],
	 endpal = new char[768],
	 gammapal = new char[768];

TFig ArmFig[6];

TMapInfo *Map 		= new TMapInfo[4096];
TObjInfo *ObjMap 	= new TObjInfo[4096];
std::int16_t *IDMap			= new std::int16_t[4096];
char *LightMap		= new char[4096];
 
#ifndef __DATAFILE__
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
T_ROCKZ 		,T_ROCKSS ,
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
#endif
int changewtext,changeftext;	 

TFigure FigList[MAXFIGURES];

 
 
static_assert(sizeof(TMapInfo) == 2, "TMapInfo must remain one 16-bit map word");
static_assert(sizeof(TObjInfo) == 2, "TObjInfo must remain one 16-bit object-map word");
static_assert(sizeof(std::int16_t) == 2, "signed map IDs must remain 16-bit");

TFig RedKey, YellowKey;

#define	SqrLumaca	0
TFig Sqrs[11];

TFig *litnumf 	= new TFig[10],
	 *normnumf 	= new TFig[10],
	 *bignumf 	= new TFig[10],
	 *stdnumf  	= new TFig[10];
TFig *happyface = new TFig[17];

char ValidGame = 0,
	  
	 init_pause = 0,
	 gamma = STDGAMMA;
char initlev = 0;
 
char players = 0;
#define STDGFLAGS 	(GFL_AUTOCENTER|GFL_SHOWBLOOD)
byte gameflags = STDGFLAGS;
char useslim = 0, slowmode = 0;

char quake = 0;			 
int windsound = -1;		 
int sursound = -1;		 
int action, actioncount;	 
							 
							 
word maxdim = 320,		 
	viewdim[3] = {320,152,148};
int palanimstate = 0,	 
	palanimcount = 0;
int cbackg0,fbackg0,	 
	cbackg1,fbackg1,
	fbacki[MAXBACKGROUND][2],cbacki[MAXBACKGROUND][2],
	dragon1[4],dragon2[4],castle[4];  
int corner[8];
int thunder,					 
	volume_bar, volume_sel,		 
	toggle_on, toggle_off,		 
	woodblock, kidwarn,
	levelblock, matchblock,
	sl_greetings, sl_quotes,
	sl_ordering, sl_help,
	sl_info, sl_messages;

Font *Std, *Big;  
int menustate = 0,
	oldmenustate = -1,
	menuitem = 0;
char viewstate = 0,
	 repaint = 1;
int numenemies = 0;
 
std::int16_t Kaos_count;		 
int minibox_I, minibox_F;
char s_autocenter = 1, s_Kaos_enable = 1;
char s_fullscreen = 1;

char message[3][80] = {"","",""};
char message_num=0, message_count=0;

Actor	*PlayAr[3] = {NULL,NULL,NULL};
InfoBar *InfoBarAr[3] = {NULL,NULL,NULL};
struct {int vidx,vidy;	 
		char cd, fd;} PlayVid[3] = {
			{160,84,0,0},{160,100,0,0},{160,100,0,0}
		};
TPlayerData PlayerData[3];
int KAOSMark[4];
int MNID_Nico, MNID_Maxy, MNID_Andy;
int PlayerAscia[3],PlayerPist[3],PlayerFuc[3],PlayerMitr[5],PlayerBaz;
int WorkID[3], KeyLeft, KeyRight;
char levnum, levname[24];

 
word playbackwait = MAXPLAYBACKWAIT;
char saveplayback = 0;
int playbackhandle = -1;

 
std::atomic<byte> Sync{0};

 


std::uint64_t kaos_sim_tic = 0;
volatile char SyncTrig = 1;
char SyncOk = 0;

TControl PControl[4] = {{
					KB_W,		KB_S,		KB_A, 	KB_D,
					KB_SPACE, 	KB_CTRL, 	KB_RSHIFT,	KB_ALT,
					KB_ENTER, 	KB_t3,		KB_t1,		KB_CANC,
					KB_Z
				},{
					KB_E,		KB_D,		KB_S,		KB_F,
					KB_TAB,		KB_A,		KB_Q,		KB_Z,
					KB_BSLASH,	KB_4,		KB_3,		KB_2,
					KB_1
				},{
					KB_t5,		KB_DOWN, 	KB_t1,		KB_t3,
					KB_RIGHT,	KB_t0,		KB_UP,		KB_LEFT,
					KB_tPLUS,  	KB_MINUS,	KB_PER,		KB_SLASH,
					KB_t7
				},{
					KB_P,		KB_O_,		KB_L,		KB_A_,
					KB_J,		KB_K,		KB_U,		KB_H,
					KB_COMMA,	KB_13,		KB_12,		KB_0,
					KB_9
				}
};

ObjectsList objectslist;
ActorsList actorslist;
DoorManager	doormanager;

#pragma warn -rvl
int xytoidx(fixed x, fixed y) {
    const int ix = static_cast<int>((static_cast<std::uint32_t>(x) >> 16) & 0x7fc0u);
    const int iy = static_cast<int>((static_cast<std::uint32_t>(y) >> 22) & 0x03ffu);
    return ix + iy;
}
#pragma warn +rvl

int is_nearPlayer(fixed x, fixed y, fixed maxdist) {
	Actor *plyr;
	for (int i=0;i<players;i++) {
		plyr = PlayAr[i];
		if (LABS(plyr->mover->x - x)<maxdist &&
			LABS(plyr->mover->y - y)<maxdist)
			return plyr->getid();
	}
	return -1;
}

void drawnum(int x, int y, byte col1, byte col2,
			 TFig *numfont, int num) {
	int i;
	char s[16];
	void far *p;
	sprintf(s,"%d",ABS(num));	 
								 
	for(i=0;s[i];i++);
	if (i>0)
		for(i--;i>=0;i--) {
			TFig &fig = numfont[s[i]-'0'];
			x-=fig.dx;
			if ((p = mm_recall(fig.memidx)) != NULL)
				putlchar(x--,y,fig.dx,fig.dy,col1,col2,p);
		}
}

static char changegamma(char newgamma) {
	int i;
	PPal app, app1;

	if (newgamma>=MINGAMMA && newgamma<=MAXGAMMA) {
		gamma = newgamma;
		app = gammapal;
		app1 = (PPal)mm_recall(origpal);
		if (newgamma>STDGAMMA) {
			for (i=0;i<768;i++,app++,app1++)
				*app = *app1+((int)(63-(*app1)) * newgamma / MAXGAMMA);
		} else
		if (newgamma<STDGAMMA) {
			for (i=0;i<768;i++,app++,app1++)
				*app = *app1-((int)(*app1) * newgamma / MINGAMMA);
		} else fwmove(gammapal,app1,768);
		fwmove(startpal,gammapal,768);
		fwmove(endpal,gammapal,768);
		setpal(startpal,0,256);
	} else return 0;
	return 1;
}

static char changeviewsize(int newval) {
	char setit = 0,
		pnum = players-1;
	int dy;

	if (viewdim[pnum] > maxdim) {
		viewdim[pnum] = maxdim;
		setit++;
	}
	switch(newval) {
	case -1:  
		if (viewdim[pnum]>96) {
			if ((viewdim[pnum] -= 16) < 96)
				viewdim[pnum] = 96;
			setit++;
		}
		break;
	case 1:
		if (viewdim[pnum]<maxdim) {
			if ((viewdim[pnum] += 16) > maxdim)
				viewdim[pnum] = maxdim;
			setit++;
		}
		break;
	default:
		if (newval<96) newval=96; else
		 if (newval>maxdim) newval=maxdim;
		viewdim[pnum] = newval;
		setit++;	 
		break;
	}
	if (setit) {
		dy = viewdim[pnum]*10/19;
		if (dy > 168) dy = 168;
		setviewsize(viewdim[pnum],dy);
		Sync=0;
		repaint=1;
	}
	return setit;
}

static void loadconfig() {
	int handle;
	unsigned i;
	if ((handle = CRC_openread_user("KAOS.CFG"))>=0) {
		CRC_read(handle,&sm_soundvol,2);
		 
		 
		CRC_read(handle,&i,2);
		sbmix_setvol(MIXER_MIDI,i);
		CRC_read(handle,&viewdim,2+2+2);
		CRC_read(handle,&gamma,sizeof(gamma));
		changegamma(gamma);
		CRC_read(handle,&sm_lrinverted,1);
		CRC_read(handle,&gameflags,sizeof(gameflags));
#ifndef __SHAREWARE__
		if (CRC_remaining(handle) >= 4)
			CRC_read(handle,&s_fullscreen,1);
		else
			s_fullscreen = 1;
#else
		if (CRC_remaining(handle) >= 3)
			CRC_read(handle,&s_fullscreen,1);
		else
			s_fullscreen = 1;
#endif
		CLRFLAG(gameflags,GFL_TOTALMAP);
		s_Kaos_enable = GETFLAG(gameflags,GFL_KAOSENABLE);
		s_autocenter = GETFLAG(gameflags,GFL_AUTOCENTER);
#ifndef __SHAREWARE__
		CRC_read(handle,&initlev,1);
		if (initlev>12 || initlev<-16) initlev=0;
#else
		initlev=0;	 
#endif
		CRC_close(handle);
	} else changegamma(STDGAMMA);
}

static void saveconfig() {
	int handle;

	if ((handle = CRC_openwrite_user("KAOS.CFG"))>=0) {
		 
		CRC_write(handle,&sm_soundvol,2);
		CRC_write(handle,&SBdefault[MIXER_MIDI],2);
		CRC_write(handle,&viewdim,2+2+2);
		CRC_write(handle,&gamma,sizeof(gamma));
		 
		CRC_write(handle,&sm_lrinverted,1);
		MAKEFLAG(gameflags,GFL_KAOSENABLE,s_Kaos_enable);
		MAKEFLAG(gameflags,GFL_AUTOCENTER,s_autocenter);
		CRC_write(handle,&gameflags,sizeof(gameflags));
		s_fullscreen = kaos_sdl_get_fullscreen() ? 1 : 0;
		CRC_write(handle,&s_fullscreen,1);
#ifndef __SHAREWARE__
		CRC_write(handle,&initlev,1);
#endif
		CRC_close(handle);
	}
}

#ifdef __DATAFILE__
static void near pre_loadtext() {
	TMapInfo *mappa = Map;
	byte *fmap = floormap, *cmap = ceilmap;
	int i;
	soundmanager(-1,0,0);
	mm_recall(cbackg0); mm_recall(fbackg1);
	mm_recall(cbackg1); mm_recall(fbackg0);
	objectslist.preloadall();
	for (i=4096; i--; mappa++) {
		if (!(i & 255)) soundmanager(-1,0,0);
		if (mappa->stop)
			switch(mappa->type) {
				case MTP_DOOR1:
				case MTP_DOOR2:
					mm_recall(doormanager.doortype[mappa->data]);
					break;
				case MTP_GRID1:
					mm_recall(mappa->data);
					break;
				default:
					mm_recall(mappa->data);
			}
		mm_recall(*(fmap++));
		mm_recall(*(cmap++));
	}
}
#endif

static Object * near wlkobj(fixed x, fixed y, word fig, char takeable) {
	Object *object;
	delete mm_reserve(sizeof(Object));
	object = new Object(x,y,fig,1,MVT_0);
	object->mover->flags &= ~(OMF_UNWALKABLE | OMF_HITABLE);
	if (takeable) object->oflags |= OFL_TAKEABLE;
	objectslist.put(object);
	return object;
}

 
static void near adjustmap() {
	TMapInfo *mappa = Map;
	int x, y, fig;
	fixed fx, fy;
	fixed p[6];
	int dir[3] = {ANG0,ANG0,ANG0};
	Object *object;
	byte data;
	word ogg = OMF_STANDARD;
	word *oggmap = objmap;
	int pvalid = 0;

	numenemies = 0;
	for (x=0;x<64;x++)
	 for (y=0;y<64;y++,mappa++,oggmap++) {
		if (!mappa->stop) {
		 if (mappa->none) {
			fx = lshl16((x<<6)+32);
			fy = lshl16((y<<6)+32);
			 
			if (mappa->data >= T_TEXTURES) {
				if (mappa->data < T_OBJECTS) {
					switch(data = mappa->data-T_TEXTURES) {
						case 0:	 
							wlkobj(fx,fy,F_FUCILE,1);
							break;
						case 1:  
							wlkobj(fx,fy,F_MITRA,1);
							break;
						case 2:  
							wlkobj(fx,fy,F_BAZOOKA,1);
							break;
						case 3:  
							delete mm_reserve(sizeof(Barile));
							objectslist.put(new Barile(fx,fy));
							break;
						case 4:  
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:
						case 10:
						case 11:
							delete mm_reserve(sizeof(Torcia));
							switch ((data-4) & 3) {
								case 0: fx+=(28l<<16); break;
								case 1: fy+=(28l<<16); break;
								case 2: fx-=(28l<<16); break;
								case 3: fy-=(28l<<16); break;
							}
							if (data>=8) fig = F_TORCIAM;
									else fig = F_TORCIA;
							object = new Torcia(fx,fy,fig);
							objectslist.put(object);
							break;
						case 12:  
						case 13:
							delete mm_reserve(sizeof(Object));
							if (data == 12) fig = F_COLUMN;
									   else fig = F_COLUMNM;
							object = new Object(fx,fy,fig,1,MVT_0);
							object->mover->z = 30l<<FIXSHIFT;
							object->mover->shdim = 3;
							objectslist.put(object);
							break;
						case 14:  
						case 15:
							delete mm_reserve(sizeof(Lampadario));
							if (data==14) fig = F_LAMP;
									 else fig = F_TRESPOLO;
							object = new Lampadario(fx,fy,fig);
							 






							objectslist.put(object);
							break;
						case 16:  
						case 17:
							if (data==16) fig=F_KEYRED;
									 else fig=F_KEYELLOW;
							wlkobj(fx,fy,fig,1);
							break;
						case 18:  
						case 19:
						case 20:
						case 21:
						case 22:
							switch(data) {
								case 18: fig=F_CARTUCCIA1; break;
								case 19: fig=F_CARTUCCIA2; break;
								case 20: fig=F_CARTUCCIA3; break;
								case 21: fig=F_CARTUCCIA4; break;
								case 22: fig=F_BOMB; break;
							}
							wlkobj(fx,fy,fig,1);
							break;
						case 23:  
						case 24:
							delete mm_reserve(sizeof(Ossa));
							if (data==23) fig = F_OSSALIT;
									 else fig = F_OSSABIG;
							object = new Ossa(fx,fy,fig);
							if (data==24) {
								object->mover->z = 18l<<FIXSHIFT;	 
								object->mover->shdim = 5;
							}
							objectslist.put(object);
							break;
						case 25:  
						case 26:  
						case 27:  
						case 28:  
						case 29:  
						case 30:  
						case 31:  
						case 32:  
						case 33:  
						case 34:  
						case 35:  
						case 36:  
						case 37:  
						case 38:  
						case 39:  
						case 40:  
						case 41:  
						case 42:  
						case 43:  
						case 44:  
							delete mm_reserve(sizeof(Vaso));
							if (data<35) {
								fig = F_VASO;
								data -= 25;
							} else {
								fig = F_PIANTA;
								data -= 35;
							  }
							objectslist.put(new Vaso(fx,fy,fig,data));
							 
							 
							break;
						case 45:  
							 
							delete mm_reserve(sizeof(Bonus));
							objectslist.put(new Bonus(fx,fy));
							break;
						case 46:  
						case 47:
							if (data==46) fig = F_FARMABIG;
									 else fig = F_FARMALIT;
							wlkobj(fx,fy,fig,1);
							break;
						case 48:  
						case 49:
							if (data==48) fig = F_SOLDOBIG;
									 else fig = F_SOLDOLIT;
							wlkobj(fx,fy,fig,1);
							break;
						case 50:  
						case 51:
							delete mm_reserve(sizeof(Object));
							 



							fig = F_ALBERO+data-50;
							object = new Object(fx,fy,fig,1,MVT_0);
							object->mover->z = 28l<<FIXSHIFT;
							object->mover->shdim = 4;
							objectslist.put(object);
							break;
						case 52:  
							delete mm_reserve(sizeof(Torcia));
							objectslist.put(new Torcia(fx,fy,F_BRAZIER));
							break;
						case 53:  
							delete mm_reserve(sizeof(AntiKaos));
							objectslist.put(new AntiKaos(fx,fy));
							break;
					}
				} else
				if (mappa->data < T_ENEMIES) {
					switch(data = mappa->data-T_OBJECTS) {
						case 0:  
							delete mm_reserve(sizeof(Slimer));
							objectslist.put(new Slimer(fx,fy)); break;
						case 1:  
#ifdef __ALLOWENEMY__
							delete mm_reserve(sizeof(Enemy));
							objectslist.put(new Enemy(fx,fy,F_FACE));
#else
							 
#endif
							break;
						case 2:  
							delete mm_reserve(sizeof(Spider));
							objectslist.put(new Spider(fx,fy)); break;
						case 3:  
							delete mm_reserve(sizeof(Soldier));
							objectslist.put(new Soldier(fx,fy)); break;
						case 4:  
							delete mm_reserve(sizeof(Mouse));
							objectslist.put(new Mouse(fx,fy)); break;
						case 5:  
#ifdef __ALLOWENEMY__
							delete mm_reserve(sizeof(Enemy));
							objectslist.put(new Enemy(fx,fy,F_BUB));
#else
							 
#endif
							break;
						default:
							switch(data) {
								case  6: fig = F_SLIMER+21;  	 
									break;
								case  7: fig = F_FACE+16; 		 
									break;
								case  8: fig = F_SPIDER+18;  	 
									break;
								case  9: fig = F_SOLDIER+34;  	 
									break;
								case 10: fig = F_MOUSE+9;	  	 
									break;
								case 11: fig = F_BUB+19;  		 
									break;
							}
							delete mm_reserve(sizeof(Object));
							object = new Object(fx,fy,fig,1,MVT_0);
							object->mover->z = lshl16(static_cast<std::int16_t>(
								-FigList[fig].height-(FigList[fig].dy>>3)));
							CLRFLAG(object->mover->flags,OMF_UNWALKABLE|OMF_HITABLE);
							objectslist.put(object);
							break;
					}
				} else
				if (mappa->data < T_PLAYERS) {
					data = mappa->data-T_ENEMIES;
					if (data<12) {
						fig = (data>>1) & 0xfffe;
						p[fig]=fx; p[fig+1]=fy;
						fig>>=1;
						pvalid |= 1<<fig;
						switch(data & 3) {
							case 0: dir[fig] = ANG0; break;
							case 1: dir[fig] = ANG90; break;
							case 2: dir[fig] = ANG180; break;
							case 3: dir[fig] = ANG270; break;
						}
					} else error("adjustmap","unknown Player");
				} else error("adjustmap","unknown Object");
			} else error("adjustmap","invalid map");
			mappa->data = 0;
			 
			mappa->type = 0;
			mappa->shape= 0;
			mappa->none = 0;
		 }
		} else {
			switch(mappa->type) {
				case MTP_DOOR1:
				case MTP_DOOR2:
					mappa->data = doormanager.add(mappa->data);
					 
					break;
				case MTP_GRID1:
					ogg &= ~OMF_SOUNDABLE;
					break;
			}
			if (mappa->data == T_NULL+1) {
				mappa->data--;
				mappa->stop=0;
				ogg = OMF_UNWALKABLE;
			}
			*oggmap += ogg;
			ogg = OMF_STANDARD;
		  }
	 }

	doormanager.makeslots();

	switch (players) {
		case 1:
			if (!(pvalid & 1))
				error("adjustmap","no Player");
			delete mm_reserve(sizeof(Player));
			if (ValidGame)
				PlayAr[0] = new Player(p[0],p[1],dir[0],PlayerData[0],0);
			else
				PlayAr[0] = new Player(p[0],p[1],dir[0],0);
			objectslist.put(PlayAr[0]);

			if (!playbackwait) {
				((Player *)PlayAr[0])->setcomtype(COM_LOAD);
			} else
			if (saveplayback) {
				((Player *)PlayAr[0])->setcomtype(COM_SAVE);
			}
			delete mm_reserve(sizeof(InfoBar));
			InfoBarAr[0] = new InfoBar(PlayAr[0]->getid());
			break;
		case 2:
			if ((pvalid & 3) != 3)
				error("adjustmap","no 2 Players");
			delete mm_reserve(sizeof(Player));
			if (ValidGame)
				PlayAr[0] = new Player(p[0],p[1],dir[0],PlayerData[0],1);
			else
				PlayAr[0] = new Player(p[0],p[1],dir[0],1);

			if (!useslim) {
				delete mm_reserve(sizeof(Player));
				if (ValidGame)
					PlayAr[1] = new Player(p[2],p[3],dir[1],PlayerData[1],2);
				else
					PlayAr[1] = new Player(p[2],p[3],dir[1],2);
			} else {
				delete mm_reserve(sizeof(Slimer));
				PlayAr[1] = new Slimer(p[2],p[3]);
				PlayAr[1]->seen();
			  }
			objectslist.put(PlayAr[0]);
			objectslist.put(PlayAr[1]);
			if (!playbackwait) {
				((Player *)PlayAr[0])->setcomtype(COM_LOAD);
				((Player *)PlayAr[1])->setcomtype(COM_LOAD);
			} else
			if (saveplayback) {
				((Player *)PlayAr[0])->setcomtype(COM_SAVE);
				((Player *)PlayAr[1])->setcomtype(COM_SAVE);
			}
			delete mm_reserve(sizeof(LittleInfoBar));
			InfoBarAr[0] = new LittleInfoBar(PlayAr[0]->getid(),4,176);
			if (!useslim) {
			delete mm_reserve(sizeof(LittleInfoBar));
			InfoBarAr[1] = new LittleInfoBar(PlayAr[1]->getid(),163,92);
			}
			break;
		case 3:
			if ((pvalid & 7) != 7)
				error("adjustmap","no 3 Players");
			delete mm_reserve(sizeof(Player));
			if (ValidGame)
				PlayAr[0] = new Player(p[0],p[1],dir[0],PlayerData[0],1);
			else
				PlayAr[0] = new Player(p[0],p[1],dir[0],1);
			if (!useslim) {
				delete mm_reserve(sizeof(Player));
				if (ValidGame)
					PlayAr[1] = new Player(p[2],p[3],dir[1],PlayerData[1],2);
				else
					PlayAr[1] = new Player(p[2],p[3],dir[1],2);
			} else {
				delete mm_reserve(sizeof(Slimer));
				PlayAr[1] = new Slimer(p[2],p[3]);
				PlayAr[1]->seen();
			  }
			if (!useslim) {
				delete mm_reserve(sizeof(Player));
				if (ValidGame)
					PlayAr[2] = new Player(p[4],p[5],dir[2],PlayerData[2],3);
				else
					PlayAr[2] = new Player(p[4],p[5],dir[2],3);
			} else {
				delete mm_reserve(sizeof(Slimer));
				PlayAr[2] = new Slimer(p[4],p[5]);
				PlayAr[2]->seen();
			  }
			objectslist.put(PlayAr[0]);
			objectslist.put(PlayAr[1]);
			objectslist.put(PlayAr[2]);
			if (!playbackwait) {
				((Player *)PlayAr[0])->setcomtype(COM_LOAD);
				((Player *)PlayAr[1])->setcomtype(COM_LOAD);
				((Player *)PlayAr[2])->setcomtype(COM_LOAD);
			} else
			if (saveplayback) {
				((Player *)PlayAr[0])->setcomtype(COM_SAVE);
				((Player *)PlayAr[1])->setcomtype(COM_SAVE);
				((Player *)PlayAr[2])->setcomtype(COM_SAVE);
			}
			delete mm_reserve(sizeof(LittleInfoBar));
			InfoBarAr[0] = new LittleInfoBar(PlayAr[0]->getid(),4,180);
			if (!useslim) {
			delete mm_reserve(sizeof(LittleInfoBar));
			InfoBarAr[1] = new LittleInfoBar(PlayAr[1]->getid(),164,180);
			delete mm_reserve(sizeof(LittleInfoBar));
			InfoBarAr[2] = new LittleInfoBar(PlayAr[2]->getid(),84,80);
			}
			break;
	}
#ifdef __DATAFILE__
	pre_loadtext();
#endif
}

char initgame(int plrs) {
	int i;

	if (plrs != players) {

		switch (plrs) {
		case 1:
			maxdim=320;
			PlayVid[0].vidx = 160;
			PlayVid[0].vidy = 84;
			break;
#ifndef __SHAREWARE__
		case 2:
			maxdim=152;
			PlayVid[0].vidx = 80;
			PlayVid[0].vidy = 130;
			PlayVid[1].vidx = 239;
			PlayVid[1].vidy = 46;
			break;
		case 3:
			maxdim=148;
			PlayVid[0].vidx = 80;
			PlayVid[0].vidy = 142;
			PlayVid[1].vidx = 240;
			PlayVid[1].vidy = 142;
			PlayVid[2].vidx = 160;
			PlayVid[2].vidy = 42;
			break;
#endif
		default: return 0;
		}

		players = plrs;
		changeviewsize(viewdim[plrs-1]);
	}
	return 1;
}

char delgame() {
	int i;
	if (ValidGame) {
		for (i=0;i<players;i++)
			if (PlayAr[i]) {
				((Player *)PlayAr[i])->getdata(PlayerData[i]);
				 
				PlayAr[i] = NULL;
			}
		 
		ShutDown=1;  
		for (i=0;i<3;i++)
			if (InfoBarAr[i]) {
				delete InfoBarAr[i];
				InfoBarAr[i]=NULL;
			}
		objectslist.eraseall();
		actorslist.eraseall();
		doormanager.eraseall();
		eventmanager.flush();
		stopallsounds();
		windsound=-1;
		numenemies=0;
		ShutDown=0;
		ValidGame=0;
		return 1;
	}
	return 0;
}

void drawdisk() {
	setvbuff(VMEMPTR);
	storerfig(8,8,32,24,mm_recall(Sqrs[10].memidx));
	setvbuff(vpage);
	repaint = 1;
}

void drawpict(int *pictpiece, void far *dest) {
	int i;
	void far *ptr;
	for (i=0;i<4;i++) {
		if ((ptr = mm_recall(pictpiece[i])) != NULL) {
			setvbuff(ptr);
			fcopyarea(0,i*50,50,dest);
		} else ffillarea(i*50,50,0x7f7f7f7fl);
	}
	setvbuff(vpage);
}

void drawcorners(int x, int y) {
	int i,app;
	app = x+viewwidth-24;
	setvrect(0,0,320,200);
	storelcfig(app,y-4,24,4,mm_recall(corner[4]));
	storelcfig(app,y+viewheight,24,4,mm_recall(corner[7]));
	for(i=0,app=x;i<viewwidth/24;i++,app+=24) {
		storelcfig(app,y-4,24,4,mm_recall(corner[4]));
		storelcfig(app,y+viewheight,24,4,mm_recall(corner[7]));
	}
	app = y+viewheight-24;
	storelcfig(x-4,app,4,24,mm_recall(corner[5]));
	storelcfig(x+viewwidth,app,4,24,mm_recall(corner[6]));
	for(i=0,app=y;i<viewheight/24;i++,app+=24) {
		storelcfig(x-4,app,4,24,mm_recall(corner[5]));
		storelcfig(x+viewwidth,app,4,24,mm_recall(corner[6]));
	}
	storelcfig(x-4,y-4,4,4,mm_recall(corner[0]));
	storelcfig(x+viewwidth,y-4,4,4,mm_recall(corner[1]));
	storelcfig(x-4,y+viewheight,4,4,mm_recall(corner[2]));
	storelcfig(x+viewwidth,y+viewheight,4,4,mm_recall(corner[3]));
}

void lockscreen() {
	drawpict(dragon1,VMEMPTR);
	setvbuff(VMEMPTR);
	Big->colorwrite(16,8,50,"KAOS Policy Subsystem");
	Big->colorwrite(32, 36,2,"Il Millennium Group ha");
	Big->colorwrite(32, 58,2,"limitato la diffusione");
	Big->colorwrite(32, 80,2,"di questi nuovi livelli.");
	Big->colorwrite(8,106,4,"Non hai i privilegi per poter");
	Big->colorwrite(8,128,4,"giocare con questo episodio.");
	setvbuff(vpage);
	kb_onepressed(KB_ESC);
	kb_onepressed(KB_ENTER);
	kb_onepressed(KB_SPACE);
	while (!kb_onepressed(KB_ESC) && !kb_onepressed(KB_ENTER) &&
			!kb_onepressed(KB_SPACE)) ;
	 
	repaint = 1;
}

void congratulations() {
	drawpict(dragon1,VMEMPTR);
	setvbuff(VMEMPTR);
	Big->colorwrite(47,  7,48,"Episodio completato");
	Big->colorwrite(48,  8,50,"Episodio completato");
	Big->colorwrite(32, 48,2,"Congratulazioni !!!");
	Big->colorwrite(32, 72,2,"Sei uscito dal mondo");
	Big->colorwrite(32, 96,2,"del Kaos");
	 
	 
	 



	setvbuff(vpage);
	kb_onepressed(KB_ESC);
	kb_onepressed(KB_ENTER);
	kb_onepressed(KB_SPACE);
	while (!kb_onepressed(KB_ESC) && !kb_onepressed(KB_ENTER) &&
			!kb_onepressed(KB_SPACE)) ;
	 
	repaint = 1;
}

 
char loadmap(int level) {
	char id[9] = "........";
	int handle, i, j;
	byte num;
	 
	sprintf(levname,"%s.DLF",levfile);
	 
	 

	if ((handle = CRC_openread(levname)) >= 0) {
		drawdisk();
#ifdef __DATAFILE__
	mm_flushall();
#ifdef __VISUALIZE__
	mm_visualize();
	sound(500);delay(50);nosound();delay(50);
#endif
#else
	mm_resetall();
#endif
		if (delgame()) {
			ValidGame=1;
			 
			 
		}
		CRC_read(handle,&id,8);
		if (fwcomp(id,(void *)&idDLF,8)) {
			CRC_read(handle,&num,sizeof(num));
			for (j=0,i=0;j<num;j++) {
				CRC_read(handle,&i,1);
				if (i == level) break;
				 
				CRC_lseek(handle,24l+1l +8192l+4096l+4096l,SEEK_CUR);
			}
			if (i == level) {
				levnum = level;
				CRC_read(handle,levname,24);
				CRC_read(handle,&i,1);  
				underblack = i & 0x80;
				raindraw = i & 0x40;
				i &= 0x0f;
				if (i<0 || i>=MAXBACKGROUND) i=0;
				cbackg0 = cbacki[i][0];
				fbackg0 = fbacki[i][0];
				cbackg1 = cbacki[i][1];
				fbackg1 = fbacki[i][1];
				 
				 
				CRC_read(handle,Map,8192);
				CRC_read(handle,floormap,4096);
				CRC_read(handle,ceilmap,4096);
			} else {
				CRC_close(handle);
				return ValidGame=0;
			  }
#ifdef __CHECKFIRM__
		   
			for (j++;j<num;j++)
				CRC_lseek(handle,24l+1l +8192l+4096l+4096l,SEEK_CUR);
			std::int16_t firm_count;
			CRC_read(handle,&firm_count,sizeof(firm_count));
			std::uint32_t firm;
			for (j=0;j<firm_count;j++) {	 
				CRC_read(handle,&firm,sizeof(firm));
				if (firm == SerialNo) {
					CRC_close(handle);
					lockscreen();
					return ValidGame=0;
				}
			}
		   
#endif
			CRC_close(handle);
			fdfill(ObjMap,0l,8192);
			fdfill(IDMap,0xffffffffl,8192);
			fdfill(LightMap,0l,4096);
			adjustmap();
		} else error("loadmap",err_invalidformat);
	} else error("loadmap",err_filenotfound);
	return ValidGame=1;  
}

char near loadgamedat( int level) {
	char id[9] = "........";
	int handle, i,j;
	byte num;
	 
	sprintf(levname,"%s.DLF", levfile);
	 
	 

	if ((handle = CRC_openread(levname)) >= 0) {
		 
		CRC_read(handle,&id,8);
		if (fwcomp(id,(void *)&idDLF,8)) {
			CRC_read(handle,&num,sizeof(num));
			for (j=0,i=0;j<num;j++) {
				CRC_read(handle,&i,1);
				if (i == level) break;
				 
				CRC_lseek(handle,24l+1l +8192l+4096l+4096l,SEEK_CUR);
			}
			if (i == level) {
				levnum = level;
				CRC_read(handle,levname,24);
				CRC_read(handle,&i,1);  
				underblack = i & 0x80;
				raindraw = i & 0x40;
				i &= 0x0f;
				if (i<0 || i>=MAXBACKGROUND) i=0;
				cbackg0 = cbacki[i][0];
				fbackg0 = fbacki[i][0];
				cbackg1 = cbacki[i][1];
				fbackg1 = fbacki[i][1];
				 
				 
				 
				CRC_lseek(handle,8192l+4096l,SEEK_CUR);
				 
				CRC_read(handle,ceilmap,4096);
			} else {
				CRC_close(handle);
				return 0;
			  }
#ifdef __CHECKFIRM__
		   
			for (j++;j<num;j++)
				CRC_lseek(handle,24l+1l +8192l+4096l+4096l,SEEK_CUR);
			std::int16_t firm_count;
			CRC_read(handle,&firm_count,sizeof(firm_count));
			std::uint32_t firm;
			for (j=0;j<firm_count;j++) {	 
				CRC_read(handle,&firm,sizeof(firm));
				if (firm == SerialNo) {
					lockscreen();
					CRC_close(handle);
					return 0;
				}
			}
		   
#endif
			CRC_close(handle);
		} else {
			CRC_close(handle);
			return 0;
		  }
		 
	} else return 0;
		 
	return 1;  
}

char loadgame(int game) {
	if (game<0 || game>7) return 0;
	int handle, i, j;
	char s[32], lev, plrnum;
	std::int16_t plrids[3];
	byte objtype;
	Object *obj;

	sprintf(s,"KAOS%1d.GAM",game);
	if ((handle = CRC_openread_user(s)) >= 0) {
		drawdisk();

		CRC_read(handle,s,8);
		if (!fwcomp(s,(void *)&idGAM,8)) {CRC_close(handle); return 0;}
		CRC_read(handle,s,32);
		CRC_read(handle,&plrnum,sizeof(plrnum));
		CRC_read(handle,&plrids,2+2+2);	 
		CRC_read(handle,&objtype,sizeof(objtype));
		MAKEFLAG(gameflags,GFL_COMPETITIVE,GETFLAG(objtype,GFL_COMPETITIVE));
		MAKEFLAG(gameflags,GFL_KAOSENABLE,GETFLAG(objtype,GFL_KAOSENABLE));
		CRC_read(handle,s,8); s[8]=0;
		if (strcmp(s,levfile)) {CRC_close(handle); return 0;}
#ifdef __DATAFILE__
		mm_flushall();
#else
		mm_resetall();
#endif
		delgame();
		if (!initgame(plrnum)) {CRC_close(handle); return 0;}
		CRC_read(handle,&lev,sizeof(lev));
		if (!loadgamedat(lev)) {CRC_close(handle); return 0;}
		 
		CRC_read(handle,Map,8192);
		CRC_read(handle,ObjMap,8192);
		CRC_read(handle,IDMap,8192);
		CRC_read(handle,LightMap,4096);
		CRC_read(handle,floormap,4096);
		 

		doormanager.load(handle);
		numenemies = 0;
		do {
			 
			 
			CRC_read(handle,&objtype,sizeof(objtype));
			if (objtype==OBJT_END) break;
			switch (objtype) {
				case OBJT_OBJECT:
					delete mm_reserve(sizeof(Object));
					obj = new Object(handle);
					break;
				case OBJT_ACTOR:
					delete mm_reserve(sizeof(Actor));
					obj = new Actor(handle);
					break;
				case OBJT_LIGHTACTOR:
					delete mm_reserve(sizeof(LightActor));
					obj = new LightActor(handle);
					break;
				case OBJT_CANFIREACTOR:
					delete mm_reserve(sizeof(CanFireActor));
					obj = new CanFireActor(handle);
					break;
				case OBJT_OSSA:
					delete mm_reserve(sizeof(Ossa));
					obj = new Ossa(handle);
					break;
				case OBJT_FLOSSA:
					delete mm_reserve(sizeof(FlOssa));
					obj = new FlOssa(handle);
					break;
				case OBJT_BONUS:
					delete mm_reserve(sizeof(Bonus));
					obj = new Bonus(handle);
					break;
				case OBJT_SLIMER:
					delete mm_reserve(sizeof(Slimer));
					obj = new Slimer(handle);
					break;
				case OBJT_SPIDER:
					delete mm_reserve(sizeof(Spider));
					obj = new Spider(handle);
					break;
				case OBJT_SOLDIER:
					delete mm_reserve(sizeof(Soldier));
					obj = new Soldier(handle);
					break;
				case OBJT_MOUSE:
					delete mm_reserve(sizeof(Mouse));
					obj = new Mouse(handle);
					break;
				case OBJT_FIREBOOM:
					delete mm_reserve(sizeof(FireBoom));
					obj = new FireBoom(handle);
					break;
				case OBJT_FIREBALL:
					delete mm_reserve(sizeof(Fireball));
					obj = new Fireball(handle);
					break;
				case OBJT_TORCIA:
					delete mm_reserve(sizeof(Torcia));
					obj = new Torcia(handle);
					break;
				case OBJT_LAMPADARIO:
					delete mm_reserve(sizeof(Lampadario));
					obj = new Lampadario(handle);
					break;
				case OBJT_BARILE:
					delete mm_reserve(sizeof(Barile));
					obj = new Barile(handle);
					break;
				case OBJT_VASO:
					delete mm_reserve(sizeof(Vaso));
					obj = new Vaso(handle);
					break;
				case OBJT_PLAYER:
					 
					 
					 
					 
					delete mm_reserve(sizeof(Player));
					obj = new Player(handle);

					j = obj->getid();	 
					for (i=0;i<3;i++)
						if (plrids[i]==j) {
							PlayAr[i] = (Player *)obj;
							delete mm_reserve(sizeof(InfoBar));
							InfoBarAr[i]=new InfoBar(PlayAr[i]->getid());
							i=-1;
							break;
						}
					if (i>=0) error("loadgame","invalid Player slot");
					break;
				case OBJT_OBJMOVER:
					delete mm_reserve(sizeof(ObjMover));
					obj = new ObjMover(handle);
					break;
				case OBJT_BOMB:
#ifndef __SHAREWARE__
					delete mm_reserve(sizeof(Bomb));
					obj = new Bomb(handle);
#endif
					break;
				case OBJT_ANTIKAOS:
					delete mm_reserve(sizeof(AntiKaos));
					obj = new AntiKaos(handle);
					break;
				default: error("loadgame","wrong Object type");
			}
			objectslist.loadobj(obj,objtype);
		} while (1);	 
		 
		CRC_read(handle,&Kaos_count,sizeof(Kaos_count));
		 
		CRC_close(handle);
#ifdef __DATAFILE__
		pre_loadtext();
#endif
	} else return 0;
	return 1;
}

char savegame(int game, char *name) {
	if (game<0 || game>7) return 0;
	int handle,i;
	std::int16_t plrids[3] = {-1,-1,-1};
	char s[16];
	sprintf(s,"KAOS%1d.GAM",game);
	if ((handle = CRC_openwrite_user(s)) >= 0) {
		drawdisk();

		CRC_write(handle,(void *)&idGAM,8);
		CRC_write(handle,name,32);
		CRC_write(handle,&players,sizeof(players));
		for (i=0;i<players;i++) {
			if (PlayAr[i])
				plrids[i] = PlayAr[i]->getid();
			else error("savegame","no Player");
		}
		CRC_write(handle,&plrids,2+2+2);	 
		CRC_write(handle,&gameflags,sizeof(gameflags));
		CRC_write(handle,levfile,8);
		CRC_write(handle,&levnum,sizeof(levnum));
		CRC_write(handle,Map,8192);
		CRC_write(handle,ObjMap,8192);
		CRC_write(handle,IDMap,8192);
		CRC_write(handle,LightMap,4096);
		CRC_write(handle,floormap,4096);
		 
		doormanager.save(handle);
		objectslist.save(handle);
		 
		CRC_write(handle,&Kaos_count,sizeof(Kaos_count));
		 
		if (!CRC_close(handle)) return 0;
	} else return 0;
	return 1;
}

 
void end_playback() {
	if (playbackhandle>=0) {
		CRC_close(playbackhandle);
		playbackhandle = -1;
	}
}

char play_playback(int num) {
	char plrs;

	if (playbackhandle<0 && num>=0 && num<MAXPLAYBACKS) {
		char s[13];
		sprintf(s,"KAOS%1d.DEM",num);
		if ((playbackhandle = CRC_openread_user(s))>=0) {
		  CRC_read(playbackhandle,s,8);
		  if (!fwcomp(s,(void *)&idDEM,8)) {end_playback(); return 0;}
		  CRC_read(playbackhandle,s,8); s[8]=0;
		  if (!strcmp(s,levfile)) {
			CRC_read(playbackhandle,&plrs,sizeof(plrs));
			if (!initgame(plrs)) {end_playback(); return 0;}
			CRC_read(playbackhandle,&levnum,sizeof(levnum));
			CRC_read(playbackhandle,&plrs,sizeof(plrs));
			MAKEFLAG(gameflags,GFL_COMPETITIVE,GETFLAG(plrs,GFL_COMPETITIVE));
			MAKEFLAG(gameflags,GFL_KAOSENABLE,GETFLAG(plrs,GFL_KAOSENABLE));
			MAKEFLAG(gameflags,GFL_AUTOCENTER,GETFLAG(plrs,GFL_AUTOCENTER));
			playbackwait = 0;	 
			return 1;
		  }
		}
	}
	end_playback();
	return 0;
}

char record_playback(int num) {
	if (playbackhandle<0 && num>=0 && num<MAXPLAYBACKS) {
		char s[13];
		sprintf(s,"KAOS%1d.DEM",num);
		if ((playbackhandle = CRC_openwrite_user(s))>=0) {
		  CRC_write(playbackhandle,(void *)&idDEM,8);
		  CRC_write(playbackhandle,levfile,8);
		  CRC_write(playbackhandle,&players,sizeof(players));
		  CRC_write(playbackhandle,&levnum,sizeof(levnum));
		  CRC_write(playbackhandle,&gameflags,sizeof(gameflags));
		  playbackwait = MAXPLAYBACKWAIT;
		  return 1;
		}
	}
	return 0;
}
 

void allocfig(int fignum, int x, int y, int dx, int dy, int h) {
	void far *data;
	FigList[fignum].dx = dx;
	FigList[fignum].dy = dy;
	FigList[fignum].height = h;
#ifdef __DATAFILE__
	FigList[fignum].memidx = mm_alloc(dx*dy);
#else
	if ((data = mm_recall(FigList[fignum].memidx = mm_alloc(dx*dy))) != NULL)
		getrfig(x,y,dx,dy,data);
	else error("allocfig",err_notenoughmemory);
#endif
}

void init() {
	 
	int i,j;
	long l;
	const char *text;

	for (i=0,j=0,text=NicFirm;i<18;i++) j+=*(text++);
	if (j!=NicCheck)  
		error("init","file corrupt");
	for (i=0,j=0,text=MaxFirm;i<17;i++) j+=*(text++);
	if (j!=MaxCheck)  
		error("init","file corrupt");
	for (i=0,j=0,text=AndFirm;i<17;i++) j+=*(text++);
	if (j!=AndCheck)  
		error("init","file corrupt");
	for (j=0,text=KaosFirm;*text;) j+=*(text++);
	if (j!=KaosCheck)  
		error("init","file corrupt");
	 

	initsounds();

	randomize();
	printf("\n%s",KaosFirm);
	word memo = (word)((unsigned long)coreleft()>>10);
	printf("\n MemoryManager: base free  = %5uKb\n", memo);
			 





	if (memo<75) {
		printf("\n Not enough conventional memory, stingy man !");
		exit(EXIT_FAILURE);
	}
	memo = (word)(mm_memavail()>>10);
	printf("    (v3.3)      total free = %5uKb;\n",memo);
			 





	if (init_pause) {
		 
		printf("\n  < hit any key to start >");
		while (kbhit()) getch();
		getch();
	}
	printf("\n        WAIT PLEASE...    ");

	int idx = 0;
	for (i=0;i<MAXTEXTURES;i++) mm_alloc(4096);	 
#ifndef __DATAFILE__
	void far *data;
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
#endif

	changewtext = mm_alloc(MAXTEXTURES);
	changeftext = mm_alloc(MAXTEXTURES);
#ifndef __DATAFILE__
	fdmove(mm_recall(changewtext),ChangeWallText,MAXTEXTURES);
	fdmove(mm_recall(changeftext),ChangeFloorText,MAXTEXTURES);
#endif

#ifndef __DATAFILE__
	loadLBM("lbm\\objects.lbm",NULL);
#endif
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
	allocfig(F_VASO,69,74,21,30,-24);
	allocfig(F_PIANTA,94,72,54,59,-24);
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

	YellowKey.dx = RedKey.dx = 15;
	YellowKey.dy = RedKey.dy = 16;
	YellowKey.memidx = mm_alloc(15*16);
#ifndef __DATAFILE__
	getlfig(15,169,15,16,mm_recall(YellowKey.memidx));
#endif
	RedKey.memidx = mm_alloc(15*16);
#ifndef __DATAFILE__
	getlfig(0,169,15,16,mm_recall(RedKey.memidx));
#endif
	for (i=0;i<10;i++) {
		if (i==1) {
			bignumf[i].dx = 5; bignumf[i].dy = 11;
			bignumf[i].memidx = mm_alloc(55);
#ifndef __DATAFILE__
			getlfig(116+(i<<3),16,5,11,mm_recall(bignumf[i].memidx));
#endif
		} else {
			bignumf[i].dx = 7; bignumf[i].dy = 11;
			bignumf[i].memidx = mm_alloc(77);
#ifndef __DATAFILE__
			getlfig(114+(i<<3),16,7,11,mm_recall(bignumf[i].memidx));
#endif
		  }
	}
#ifndef __DATAFILE__
	loadLBM("lbm\\objects2.lbm",NULL);
#endif
	allocfig(F_OSSALIT,95,134,47,22,0);
	allocfig(F_OSSABIG,205,107,73,53,-25);
	allocfig(F_TESCHIO  , 95,157,11,11,0);
	allocfig(F_TESCHIO+1,107,157,10,11,0);
	allocfig(F_LAMP+2, 49,  1,46,22,0);
	allocfig(F_LAMP+3, 49, 24,49,22,0);
	allocfig(F_LAMPX , 49, 47,49,22,0);
	allocfig(F_VASO+1  ,100,  1,33,37,-24);
	allocfig(F_VASO+2  ,134,  1,44,37,-24);
	allocfig(F_VASO+3  ,179,  1,48,41,-24);
	allocfig(F_VASO+4  ,228, 23,33,20,-24);
	allocfig(F_VASO+5  ,228, 10,25,12,-24);
	allocfig(F_PIANTA+1,100, 44,50,62,-24);
	allocfig(F_PIANTA+2,151, 44,61,51,-24);
	allocfig(F_PIANTA+3,213, 44,55,33,-24);
	allocfig(F_PIANTA+4,215, 78,53,28,-24);
	allocfig(F_ALBERO  ,151, 96,53,73,-32);
	allocfig(F_ALBERO+1,269, 34,46,72,-32);
	allocfig(F_BRAZIER  ,  1,124,37,46,-24);
	allocfig(F_BRAZIER+1,279,109,37,46,-24);
	allocfig(F_BRAZIER+2, 39,136,37,46,-24);
	allocfig(F_BRAZIER+3,262,  1,37,32,-24);
	for (i=0; i<15; i++)
		allocfig(F_BOMB+i,i*13,188,12,12,-4);

	KAOSMark[0] = mm_alloc(27*33);
	KAOSMark[1] = mm_alloc(24*37);
	KAOSMark[2] = mm_alloc(25*33);
	KAOSMark[3] = mm_alloc(22*33);
	volume_bar = mm_alloc(67*12);
	volume_sel = mm_alloc(7*10);
	toggle_on = mm_alloc(12*12);
	toggle_off = mm_alloc(12*12);

#ifndef __DATAFILE__
	loadLBM("lbm\\misc.lbm",NULL);
	getrfig(128,0,27,33,mm_recall(KAOSMark[0]));
	getrfig(161,0,24,37,mm_recall(KAOSMark[1]));
	getrfig(194,0,25,33,mm_recall(KAOSMark[2]));
	getrfig(227,0,22,33,mm_recall(KAOSMark[3]));
	getlfig( 0, 0,67,12,mm_recall(volume_bar));
	getlfig(67, 1, 7,10,mm_recall(volume_sel));
	getlfig(74, 0,12,12,mm_recall(toggle_off));
	getlfig(86, 0,12,12,mm_recall(toggle_on));
#endif
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

#ifndef __DATAFILE__
	loadLBM("lbm\\slimer.lbm",NULL);
#endif
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

	 FigList[F_SLIMER+10]=FigList[F_SLIMER];
	allocfig(F_SLIMER+11,128,143,51,34,-12);
	 FigList[F_SLIMER+12]=FigList[F_SLIMER];
	allocfig(F_SLIMER+13,180,140,48,36,-12);

	 FigList[F_SLIMER+14]=FigList[F_SLIMER];
	allocfig(F_SLIMER+15, 66,144,61,27,-12);

#ifndef __DATAFILE__
	loadLBM("lbm\\bub1.lbm",NULL);
#endif
	allocfig(F_BUB  ,  1, 69,70,60,-30);
	allocfig(F_BUB+1, 72, 69,66,60,-30);
	allocfig(F_BUB+2,139, 69,61,60,-30);
	allocfig(F_BUB+3,201, 69,58,60,-30);
	allocfig(F_BUB+4,  1,  9,70,59,-29);

	 FigList[F_BUB+10]=FigList[F_BUB];
	allocfig(F_BUB+11, 1,130,70,56,-32);

	 FigList[F_BUB+12]=FigList[F_BUB];
	allocfig(F_BUB+13,72,130,73,56,-32);
#ifndef __DATAFILE__
	loadLBM("lbm\\bub2.lbm",NULL);
#endif
	allocfig(F_BUB+5,  1, 64,70,62,-32);
	allocfig(F_BUB+6, 72, 64,66,62,-32);
	allocfig(F_BUB+7,139, 64,63,62,-32);
	allocfig(F_BUB+8,203, 64,58,62,-32);
	allocfig(F_BUB+9,  1,  1,70,62,-32);
#ifndef __DATAFILE__
	loadLBM("lbm\\face.lbm",NULL);
#endif
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

	 FigList[F_FACE+10]=FigList[F_FACE];
	allocfig(F_FACE+11,  1, 92,34,45,-24);

#ifndef __DATAFILE__
	loadLBM("lbm\\morti.lbm",NULL);
#endif
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
#ifndef __DATAFILE__
	loadLBM("lbm\\demo.lbm",NULL);
#endif
#ifndef __DATAFILE__
	loadLBM("lbm\\player1.lbm",NULL);
#endif
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
#ifndef __DATAFILE__
	loadLBM("lbm\\player2.lbm",NULL);
#endif
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

	 FigList[F_PLAYER+32]=FigList[F_PLAYER];
	allocfig(F_PLAYER+33,  1,114,35,53,-32);
	allocfig(F_PLAYER+34, 37,114,35,53,-32);
	allocfig(F_PLAYER+35, 73,114,51,45,-26);
	allocfig(F_PLAYER+36,125,132,50,27,-33);
	allocfig(F_PLAYER+37,176,132,50,27,-37);
	allocfig(F_PLAYER+38,227,139,50,20,-40);
#ifndef __DATAFILE__
	loadLBM("lbm\\ragno.lbm",NULL);
#endif
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

	 FigList[F_SPIDER+10]=FigList[F_SPIDER];
	allocfig(F_SPIDER+11,  1,122, 67, 29,-20);

	 FigList[F_SPIDER+12]=FigList[F_SPIDER];
	allocfig(F_SPIDER+13, 69,125, 68, 45,-20);

	allocfig(F_SPIDER+14,  1,152, 67, 45,-18);
	allocfig(F_SPIDER+15,138,125, 70, 40,-18);
	allocfig(F_SPIDER+16,209,125, 69, 33,-20);
	allocfig(F_SPIDER+17,138,166, 69, 30,-22);
	allocfig(F_SPIDER+18,208,166, 69, 26,-22);
#ifndef __DATAFILE__
	loadLBM("lbm\\other.lbm",NULL);
#endif
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

	minibox_I = mm_alloc(16*16);
	minibox_F = mm_alloc(16*16);
#ifndef __DATAFILE__
	getlfig(176,0,16,16,mm_recall(minibox_I));
	getlfig(176,16,16,16,mm_recall(minibox_F));
#endif
	for (i=0; i<11; i++) {
		Sqrs[i].dx = 32; Sqrs[i].dy = 24;
		Sqrs[i].memidx = mm_alloc(32*24);
#ifndef __DATAFILE__
		getrfig(192+((i & 3)<<5),(i>>2)*24,32,24,mm_recall(Sqrs[i].memidx));
#endif
	}

	LitBar.dx = 152; LitBar.dy = 20;
	LitBar.memidx = mm_alloc(152*20);
#ifndef __DATAFILE__
	getlfig(166,74,152,20,mm_recall(LitBar.memidx));
#endif

	for (i=0;i<10;i++) {
		litnumf[i].dx = 3; litnumf[i].dy = 5;
		litnumf[i].memidx = mm_alloc(15);
#ifndef __DATAFILE__
		getlfig(179+(i<<2),129,3,5,mm_recall(litnumf[i].memidx));
#endif
	}
	for (i=2;i<10;i++) {
		normnumf[i].dx = 7; normnumf[i].dy = 9;
		normnumf[i].memidx = mm_alloc(63);
#ifndef __DATAFILE__
		getlfig(172+(i<<3),137,7,9,mm_recall(normnumf[i].memidx));
#endif
	}
	normnumf[0].dx = 7; normnumf[0].dy = 9;
	normnumf[0].memidx = mm_alloc(63);
#ifndef __DATAFILE__
	getlfig(188+(8<<3),137,7,9,mm_recall(normnumf[0].memidx));
#endif
	normnumf[1].dx = 4; normnumf[1].dy = 9;
	normnumf[1].memidx = mm_alloc(36);
#ifndef __DATAFILE__
	getlfig(183,137,4,9,mm_recall(normnumf[1].memidx));
#endif
	for (i=0;i<10;i++) {
		stdnumf[i].dx = 7; stdnumf[i].dy = 7;
		stdnumf[i].memidx = mm_alloc(49);
#ifndef __DATAFILE__
		getlfig(176+(i<<3),149,7,7,mm_recall(stdnumf[i].memidx));
#endif
	}
	for (i=0;i<17;i++) {
		happyface[i].dx = 24; happyface[i].dy = 29;
		happyface[i].memidx = mm_alloc(24*29);
#ifndef __DATAFILE__
		getlfig(1+(i%10)*25,97+(i/10)*30,24,29,mm_recall(happyface[i].memidx));
#endif
	}
	woodblock = mm_alloc(219*20);
	levelblock = mm_alloc(64*16);
	matchblock = mm_alloc(64*16);
#ifndef __DATAFILE__
	getlfig(1,157,219,20,mm_recall(woodblock));
	getlfig(1,178,64,16,mm_recall(levelblock));
	getlfig(66,178,64,16,mm_recall(matchblock));
#endif

#ifndef __DATAFILE__
	loadLBM("lbm\\sold1.lbm",NULL);
#endif
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
#ifndef __DATAFILE__
	loadLBM("lbm\\sold2.lbm",NULL);
#endif
	allocfig(F_SOLDIER+16,155,1,33,66,-40);
	allocfig(F_SOLDIER+17,189,1,32,66,-40);
	allocfig(F_SOLDIER+18,222,1,57,65,-40);
	allocfig(F_SOLDIER+19,1,67,35,64,-40);
	allocfig(F_SOLDIER+20,1,1,32,65,-40);
	allocfig(F_SOLDIER+21,34,1,29,65,-40);
	allocfig(F_SOLDIER+22,64,1,53,65,-40);
	allocfig(F_SOLDIER+23,118,1,36,65,-40);

	 FigList[F_SOLDIER+24]=FigList[F_SOLDIER];
	allocfig(F_SOLDIER+25,37,67,34,66,-40);
	 FigList[F_SOLDIER+26]=FigList[F_SOLDIER];
	allocfig(F_SOLDIER+27,72,67,30,66,-40);

	 FigList[F_SOLDIER+28]=FigList[F_SOLDIER];
	allocfig(F_SOLDIER+29,103,69,55,68,-40);

	allocfig(F_SOLDIER+30,159,69,55,68,-36);
	allocfig(F_SOLDIER+31,215,69,68,56,-32);
	allocfig(F_SOLDIER+32,1,138,58,28,-36);
	allocfig(F_SOLDIER+33,60,142,58,24,-40);
	allocfig(F_SOLDIER+34,119,145,58,21,-42);
#ifndef __DATAFILE__
	loadLBM("lbm\\balls.lbm",NULL);
#endif
	 
	allocfig(F_BOOM   ,  0, 0,15,15,-7);
	 FigList[F_FIREBALL]=FigList[F_BOOM];
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
	 FigList[F_SLIMFIRE]=FigList[F_BOOM+6];
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

	 
	 

	for (i=0;i<4;i++) corner[i] = mm_alloc(4*4);
	for (i=4;i<8;i++) corner[i] = mm_alloc(24*4);
#ifndef __DATAFILE__
	getlfig(0,133,4,4,mm_recall(corner[0]));
	getlfig(5,133,4,4,mm_recall(corner[1]));
	getlfig(0,138,4,4,mm_recall(corner[2]));
	getlfig(5,138,4,4,mm_recall(corner[3]));
	getlfig(1,128,24,4,mm_recall(corner[4]));
	getlfig(31,123,4,24,mm_recall(corner[5]));
	getlfig(26,123,4,24,mm_recall(corner[6]));
	getlfig(1,123,24,4,mm_recall(corner[7]));
#endif

	ArmFig[0].dx = 28; ArmFig[0].dy = 11;
	ArmFig[1].dx = 20; ArmFig[1].dy = 12;
	ArmFig[2].dx = 33; ArmFig[2].dy = 9;
	ArmFig[3].dx = 29; ArmFig[3].dy = 12;
	ArmFig[4].dx = 30; ArmFig[4].dy = 8;
	ArmFig[5].dx = 9; ArmFig[5].dy = 12;
	ArmFig[0].memidx = mm_alloc(28*11);
	ArmFig[1].memidx = mm_alloc(20*12);
	ArmFig[2].memidx = mm_alloc(33*9);
	ArmFig[3].memidx = mm_alloc(29*12);
	ArmFig[4].memidx = mm_alloc(30*8);
	ArmFig[5].memidx = mm_alloc(9*12);
#ifndef __DATAFILE__
	getlfig(120,52,28,11,mm_recall(ArmFig[0].memidx));
	getlfig(120,64,20,12,mm_recall(ArmFig[1].memidx));
	getlfig(141,64,33, 9,mm_recall(ArmFig[2].memidx));
	getlfig(120,77,29,12,mm_recall(ArmFig[3].memidx));
	getlfig(150,77,30, 8,mm_recall(ArmFig[4].memidx));
	getlfig(149,51, 9,12,mm_recall(ArmFig[5].memidx));
#endif
	BigBar.dx = 320;
	BigBar.dy = 32;
	BigBar.memidx = mm_alloc(320*32);
#ifndef __DATAFILE__
	fcopyarea(168,0,32,mm_recall(BigBar.memidx));
#endif
	MNID_Nico = mm_alloc(68*32);
	MNID_Maxy = mm_alloc(76*32);
	MNID_Andy = mm_alloc(68*32);
#ifndef __DATAFILE__
	getlfig(250,54,68,32,mm_recall(MNID_Nico));
	getlfig(242,90,76,32,mm_recall(MNID_Maxy));
	getlfig(250,126,68,32,mm_recall(MNID_Andy));
#endif

	for (i=0;i<3;i++) {
		WorkID[i] = mm_alloc(16*16);
#ifndef __DATAFILE__
		getlfig(196+i*17,56,16,16,mm_recall(WorkID[i]));
#endif
	}
	KeyLeft = mm_alloc(14*14);
	KeyRight = mm_alloc(14*14);
#ifndef __DATAFILE__
	getlfig(196,73,14,14,mm_recall(KeyLeft));
	getlfig(211,73,14,14,mm_recall(KeyRight));
#endif
	kidwarn = mm_alloc(32*28);
#ifndef __DATAFILE__
	getlfig(205,91,32,28,mm_recall(kidwarn));
#endif

	cbacki[0][0] = mm_alloc(32000);
	cbacki[0][1] = mm_alloc(32000);
	PPal apppal = (PPal)mm_recall(origpal = mm_alloc(768));
#ifndef __DATAFILE__
	loadLBM("lbm\\bck_c0.lbm",apppal);
	fcopyarea(0,0,100,mm_recall(cbacki[0][1]));
	fcopyarea(100,0,100,mm_recall(cbacki[0][0]));
	fwfill(apppal,0,3);
#endif
	fwmove(startpal,apppal,768);
	fbacki[0][0] = mm_alloc(32000);
	fbacki[0][1] = mm_alloc(32000);
#ifndef __DATAFILE__
	loadLBM("lbm\\bck_f0.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(fbacki[0][0]));
	fcopyarea(100,0,100,mm_recall(fbacki[0][1]));
#endif
	cbacki[1][0] = mm_alloc(32000);
	cbacki[1][1] = mm_alloc(32000);
#ifndef __DATAFILE__
	loadLBM("lbm\\bck_c1.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(cbacki[1][1]));
	fcopyarea(100,0,100,mm_recall(cbacki[1][0]));
#endif
	fbacki[1][0] = mm_alloc(32000);
	fbacki[1][1] = mm_alloc(32000);
#ifndef __DATAFILE__
	loadLBM("lbm\\bck_f1.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(fbacki[1][0]));
	fcopyarea(100,0,100,mm_recall(fbacki[1][1]));
#endif
	cbacki[2][0] = mm_alloc(32000);
	cbacki[2][1] = mm_alloc(32000);
#ifndef __DATAFILE__
	loadLBM("lbm\\bck_c2.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(cbacki[2][1]));
	fcopyarea(100,0,100,mm_recall(cbacki[2][0]));
#endif
	fbacki[2][0] = mm_alloc(32000);
	fbacki[2][1] = mm_alloc(32000);
#ifndef __DATAFILE__
	loadLBM("lbm\\bck_f2.lbm",NULL);
	fcopyarea(0,0,100,mm_recall(fbacki[2][0]));
	fcopyarea(100,0,100,mm_recall(fbacki[2][1]));
#endif


	PlayerAscia[0] = mm_alloc(50*115);
	PlayerAscia[1] = mm_alloc(85*150);
	PlayerAscia[2] = mm_alloc(106*123);
#ifndef __DATAFILE__
	loadLBM("lbm\\Ascia.lbm",NULL);
	getrfig(0,0,50,115,mm_recall(PlayerAscia[0]));
	getrfig(51,0,85,150,mm_recall(PlayerAscia[1]));
	getrfig(137,0,106,123,mm_recall(PlayerAscia[2]));
#endif
	PlayerPist[0] = mm_alloc(55*61);
	PlayerPist[1] = mm_alloc(55*67);
	PlayerPist[2] = mm_alloc(42*39);
	thunder = mm_alloc(80*128);
#ifndef __DATAFILE__
	loadLBM("lbm\\Mano.lbm",NULL);
	getrfig(0,0,55,61,mm_recall(PlayerPist[0]));
	getrfig(0,62,55,67,mm_recall(PlayerPist[1]));
	getrfig(56,0,42,39,mm_recall(PlayerPist[2]));
	getrfig(124,40,80,128,mm_recall(thunder));
#endif
	PlayerFuc[0] = mm_alloc(65*53);
	PlayerFuc[1] = mm_alloc(99*108);
	PlayerFuc[2] = mm_alloc(40*31);
#ifndef __DATAFILE__
	loadLBM("lbm\\Fucile.lbm",NULL);
	getrfig(0,0,65,53,mm_recall(PlayerFuc[0]));
	getrfig(0,54,99,108,mm_recall(PlayerFuc[1]));
	getrfig(66,0,40,31,mm_recall(PlayerFuc[2]));
#endif
	PlayerMitr[0] = mm_alloc(53*77);
	PlayerMitr[1] = mm_alloc(53*31);
	PlayerMitr[2] = mm_alloc(53*31);
	PlayerMitr[3] = mm_alloc(53*31);
	PlayerMitr[4] = mm_alloc(42*41);
	PlayerBaz = mm_alloc(83*68);
#ifndef __DATAFILE__
	loadLBM("lbm\\Mitra.lbm",NULL);
	getrfig(0,0,53,77,mm_recall(PlayerMitr[0]));
	getrfig(169,0,53,31,mm_recall(PlayerMitr[1]));
	getrfig(54,0,53,31,mm_recall(PlayerMitr[2]));
	getrfig(112,0,53,31,mm_recall(PlayerMitr[3]));
	getrfig(223,0,42,41,mm_recall(PlayerMitr[4]));
	getrfig(0,88,83,68,mm_recall(PlayerBaz));
#endif
#ifndef __DATAFILE__
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
#else
	loadsound(SND_THUNDER1,	NULL);
	loadsound(SND_THUNDER2,	NULL);
	loadsound(SND_GOCCIA,	NULL);
	loadsound(SND_BOOM,		NULL);
	loadsound(SND_BUB,		NULL);
	loadsound(SND_TOPO,		NULL);
	loadsound(SND_KEYS,		NULL);
	loadsound(SND_PISTOLA,	NULL);
	loadsound(SND_FUCILE,	NULL);
	loadsound(SND_MITRA,	NULL);
	loadsound(SND_MISSILE,	NULL);
	loadsound(SND_SLIME1,	NULL);
	loadsound(SND_SLIME2,	NULL);
	loadsound(SND_PASSO,	NULL);
	loadsound(SND_PORTA1,	NULL);
	loadsound(SND_PORTA2,	NULL);
	loadsound(SND_SCODE,	NULL);
	loadsound(SND_PROIETTILE,NULL);
	loadsound(SND_VASO1,	NULL);
	loadsound(SND_VASO2,	NULL);
	loadsound(SND_COLPO1,	NULL);
	loadsound(SND_COLPO2,	NULL);
	loadsound(SND_TOGGLE,	NULL);
	loadsound(SND_WIND,		NULL);
	loadsound(SND_FIRE,		NULL);
	loadsound(SND_MISSILGO,	NULL);
	loadsound(SND_UNLOCK,	NULL);
	loadsound(SND_CLOSE,	NULL);
	loadsound(SND_GLASS,	NULL);
	loadsound(SND_PASSO2,	NULL);
	loadsound(SND_SOLDDEAD,	NULL);
	loadsound(SND_SLIMDEAD,	NULL);
	loadsound(SND_SPIDDEAD,	NULL);
	loadsound(SND_SOLDATT,	NULL);
	loadsound(SND_SLIMATT,	NULL);
	loadsound(SND_SPIDATT,	NULL);
	loadsound(SND_CLOCK,	NULL);
	loadsound(SND_CLOCK2,	NULL);
	loadsound(SND_SPIDNEAR,	NULL);
	loadsound(SND_CHARGE,	NULL);
	loadsound(SND_CHARGE2,	NULL);
	loadsound(SND_SOLDDOWN,	NULL);
	loadsound(SND_SLIMDOWN,	NULL);
	loadsound(SND_SPIDDOWN,	NULL);
	loadsound(SND_OSSA,		NULL);
	loadsound(SND_VENOM,	NULL);
	loadsound(SND_RISATA,	NULL);
	loadsound(SND_BONUS,	NULL);
	loadsound(SND_ASCIAHIT,	NULL);
	loadsound(SND_SOLDHIT,	NULL);
	loadsound(SND_SLIMHIT,	NULL);
	loadsound(SND_SPIDHIT,	NULL);
	loadsound(SND_BBOUNCE,	NULL);
#endif
}


extern std::atomic<byte> Sync;
extern char slowmode;
static std::atomic<bool> g_sync_running{false};
static std::atomic<bool> g_sync_enabled{false};
static std::thread g_sync_thread;

namespace {

using steady_clock = std::chrono::steady_clock;

 


constexpr std::uint64_t PIT_CLOCK_HZ = 1193182ull;
constexpr std::uint64_t PIT_DIVISOR = 0x8000ull;
constexpr std::uint64_t BIOS_DIVISOR = 0x10000ull;

struct ExactPeriod {
    std::uint64_t whole_ns;
    std::uint64_t rem;
    std::uint64_t den;
};

constexpr ExactPeriod make_pit_period()
{
    const std::uint64_t n = PIT_DIVISOR * 1000000000ull;
    return { n / PIT_CLOCK_HZ, n % PIT_CLOCK_HZ, PIT_CLOCK_HZ };
}

constexpr ExactPeriod make_bios_period()
{
    const std::uint64_t n = BIOS_DIVISOR * 1000000000ull;
    return { n / PIT_CLOCK_HZ, n % PIT_CLOCK_HZ, PIT_CLOCK_HZ };
}

constexpr ExactPeriod PIT_PERIOD = make_pit_period();
constexpr ExactPeriod BIOS_PERIOD = make_bios_period();

void advance_period(steady_clock::time_point &deadline,
                    std::uint64_t &remainder,
                    const ExactPeriod &period)
{
    deadline += std::chrono::nanoseconds(period.whole_ns);
    remainder += period.rem;
    if (remainder >= period.den) {
        deadline += std::chrono::nanoseconds(1);
        remainder -= period.den;
    }
}

void add_sync_ticks(std::uint64_t ticks)
{
    if (!ticks)
        return;

    byte cur = Sync.load(std::memory_order_relaxed);
    while (ticks && cur < 8) {
        const byte room = static_cast<byte>(8 - cur);
        const byte add = static_cast<byte>(ticks < room ? ticks : room);
        if (Sync.compare_exchange_weak(cur,
                                       static_cast<byte>(cur + add),
                                       std::memory_order_relaxed,
                                       std::memory_order_relaxed))
            return;
    }
}

}  

void kaos_sdl_start_sync()
{
    if (g_sync_running.exchange(true))
        return;

    g_sync_thread = std::thread([] {
        const ExactPeriod &period = slowmode ? BIOS_PERIOD : PIT_PERIOD;
        using clock = steady_clock;

        clock::time_point next = clock::now();
        std::uint64_t remainder = 0;

         

        advance_period(next, remainder, period);

        while (g_sync_running.load(std::memory_order_relaxed)) {
            const auto now = clock::now();
            if (now < next) {
                const auto remaining = next - now;
                if (remaining > std::chrono::milliseconds(2))
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                else
                    std::this_thread::yield();
                continue;
            }

             




            std::uint64_t elapsed = 0;
            const auto sample_now = clock::now();
            do {
                ++elapsed;
                advance_period(next, remainder, period);
            } while (next <= sample_now && elapsed < 256);

             

            if (g_sync_enabled.load(std::memory_order_relaxed))
                add_sync_ticks(elapsed);
        }
    });
}

void kaos_sdl_enable_sync()
{
    g_sync_enabled.store(true, std::memory_order_release);
}

void kaos_sdl_disable_sync()
{
    g_sync_enabled.store(false, std::memory_order_release);
}

void kaos_sdl_stop_sync()
{
    g_sync_enabled.store(false, std::memory_order_release);
    if (!g_sync_running.exchange(false))
        return;

    if (g_sync_thread.joinable())
        g_sync_thread.join();
}

 
void instSync(void) {
    if (!SyncOk) {
        SyncOk = 1;
        kaos_sdl_enable_sync();
        kaos_sdl_start_sync();
    }
}

void uninstSync(void) {
    if (SyncOk) {
        SyncOk = 0;
        kaos_sdl_disable_sync();
        kaos_sdl_stop_sync();
    }
}


 







const char *CodeMNNico = "MOPLGT",
	 *CodeMNMaxy = "MOOD\\^",
	 *CodeMNAndy = "MOCQH^",
	 *CodeMNLev  = "MONHZ",
	 *CodeMNStrg = "MOUWVL",
	 *CodeMNTMap = "MOVPEU",
	 *CodeMNBest = "MODHWY",
	 *CodeMNDark = "MOFDVP",
	 *CodeMNBoom = "MODRSR",
	 *CodeMNBGod = "MODJSI",
	 *CodeMNHard = "MOJDVI",
	 *CodeMNCapt = "MOEDTY", 	 
	 *CodeMNInvi = "MOKQZN";

int Sqr_count = 0, Sqr_focus = 0, Sqr_ratio = 0;
void FocusSqr(int type, int time) {
	if (Sqr_count>=18) return;
	Sqr_focus = type;
	Sqr_count = time;
	Sqr_ratio = 1;
	 
	 
}

void checkword() {
	int codenum;
	 
	if (saveplayback && playbackwait) return;

	if (fwcomp(CodeAr,CodeMNNico,6)) codenum=SC_MNNico; else
	if (fwcomp(CodeAr,CodeMNMaxy,6)) codenum=SC_MNMaxy; else
	if (fwcomp(CodeAr,CodeMNAndy,6)) codenum=SC_MNAndy; else
	if (fwcomp(CodeAr,CodeMNLev ,5)) codenum=SC_MNLev ; else
	if (fwcomp(CodeAr,CodeMNStrg,6)) codenum=SC_MNStrg; else
	if (fwcomp(CodeAr,CodeMNTMap,5)) codenum=SC_MNTMap; else
	if (fwcomp(CodeAr,CodeMNBest,6)) codenum=SC_MNBest; else
	if (fwcomp(CodeAr,CodeMNDark,6)) codenum=SC_MNDark; else
	if (fwcomp(CodeAr,CodeMNBoom,6)) codenum=SC_MNBoom; else
	if (fwcomp(CodeAr,CodeMNBGod,6) && players<2) codenum=SC_MNBGod; else
	if (fwcomp(CodeAr,CodeMNHard,6) && players<2) codenum=SC_MNHard; else
	if (fwcomp(CodeAr,CodeMNCapt,6)) codenum=0xff; else  
	if (fwcomp(CodeAr,CodeMNInvi,6)) codenum=SC_MNInvi; else
	return;
	CodeAr[0] = 0;	 
	eventmanager.sendcom(EV_SCODE,codenum);
	switch(codenum) {
		case 0xff: return;
		case SC_MNBGod: codenum=SC_MNLev; break;
		case SC_MNHard:	codenum=SC_MNMaxy; break;
		case SC_MNInvi:	codenum=SC_MNLev; break;
	}
	FocusSqr(codenum,72);
	playsound(SND_SCODE,0,0,0,SYSOWN,SFL_FIXED);
	playsound(SND_RISATA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
}

void animpal(byte animnum,int rep) {
	if (players>1)
		if (animnum!=PAA_LIGHT && animnum!=PAA_FADEIN &&
			animnum!=PAA_FADEOUT) return;
	if (!palanimstate) fwmove(endpal,startpal,768);
	palanimstate = 1;
	do
		switch (animnum) {
			case PAA_RED: palfilter(endpal,48,0,0); break;
			case PAA_GREEN: palfilter(endpal,0,48,0); break;
			case PAA_GOLD: palfilter(endpal,40,40,0); break;
			case PAA_ARM: palfilter(endpal,48,0,48); break;
			case PAA_YKEY: palfilter(endpal,32,63,0); break;
			case PAA_RKEY: palfilter(endpal,63,32,0); break;
            case PAA_LIGHT: {
                for (int i = 0; i < 256; ++i) {
                    byte *p = reinterpret_cast<byte *>(endpal) + i * 3;
                    const int intensity = p[0] + p[1] + p[2];
                    if (intensity > 40) {
                        p[0] = static_cast<byte>(p[0] + 63);
                        p[1] = static_cast<byte>(p[1] + 58);
                        p[2] = static_cast<byte>(p[2] + 57);
                    }
                    p[0] >>= 1; p[1] >>= 1; p[2] >>= 1;
                }
                palanimcount = 16;
                break;
            }
			case PAA_GOD:  


						   
						  *(startpal)=58;
						  *(startpal+1)=50;
						  palanimstate=2;
						  break;
			case PAA_UNGOD:  
							 
							fwmove(startpal,gammapal,2);
							palanimstate=2;
							break;
			case PAA_HARD:	*(startpal)=58;
							 
							palanimstate=2;
							break;
			case PAA_UNHARD:fwmove(startpal,gammapal,2);
							palanimstate=2;
							break;
			case PAA_FADEIN: fwfill(endpal,0,768);
							 fwmove(startpal,gammapal,768);
							 palanimstate=2;
							 break;
			case PAA_FADEOUT: fwfill(startpal,0,768);
							  fwmove(endpal,gammapal,768);
							  palanimstate=2;
							  break;
		}
	while (rep--);
}

void pal_animate(char change) {
	switch (palanimstate) {
		case 0: return;
		case 1:
			if ((palanimcount+=4) > 16) {
				palanimcount=0;
				palanimstate=2;
				setpal(endpal,0,256);	 
			} else if (change)
						palmorphpart(startpal,endpal,0,256,palanimcount,16);
			break;
		case 2:
			if (++palanimcount > 16) {
				palanimcount=0;
				palanimstate=0;
				setpal(startpal,0,256);
			} else if (change)
						palmorphpart(endpal,startpal,0,256,palanimcount,16);
			break;
	}
}

void intro() {
	char *p[4];
	int i,j;
	 





	int x[4],y[4],
		snd[4] = {-1,-1,-1,-1},
		sd[4] = {-3,-1,1,3},
		dx[4] = {0,0,0,0},
		dy[4] = {0,0,0,0},
		ex[4] = {115-2,140-2,163-2,185-2};
	int rdx[4] = {27,24,25,22},
		rdy[4] = {33,37,33,33};
	int kk;
	for (i=0;i<4;i++)
		dragon2[i] = mm_alloc(16000);
#ifndef __DATAFILE__
	loadLBM("lbm\\dragon2.lbm",NULL);
	for (i=0;i<4;i++)
		fcopyarea(i*50,0,50,mm_recall(dragon2[i]));
#endif
	for (i=0;i<4;i++) {
		dragon1[i] = mm_alloc(16000);
		p[i]=(char *)mm_reserve(rdx[i]*rdy[i]);
	}
#ifndef __DATAFILE__
	loadLBM("lbm\\dragon1.lbm",NULL);
	for (i=0;i<4;i++) fcopyarea(i*50,0,50,mm_recall(dragon1[i]));
#endif
	drawpict(dragon1,vpage);
	fadeout((PPal)mm_recall(origpal),0,256,1);
	ftransfscreen(VMEMPTR);
	while(kbhit()) getch();
		ftransfblock(64,24,196,28,VMEMPTR);
	Big->colorwrite(50,24,32,"The Millennium Group");
	ftransfblock(50,24,240,28,VMEMPTR);
	fadein((PPal)mm_recall(origpal),0,256,80);
	 
	 
	for(i=20;i-- && !kbhit();){waitvsync(); }
	if (!kbhit()) animpal(PAA_LIGHT,2);
	Big->colorwrite(50,24,50,"The Millennium Group");
	ftransfblock(50,24,240,28,VMEMPTR);
	 
	play3Dsound(48l<<FIXSHIFT,0l,SND_THUNDER1,0,SYSOWN,SFL_BASIC);
	setvbuff(VMEMPTR);
	putlfig(4,170,32,28,mm_recall(kidwarn));
	setvbuff(vpage);
	int ang = ANG360-ANG45;
	if (palanimstate)
	for(i=100;i--;){
		if (!palanimstate) waitvsync();
					  else pal_animate(1);
		soundmanager(0,0,ang=angleadd(ang,ANGLVL));
	}
	Big->colorwrite(116,50,96,"presents");
	ftransfblock(108,50,128,28,VMEMPTR);
	 
	for(i=100;i-- && !kbhit();) {
		waitvsync();
		soundmanager(0,0,ang=angleadd(ang,ANGLVL));
	}
	 
	setvrect(0,0,320,200);
	for(i=0;i<=64+16*3;i++) {
		waitvsync();
		soundmanager(0,0,ang=angleadd(ang,ANGLVL));
		for(j=0;j<4;j++) {
			if (dx[j]>0 && dy[j]>0)
				storelfig(x[j],y[j],dx[j],dy[j],p[j]);
			kk=i-(j<<4);
			if (!kk) {
				snd[j] = playsound(SND_MISSILGO,SM_NORMVOL-(j<<1),SM_NORMVOL-6+(j<<1),
								sd[j],SYSOWN,SFL_FIXED|SFL_CONTINUE);
				if (j==3) ftransfblock(4,170,32,28,VMEMPTR);  
			} else
			if (kk==58) {
				if (snd[j]>=0) {
					stopsound(snd[j]);
					snd[j]=-1;
				}
				playsound(SND_BOOM,SM_NORMVOL-(j<<1),SM_NORMVOL-6+(j<<1),sd[j],SYSOWN,SFL_FIXED);
			}
			if (kk<=0) {
				 
				continue;
			} else if (kk>64) kk=64;
			x[j] = 160+(int)(((std::int32_t)ex[j]-160)*kk>>6);
			y[j] = 10+(int)((86l-10l)*kk>>6);
			dx[j] = rdx[j]*kk>>6;
			dy[j] = rdy[j]*kk>>6;

			if (dx[j]>0 && dy[j]>0)
				getlfig(x[j],y[j],dx[j],dy[j],p[j]);

		}
		for (j=4;j--;)
		  if (dx[j]>0 && dy[j]>0)
			scalerscfig(x[j],y[j],rdx[j],rdy[j],dx[j],dy[j],0,mm_recall(KAOSMark[j]));
		Big->colorwrite(50,24,50,"The Millennium Group");
		for (j=4;j--;)
		  if (dx[j]>0 && dy[j]>0)
			ftransfblock(x[j]-8,y[j]-8,dx[j]+16,dy[j]+16,VMEMPTR);
		 
	}
	 
	for (i=0;i<4;i++) {
		delete p[i];
		if (snd[i]>=0) stopsound(snd[i]);
	}
	setvbuff(VMEMPTR);
	Std->write(74,124,229,233,KaosFirm);
	Std->write(80,140,128,132,NicMark);
	char *s = (char *)mm_reserve(80);
	sprintf(s,"Graphic Art and Sounds by %s",MaxFirm);
	 
	 
	Std->write(80,148,128,132,s);
	sprintf(s,"Manual Design by %s",AndFirm);
	 
	 
	Std->write(80,156,128,132,s);
	 





	if (sb_type>=SBT_SBPRO)
		Std->write(32,180,3,1,"Enhanced 3D Sound System");
	else
		Std->write(32,180,116,114,"Enhanced 3D Sound System");
	Std->write(32,190,3,1,"Light & shade managing");
	Std->write(160,180,3,1,"Several enemies with own AI");
#ifndef __SHAREWARE__
	Std->write(160,190,3,1,"Multiplayer game capabilities");
	sprintf(s,"Serial #%08lX",static_cast<unsigned long>(SerialNo));
	Std->write(120,2,230,234,s);
#else
	Std->write(160,190,116,114,"Multiplayer game capabilities");
#endif
	delete s;
	setvbuff(vpage);
	while(kbhit()) getch();
	while (sm_soundnum) {waitvsync();soundmanager(0,0,0);}
	 












	for(i=800;i-- && !kbhit();) waitvsync();
	 
	fadeout((PPal)mm_recall(origpal),0,256,20);
	castlepal = mm_alloc(768);
	for (i=0;i<4;i++) castle[i] = mm_alloc(16000);
#ifndef __DATAFILE__
	loadLBM("lbm\\pres.lbm",mm_recall(castlepal));
	for (i=0;i<4;i++) fcopyarea(i*50,0,50,mm_recall(castle[i]));
#endif
	drawpict(castle,vpage);
	ftransfscreen(VMEMPTR);
	fadein((PPal)mm_recall(castlepal),0,256,40);
	if (kbhit()) getch();
	for (i=500;i-- && !kbhit();) waitvsync();
	while(kbhit()) getch();
	fadeout((PPal)mm_recall(castlepal),0,256,20);
	mm_unload(castlepal);
	for(i=0;i<4;i++) mm_unload(castle[i]);
}

const char *(menutext[15]) = {
	"New game","Load game","Save game","Options","Info!","Demo","Quit",
	"Single Player","Two Players","Three Players",
	"Ordering!","Help!","Millennium Group","Greetings","Quotes"
};

 
void darkscreen(char shade) {
    if (!vpage) return;
    for (int i = 0; i < 320 * 200; ++i) {
        byte value = vpage[i];
        byte shaded = static_cast<byte>(value + static_cast<byte>(shade));
        byte hi = static_cast<byte>((value ^ shaded) & 0xf0);
        if (hi) shaded = static_cast<byte>((shaded ^ hi) | 0x0f);
        vpage[i] = shaded;
    }
}

char *menufnm[6];

char input(int x, int y, char *str) {
	char app[34];
	char nome[33] = "";
	byte c, len;
	int anim = 20;

	kb_enablestd();
	len = strlen(strcpy(nome,str));
	while (1) {
		if (++anim>40) anim=0;
		storelfig(x,y,219,20,mm_recall(woodblock));
		strcpy(app,nome);
		if (anim>20) strcat(app,"_");
		Std->write(x+8,y+8,112,116,app);
		waitvsync();
		ftransfblock(x,y,219,20,VMEMPTR);
		if (kbhit())
		switch (c = getch()) {
			case 27:
				kb_disablestd();
				return 0;
			case 13:
				kb_disablestd();
				fwmove(str,nome,len+1);
				return 1;
			case 8: if (len) nome[--len]=0; break;
			default:
				if (c>=' ' && c<=(byte)'') {
					if (len<32) {
						nome[len++]=c;
						nome[len]=0;
					}
				}
				break;
		}
	}
}

void handlemenu() {
  int i, j, col;
  char *s;
  switch (menustate) {
	case 0:	 
	  if (kb_onepressed(KB_ESC) && (!ValidGame || !playbackwait || !saveplayback)) {
		menustate=1;
		menuitem=0;
		 
		viewstate = playbackwait ? 1 : 0;
		kb_onepressed(KB_UP);
		kb_onepressed(KB_DOWN);
		kb_onepressed(KB_LEFT);
		kb_onepressed(KB_RIGHT);
		kb_onepressed(KB_ENTER);
		 





		stopallsounds();
		windsound=-1;
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
	  }
	  break;
	case 1:  
	  kb_onepressed(KB_LEFT);
	  kb_onepressed(KB_RIGHT);
	  Big->colorwrite(99,3,48,"Main Menu");
	  Big->colorwrite(100,4,50,"Main Menu");
	  for (i=0;i<7;i++) {
		if (i==2)
			if (ValidGame && players && playbackwait)
				if (i==menuitem) col=50; else col=2;  
			else
				if (i==menuitem) col=54; else col=6;  
		else
		if (i==5)
			if (ValidGame && playbackwait)
				if (i==menuitem) col=50; else col=2;
			else
				if (i==menuitem) col=54; else col=6;
		else
			if (i==menuitem) col=50; else col=2;  
		Big->colorwrite(100,32+i*22,col,menutext[i]);
	  }
	  if (kb_onepressed(KB_UP)) {
		if (menuitem) menuitem--; else menuitem=6;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_DOWN)) {
		if (menuitem<6) menuitem++; else menuitem=0;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,+127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_ENTER)) {
		if (menuitem==2 &&
			(!ValidGame || !players || !playbackwait)) {
			playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
		} else
		if (menuitem==5) {
			if (ValidGame && playbackwait) {
				action=199;	 
				menustate=100;
				 
			}
		} else
		if (menuitem==6) action=200;	 
		else {
			playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menustate=menuitem+2;
			menuitem=0;
		}
	  } else
	  if (kb_onepressed(KB_ESC)  ) {
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
		menustate=100;
	  }
	  break;
	case 2:	 
	  Big->colorwrite(59,3,48,"Start a new game");
	  Big->colorwrite(60,4,50,"Start a new game");
	  for (i=0;i<3;i++) {
#ifdef __SHAREWARE__
			if (i>0) if (i==menuitem) col=54; else col=6;  
				else if (i==menuitem) col=50; else col=2;  
#else
			if (i==menuitem) col=50; else col=2;  
#endif
		Big->colorwrite(80,32+i*22,col,menutext[i+7]);
	  }
#ifndef __SHAREWARE__
	  if (menuitem==3) col=50; else col=2;
#else
	  if (menuitem==3) col=54; else col=6;
#endif
	  Big->colorwrite(80,32+74,col,"From:");
	  if (menuitem>0) {
			if (initlev<0) {
				storelfig(150,32+78,64,16,mm_recall(matchblock));
				col = -initlev;
			} else {
				storelfig(150,32+78,64,16,mm_recall(levelblock));
				col = initlev+1;
			  }
			drawnum(150+60,32+78+4,0,0,stdnumf,col);
	  } else {
			storelfig(150,32+78,64,16,mm_recall(levelblock));
			if (initlev>=0) col = initlev+1;
					   else col = -initlev;
			drawnum(150+60,32+78+4,0,0,stdnumf,col);
		}
	  if (kb_onepressed(KB_UP)) {
		if (menuitem) menuitem--; else menuitem=3;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_DOWN)) {
		if (menuitem<3) menuitem++; else menuitem=0;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
	  } else
#ifndef __SHAREWARE__
	  if (kb_onepressed(KB_LEFT) && (menuitem==3)) {
		if (--initlev<-16) initlev=12;
		playsound(SND_ASCIAHIT,SM_NORMVOL,SM_LOWVOL,0,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_RIGHT) && (menuitem==3)) {
		if (++initlev>12) initlev=-16;
		playsound(SND_ASCIAHIT,SM_LOWVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
	  } else
#endif
	  if (kb_onepressed(KB_ENTER)) {
		 
		switch(menuitem) {
			case 0:	 
				menustate=100;
				end_playback();
				delgame();
				if (initlev >= 0) levnum = initlev;
							 else levnum = -initlev-1;
				CLRFLAG(gameflags,GFL_COMPETITIVE);
				initgame(1);
				if (saveplayback)
					if (!record_playback(saveplayback-1)) {
						menustate=2;
						break;
					}
				playbackwait=MAXPLAYBACKWAIT;
				viewstate=2;
				action=101;
				break;
#ifndef __SHAREWARE__
			case 1:	 
			case 2:
				menustate=100;
				end_playback();
				delgame();
				if (initlev >= 0) {
					levnum = initlev;
					CLRFLAG(gameflags,GFL_COMPETITIVE);
				} else {
					levnum = -initlev-1;
					SETFLAG(gameflags,GFL_COMPETITIVE);
				  }
				initgame(menuitem+1);
				if (saveplayback)
					if (!record_playback(saveplayback-1)) {
						menustate=2;
						break;
					}
				playbackwait=MAXPLAYBACKWAIT;
				viewstate=2;
				action=101;
				break;
			case 3:
				if (++initlev>12) initlev=-16;
				playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
				break;
#endif
		}
		if (action==101) {
			CLRFLAG(gameflags,GFL_TOTALMAP);
			MAKEFLAG(gameflags,GFL_AUTOCENTER,s_autocenter);
			MAKEFLAG(gameflags,GFL_KAOSENABLE,s_Kaos_enable);
		}
	  } else
	  if (kb_onepressed(KB_ESC)) {
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
		menustate=1;
		menuitem=0;
	  }
	  break;
	case 3:  
	case 4:  
	   
	  int handle;
	  char name[16];
	  for (i=0;i<6;i++) {
		sprintf(name,"KAOS%1d.GAM",i);
		if ((handle = CRC_openread_user(name))>=0) {
			 



			menufnm[i] = (char *)mm_reserve(33);
			 
			 
			CRC_lseek(handle,8,SEEK_CUR);
			CRC_read(handle,menufnm[i],32); *(menufnm[i]+32)=0;
			CRC_close(handle);
		} else menufnm[i]=NULL;
	  }
	  if (menustate==3) Big->colorwrite(110,4,54,menutext[1] );
				   else Big->colorwrite(110,4,54,menutext[2] );
	   
	  menustate+=7;  
	  menuitem=0;
	  break;
	case 5:  
	  Big->colorwrite(109,3,48,menutext[3] );
	  Big->colorwrite(110,4,50,menutext[3] );
	  if (sm_soundok)
		if (menuitem==0) col=50; else col=2;
	  else
		if (menuitem==0) col=54; else col=6;
	  Big->colorwrite(40,32,col,"FX Volume");
	  putlfig(190,32+6,67,12,mm_recall(volume_bar));
	  putlfig(190+((sm_soundvol<<6)/SM_MAXVOL),32+7,
				7,10,mm_recall(volume_sel));
	   



	  if (sm_soundok)
		if (menuitem==1) col=50; else col=2;
	  else
		if (menuitem==1) col=54; else col=6;
	  Big->colorwrite(40,32+22,col,"Music Volume");
	  putlfig(190,32+22+6,67,12,mm_recall(volume_bar));
	  putlfig(190+((SBdefault[MIXER_MIDI] & 0xf0)>>2),32+22+7,
				7,10,mm_recall(volume_sel));
	  if (menuitem==2) col=50; else col=2;
	  Big->colorwrite(40,32+44,col,"Screen Size");
	  putlfig(190,32+44+6,67,12,mm_recall(volume_bar));
	  putlfig(190+(60*(viewdim[players-1]-96)/224),32+44+7,
				7,10,mm_recall(volume_sel));
	  if (menuitem==3) col=50; else col=2;
	  Big->colorwrite(40,32+66,col,"Brightness");
	  putlfig(190,32+66+6,67,12,mm_recall(volume_bar));
	  putlfig(190+60*(gamma-MINGAMMA)/(MAXGAMMA-MINGAMMA),32+66+7,
				7,10,mm_recall(volume_sel));
	  if (sm_soundok)
		if (menuitem==4) col=130; else col=82;
	  else
		if (menuitem==4) col=134; else col=86;
	  Std->write(56,32+90,col,col+4,"Reverse L/R");
	  if (sm_lrinverted) i=toggle_on; else i=toggle_off;
	  storelfig(40,32+88,12,12,mm_recall(i));
	   





	  if (menuitem==5) col=130; else col=82;
	  Std->write(56,32+104,col,col+4,"Video sync.");
	  if (GETFLAG(gameflags,GFL_USESYNC)) i=toggle_on; else i=toggle_off;
	  storelfig(40,32+102,12,12,mm_recall(i));
	  if (menuitem==6) col=130; else col=82;
	  Std->write(56,32+118,col,col+4,"Blood");
	  if (GETFLAG(gameflags,GFL_SHOWBLOOD)) i=toggle_on; else i=toggle_off;
	  storelfig(40,32+116,12,12,mm_recall(i));
	  if (menuitem==7) col=130; else col=82;
	  Std->write(96+56,32+90,col,col+4,"Messages");
	  if (GETFLAG(gameflags,GFL_MESSAGES)) i=toggle_on; else i=toggle_off;
	  storelfig(96+40,32+88,12,12,mm_recall(i));
	  if (menuitem==8) col=130; else col=82;
	  Std->write(96+56,32+104,col,col+4,"Kaos-mode");
	  if (s_Kaos_enable) i=toggle_on; else i=toggle_off;
	  storelfig(96+40,32+102,12,12,mm_recall(i));
	  if (menuitem==9) col=130; else col=82;
	  Std->write(96+56,32+118,col,col+4,"Auto-center view");
	  if (s_autocenter) i=toggle_on; else i=toggle_off;
	  storelfig(96+40,32+116,12,12,mm_recall(i));

	  if (kb_onepressed(KB_UP)) {
		if (menuitem) menuitem--; else menuitem=9;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_DOWN)) {
		if (menuitem<9) menuitem++; else menuitem=0;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_ESC)) {
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
		menustate=1;
		menuitem=3;
	  } else {
		  if (kb_onepressed(KB_ENTER) || kb_onepressed(KB_SPACE)) {i=1;col=0;} else
		   if (kb_onepressed(KB_LEFT)) {i=2;col=-5;} else
			if (kb_onepressed(KB_RIGHT)) {i=3;col=5;} else i=0;

		  if (i) {
			oldmenustate=-1;
			int k = 0;
			if (i>1 && menuitem>=4 && menuitem<=9) {
				if (i==2 && menuitem>=7) menuitem-=3; else
				  if (i==3 && menuitem<=6) menuitem+=3; else
					k = -1;
			} else
			switch (menuitem) {
				case 0:
					if (sm_soundok) {
						k = sm_soundvol;
					 
						if (i==3 && k<SM_MAXVOL) k++; else
						if (i==2 && k>0) k--; else k=-1;
						if (k>=0) {
						 
						 
							sm_soundvol = k;
						} else break;
					}
					k=0;
					break;
				case 1:
					if (!sm_soundok) { k=0; break;	}
					k = (SBdefault[MIXER_MIDI] & 0xf0)>>4;
					if (i==3 && k<15) k++; else
					 if (i==2 && k>0) k--; else k=-1;
					if (k>=0) {
						k = (k<<4)|(k<<12);
						sbmix_setvol(MIXER_MIDI,k);
					}
					k=0;
					break;
				case 2:
					if (i==3) k = changeviewsize(1)-1; else
					 if (i==2) k = changeviewsize(-1)-1;
					break;
				case 3:
					if (i==3) k = changegamma(gamma+1)-1; else
					 if (i==2) k = changegamma(gamma-1)-1;
					break;
				case 4: if (sm_soundok) {k=1;sm_lrinverted=!sm_lrinverted;} break;
				case 5: k=1; CHGFLAG(gameflags,GFL_USESYNC); break;
				case 6: k=1; CHGFLAG(gameflags,GFL_SHOWBLOOD); break;
				case 7: k=1; CHGFLAG(gameflags,GFL_MESSAGES); break;
				case 8: k=1; s_Kaos_enable=!s_Kaos_enable;
						 
						break;
				case 9: k=1; s_autocenter=!s_autocenter;
						if (playbackwait)
							MAKEFLAG(gameflags,GFL_AUTOCENTER,s_autocenter);
						break;
			}
			if (k>=0)
			   
			  if (!k)
				playsound(SND_UNLOCK,0,0,col,SYSOWN,SFL_FIXED);
			  else
				playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,col,SYSOWN,SFL_FIXED);
		  }
		}
	  break;
	case 6:  
	  kb_onepressed(KB_LEFT);
	  kb_onepressed(KB_RIGHT);
	  Big->colorwrite(119,3,48,"Info!");
	  Big->colorwrite(120,4,50,"Info!");
	  for (i=0;i<5;i++) {
		if (i==menuitem) col=50; else col=2;  
		Big->colorwrite(100,32+i*22,col,menutext[i+10]);
	  }
	  if (kb_onepressed(KB_UP)) {
		if (menuitem) menuitem--; else menuitem=4;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_DOWN)) {
		if (menuitem<4) menuitem++; else menuitem=0;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_ENTER)) {
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
		menustate=50+menuitem;
		menuitem=0;
	  } else
	  if (kb_onepressed(KB_ESC)) {
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
		menustate=1;
		menuitem=4;
	  }
	  break;
	case 10:	 
	case 11:     
	  kb_onepressed(KB_LEFT);
	  kb_onepressed(KB_RIGHT);
	  if (menustate==10) Big->colorwrite(110,4,50,menutext[1] );
					else Big->colorwrite(110,4,50,menutext[2] );
	  for (i=0;i<6;i++) {
		storelfig(50,32+(i*20),219,20,mm_recall(woodblock));
		if (menufnm[i]==NULL)
			if (menuitem==i)
				Std->write(58,40+(i*20),131,135,"- unused -");
			else
				Std->write(58,40+(i*20),4,7,"- unused -");
		else {
			 
			if (menuitem==i)
				Std->write(58,40+(i*20),128,132,menufnm[i]);
			else
				Std->write(58,40+(i*20),112,117,menufnm[i]);
		}
	  }
	  if (kb_onepressed(KB_UP)) {
		if (menuitem) menuitem--; else menuitem=5;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_DOWN)) {
		if (menuitem<5) menuitem++; else menuitem=0;
		playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
	  } else
	  if (kb_onepressed(KB_ENTER)) {
		if (menustate==11 || menufnm[menuitem]!=NULL) {
			if (menustate==10) {
				action=102+menuitem;
				playsound(SND_KEYS,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
				menustate=100;
			} else {
				if (menufnm[menuitem]==NULL) {
					menufnm[menuitem]=(char *)mm_reserve(33);
					*menufnm[menuitem]=0;
				}
				if (input(50,32+(menuitem*20),menufnm[menuitem]))
					if (savegame(menuitem,menufnm[menuitem])) {
						action=110;
						playsound(SND_KEYS,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
						menustate=100;
					}
			  }
		}
	  } else
	  if (kb_onepressed(KB_ESC)) {
		playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
		menuitem=menustate-9;
		menustate=1;
	  }
	  if (menustate==100 || menustate==1)
		for (i=0;i<6;i++) if (menufnm[i]!=NULL) delete menufnm[i];
	  break;
	case 50:  
		darkscreen(5);	 
		oldmenustate=-1;
		Big->colorwrite(60,14,148,"Ordering");
		putrfig(160   ,4,27,33,mm_recall(KAOSMark[0]));
		putrfig(160+25,4,24,37,mm_recall(KAOSMark[1]));
		putrfig(160+71,4,22,33,mm_recall(KAOSMark[3]));
		putrfig(160+48,4,25,33,mm_recall(KAOSMark[2]));
		for (i=0; i<10; i++) {
			if ((s=(char *)sl_find(mm_recall(sl_ordering),menuitem+i)) != NULL)
				Std->write(60,50+i*10,112,116,s);
		}
		j=0;
		if (menuitem>0) {
			putlfig(132,170,14,14,mm_recall(KeyLeft));
			j |= 1;
		}
		if (sl_find(mm_recall(sl_ordering),menuitem+i) != NULL) {
			putlfig(148,170,14,14,mm_recall(KeyRight));
			j |= 2;
		}
		switch(menuitem) {
		  case 10:
			putrfig(80,100,FigList[F_BUB+4].dx,FigList[F_BUB+4].dy,
					mm_recall(FigList[F_BUB+4].memidx));
			putrfig(170,110,FigList[F_FACE+3].dx,FigList[F_FACE+3].dy,
					mm_recall(FigList[F_FACE+3].memidx));
			break;
		   




		}
		if ((kb_onepressed(KB_LEFT) || kb_onepressed(KB_UP)) && (j & 1)) {
			playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menuitem -= 10;
		} else
		if ((kb_onepressed(KB_ENTER) || kb_onepressed(KB_RIGHT) ||
			 kb_onepressed(KB_DOWN)) && (j & 2)) {
			playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menuitem += 10;
		} else
		if (kb_onepressed(KB_ESC)) {
			playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menustate = 6;
			menuitem = 0;
		}
		break;
	case 51:
		darkscreen(5);	 
		oldmenustate=-1;
		Big->colorwrite(129,13,48,"Help!");
		Big->colorwrite(130,14,50,"Help!");

		for (i=0; i<11; i++) {
			if ((s=(char *)sl_find(mm_recall(sl_help),menuitem+i)) != NULL)
				Std->write(40,50+i*10,112,117,s);
		}
		j=0;
		if (menuitem>0) {
			putlfig(132,170,14,14,mm_recall(KeyLeft));
			j |= 1;
		}
		if (sl_find(mm_recall(sl_ordering),menuitem+i) != NULL) {
			putlfig(148,170,14,14,mm_recall(KeyRight));
			j |= 2;
		}
		if ((kb_onepressed(KB_LEFT) || kb_onepressed(KB_UP)) && (j & 1)) {
			playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menuitem -= 11;
		} else
		if ((kb_onepressed(KB_ENTER) || kb_onepressed(KB_RIGHT) ||
			 kb_onepressed(KB_DOWN)) && (j & 2)) {
			playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menuitem += 11;
		} else
		if (kb_onepressed(KB_ESC)) {
			playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menustate = 6;
			menuitem = 1;
		}
		break;
	case 52:
		darkscreen(5);	 
		oldmenustate=-1;
		Big->colorwrite(49,23,48,"The Millennium group");
		Big->colorwrite(50,24,50,"The Millennium group");
		switch (menuitem) {
			case 0:
				Std->write(74,80,112,115,NicFirm);
				 Std->write(174,80,226,230,"(Master of the C++)");
				putlfig(48,76,16,16,mm_recall(WorkID[0]));
				Std->write(74,100,112,115,MaxFirm);
				 Std->write(174,100,226,230,"(Master of the Pixel)");
				putlfig(48,96,16,16,mm_recall(WorkID[1]));
				Std->write(74,120,112,115,AndFirm);
				 Std->write(174,120,226,230,"(Master of the Pencil)");
				putlfig(48,116,16,16,mm_recall(WorkID[2]));
				 
				break;
			case 1:
				Std->write(60,60,130,134,"The Analyst and Programmer");
				Std->write(80,80,128,132,NicFirm);
				putlfig(180,67,68,32,mm_recall(MNID_Nico));
				 





				break;
			case 2:
				Std->write(60,60,130,134,"The Artist and Sound Creator");
				Std->write(80,80,128,132,MaxFirm);
				putlfig(180,67,76,32,mm_recall(MNID_Maxy));
				 





				break;
			case 3:
				Std->write(60,60,130,134,"The Manual Designer");
				Std->write(80,80,128,132,AndFirm);
				putlfig(180,67,68,32,mm_recall(MNID_Andy));
				 





				break;
		}
		if (menuitem>0) putlfig(132,142,14,14,mm_recall(KeyLeft));
		if (menuitem<3) putlfig(148,142,14,14,mm_recall(KeyRight));
		if (menuitem>=1 && menuitem<=3) {
			for (i=0;i<4;i++)
				if ((s=(char *)sl_find(mm_recall(sl_info),(menuitem-1)*4+i)) != NULL) {
					if (i<3) col=82; else col=113;
					Std->write(60,100+i*10,col,col+4,s);
				}
		}
		if ((kb_onepressed(KB_LEFT) || kb_onepressed(KB_UP)) && menuitem>0) {
			playsound(SND_ASCIAHIT,SM_NORMVOL,SM_NORMVOL-2,0,SYSOWN,SFL_FIXED);
			menuitem--;
		} else
		if ((kb_onepressed(KB_ENTER) || kb_onepressed(KB_RIGHT) ||
			 kb_onepressed(KB_DOWN)) && menuitem<3) {
			playsound(SND_ASCIAHIT,SM_NORMVOL-2,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menuitem++;
		} else
		if (kb_onepressed(KB_ESC)) {
			playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
			menustate=6;
			menuitem=2;
		}
		break;
	 












	case 53:
		kb_onepressed(KB_LEFT); kb_onepressed(KB_RIGHT);
		kb_onepressed(KB_UP); kb_onepressed(KB_DOWN);
		darkscreen(3);	 
		oldmenustate=-1;
		setvrect(0,50,320,130);
		Big->colorwrite( 99,13,49,"Greetings to");
		Big->colorwrite(100,14,50,"Greetings to");
		if ((s=(char *)sl_find(mm_recall(sl_greetings),menuitem/10 )) != NULL) {
		  for (i=0;i<14;i++) {
			if (s) {
			  switch(s[0]) {
				case '<': col=128; s++; break;
				case '>': col=226; s++; break;
				default: col=112;
			  }
			  j = (320-Std->writelen(s))>>1;
			  Std->writeclip(j,50+i*10-(menuitem%10 ),col,col+4,s);
			  s = (char *)sl_find(s,1);
			} else menuitem = 0;
		  }
		  menuitem++;
		}
		if (kb_onepressed(KB_ENTER) || kb_onepressed(KB_ESC)) {
			playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menustate=6;
			menuitem=3;
			 
			 
		}
		break;
	case 54:
		if (viewstate!=2) {
			viewstate=2;
			stopallsounds();
			windsound=-1;
		}
		kb_onepressed(KB_LEFT); kb_onepressed(KB_RIGHT);
		kb_onepressed(KB_UP); kb_onepressed(KB_DOWN);
		darkscreen(5);	 
		oldmenustate=-1;
		setvrect(0,50,320,130);
		Big->colorwrite(109,13,48,"Quotes");
		Big->colorwrite(110,14,50,"Quotes");
		if ((s=(char *)sl_find(mm_recall(sl_quotes),menuitem/20)) != NULL) {
		  for (i=0;i<14;i++ ) {
			if (s) {
			  if (s[0]!=' ') col=128; else col=112;
			  Std->writeclip(40,50+i*10-(menuitem%20>>1),col,col+4,s);
			  s = (char *)sl_find(s,1);
			} else menuitem = 0;
		  }
		  menuitem++;
		}
		if (kb_onepressed(KB_ENTER) || kb_onepressed(KB_ESC)) {
			playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			menustate=6;
			menuitem=4;
			viewstate = playbackwait ? 1 : 0;
			 
		}
		break;
	case 100:
	case 101:
	   






	  if (menustate==101) menustate=0;
					 else menustate++;
	  Sync=0;
	  viewstate=0;
	  break;
  }
}

void showlevelstats() {
	int i, app;
	std::int32_t fires, hits, blood, tecno;
	int kills;

	stopallsounds();
	windsound=-1;
	sursound=-1;
	fadeout(startpal,0,256,48);
	drawpict(dragon1,vpage);
	char s[80];
	sprintf(s,"Level %2d: %s",levnum+1,levname);
	Big->colorwrite(16,8,0,s);
	 
	ftransfscreen(VMEMPTR);
	fadein((PPal)mm_recall(origpal),0,256,32);
	((Player *)PlayAr[0])->getstats(fires,hits,blood,tecno,kills);
	Big->colorwrite(32,32,2 ,"Mira:");
	 
	if (fires>0) app=(int)(hits*100/fires);
			else app=0;
	sprintf(s,"%3d",app);
	Big->colorwrite(240,32,52,s);
	Big->colorwrite(280,32,52,"%");
	 
	playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
	ftransfscreen(VMEMPTR);
	for (i=10;i--;) {waitvsync();soundmanager(0,0,0);}
	Big->colorwrite(32,54,2 ,"Violenza:");
	 
	if (hits>0) app=(int)(blood*100/hits);
		   else app=0;
	sprintf(s,"%3d",app);
	Big->colorwrite(240,54,52,s);
	Big->colorwrite(280,54,52,"%");
	 
	playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
	ftransfscreen(VMEMPTR);
	for (i=10;i--;) {waitvsync();soundmanager(0,0,0);}
	Big->colorwrite(32,76,2 ,"Vittime:");
	 
	if (numenemies>0) app=(int)(kills*100/numenemies);
				 else app=0;
	sprintf(s,"%3d",app);
	Big->colorwrite(240,76,52,s);
	Big->colorwrite(280,76,52,"%");
	 
	playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
	ftransfscreen(VMEMPTR);
	for (i=10;i--;) {waitvsync();soundmanager(0,0,0);}
	Big->colorwrite(32,98,2 ,"Tecno hits:");
	 
	if (hits>0) app=(int)(tecno*100/hits);
		   else app=0;
	sprintf(s,"%3d",app);
	Big->colorwrite(240,98,52,s);
	Big->colorwrite(280,98,52,"%");
	 
	playsound(SND_MITRA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
	ftransfscreen(VMEMPTR);
	for (i=10;i--;) {waitvsync();soundmanager(0,0,0);}
	kb_onepressed(KB_ESC); kb_onepressed(KB_ENTER);
	do {
		soundmanager(0,0,0);
	} while (!kb_onepressed(KB_ESC) && !kb_onepressed(KB_ENTER));
}

void handle_comline(int argc, char **argv) {
	int i, j;
	char *s, *app;
	char num[11];
	for (i=1; i<argc; i++) {
		 
		s = argv[i];
		for (char *u=s; *u; ++u) *u = static_cast<char>(toupper(static_cast<unsigned char>(*u)));
		if (*s == '/' || *s == '-') s++;
		if (*s == 'F') {
			levfile = s+1;
			continue;
		}
		 
		for (j=0,app=s; j<10 && *app; app++)
			if (*app>='0' && *app<='9') {
				num[j++] = *app;
				*app=0;
			} else
			if (*app<'A' || *app>'Z') *app=0;
		num[j]=0;
		j = atoi(num);
		if (!strcmp(s,"MKRS")) {
			printf("\n%s\n%s",KaosFirm,NicMark);
			printf("\nGraphic Art & Sounds by %s",MaxFirm);
			printf("\nManual Design by %s\n",AndFirm);
			exit(0);
		} else
		if (!strcmp(s,"PAUSE")) init_pause=1; else
#ifndef __SHAREWARE__
		if (!strcmp(s,"SLOW")) slowmode=1; else
		if (!strcmp(s,"USESLIM")) useslim=1; else
		if (fwcomp(s,"SAVEDEM",7)) {
			saveplayback=1;
			if (j>=0 && j<MAXPLAYBACKS)
				saveplayback += j;
		} else
#endif
		if (!strcmp(s,"H") || !strcmp(s,"?") || !strcmp(s,"HELP")) {
		   printf("\n %s - help screen\xa",KaosFirm);
			   puts("  /PAUSE      - initial pause for checking memory");
#ifndef __SHAREWARE__
			   puts("  /SLOW       - slow motion mode");
			   puts("  /USESLIM	   - uses Slimers as 2nd & 3rd player");
			   puts("  /SAVEDEM<n> - saves KAOSn.DEM playback file (<n>=0,1,2)");
#endif
			   puts("  /F<name>    - uses <name>.DLF levels file");
			   puts("  /H          - this help screen.");
			exit(0);
		}
	}
}

int clockstate = -1,
	clockcount = 0x0000;
void clockcheck() {
    if (clockcount--) return;

    const std::time_t now = std::time(nullptr);
    const std::tm *tm = std::localtime(&now);
    if (!tm) return;

    switch (clockstate) {
    case 0:
        clockcount = 35;
        if (tm->tm_min % 15) --clockstate;
        break;

    case -1:
        if (tm->tm_min % 15) {
            clockcount = 35;
            break;
        }

        clockstate = (tm->tm_hour << 8) + (tm->tm_min / 15);
        if (tm->tm_hour > 12) clockstate -= 0xc00;
        else if (clockstate < 0x100) clockstate += 0xc00;
        [[fallthrough]];

    default: {
        clockcount = 91;
        const int vol = (windsound == -1) ? SM_LOWVOL : SM_NORMVOL;

        if (clockstate > 0xff) {
            playsound(SND_CLOCK, vol, vol, 0, SYSOWN, SFL_FIXED);
            clockstate -= 0x100;
        } else if (clockstate) {
            playsound(SND_CLOCK2, vol, vol, 0, SYSOWN, SFL_FIXED);
            --clockstate;
        }
        break;
    }
    }
}

void show_message(int n) {
	char str[80];
	char *app;
	void far *strlist = mm_recall(sl_messages);
	int i,j;

	if (sl_find(strlist,n)) {
		fwfill(str,0,80);
		 
		for (i=0;i<40;i++)
		 for (j=n;j>=0;j--)
		  if ((app = (char *)sl_find(strlist,j))[i] != ' ') {
			if (i) app += i-1;
			strcat(str,app);
			if (j==n) i=40;
			break;
		  }
		if (!message_num) message_count = MESSAGE_TIME;
		 
		if ((i = message_num) == 3) {
			strcpy(message[0],message[1]);
			strcpy(message[1],message[2]);
			i--;
		} else message_num++;
		strcpy(message[i],str);
	} else error("show_message","invalid number");
}

int kaos_main(int argc, char **argv) {
	int i, j;
	TEvent evnt;
	char Kaos_tic = 36;

	maxdim = 320;
	handle_comline(argc, argv);

	mm_init();

 







	init3DEngine();
	 




	init();

#ifndef __DATAFILE__
	Std = new Font("std"); Std->setdispl(-1,0);
	Big = new Font("big"); Big->setdispl(-1,0);
	Std->packdata();
	Big->packdata();
#else
	Std = new Font(); Std->setdispl(-1,0);
	Big = new Font(); Big->setdispl(-1,0);
	Std->unpackdata();
	Big->unpackdata();
#endif

#ifndef __DATAFILE__
	sl_greetings = sl_convert("GREETING.SLF");
	sl_quotes = sl_convert("QUOTES.SLF");
	sl_ordering = sl_convert("ORDERING.SLF");
	sl_help = sl_convert("HELP.SLF");
	sl_info = sl_convert("INFO.SLF");
	sl_messages = sl_convert("MESSAGES.SLF");
	(void)sl_convert("Keyboard.slf");	 
#else
	sl_greetings = mm_alloc(0);	 
	sl_quotes = mm_alloc(0);
	sl_ordering = mm_alloc(0);
	sl_help = mm_alloc(0);
	sl_info = mm_alloc(0);
	sl_messages = mm_alloc(0);
	 



	(void)mm_alloc(0);	 
#endif
	loadconfig();
	initgame(1);
	initL();
	kaos_sdl_set_fullscreen(s_fullscreen != 0);
	intro();
	kb_initkey();
#ifndef __DATAFILE__
#ifdef __MAKEDATA__
	mm_resetall();
	mm_makedatafile();
#endif
#endif
	mm_resetall();
	int resetcnt = 0;
	instSync();

	 
	 




	menustate=1;
	menuitem=0;
	viewstate=1;
	animpal(PAA_FADEIN,0);
	playbackwait=72;	 

  do {
  if (ValidGame) {
	Kaos_count = 60;	 
	if (!menustate || menustate>=100) animpal(PAA_FADEIN,0);
	action=1;actioncount=54;  
	Sync=0;
	kaos_sim_tic = 0;
	viewstate = 0;	 
	repaint = 1;	 
	quake = 0;
	oldmenustate = -1;
	key_pause = 0;
	message_num=0;
	message_count=0;
	do {
		++kaos_sim_tic;
		clockcheck();
		if (ShutDown) action = 200;
		if (viewstate<2) {
			actorslist.animate();
			if (!viewstate) doormanager.animate();
		}

		if (players<=1) {
			Mover &first = *(PlayAr[0]->mover);
			soundmanager(first.x,first.y,first.angle);
		} else soundmanager(-1l,0,0);

#ifdef KAOS_DEMO_TRACE
		if (players > 0 && PlayAr[0] && PlayAr[0]->mover) {
			Mover &tm = *(PlayAr[0]->mover);
			fprintf(stderr,
				"KAOS_BASE tic=%llu sync=%u rng=%u player=%d x=%08X y=%08X ang=%d flags=%04X\n",
				(unsigned long long)kaos_sim_tic,
				(unsigned)Sync.load(), kaos_random_seed(), tm.id,
				(unsigned)(std::uint32_t)tm.x,
				(unsigned)(std::uint32_t)tm.y,
				tm.angle, (unsigned)tm.flags);
		}
#endif

		 
		if (Sqr_count) Sqr_count--;
		++resetcnt;
		 


		if (!Sync) {
			if (resetcnt >= MAXRESETCNT) {  
				mm_reset();
				resetcnt = 0;
				if (viewstate<1 && sursound==-1)
				switch (random(10)) {	 
					case 2:
						if (action) break;
						if (windsound>=0) i=SM_NORMVOL; else i=SM_LOWVOL;
						if (random(2))
							sursound=playsound(SND_THUNDER1,i-random(4),i-random(4),127-random(254),SYSOWN,SFL_FIXED);
						else {
							if (i==SM_NORMVOL) {
								animpal(PAA_LIGHT,2+random(2));
								action=2;actioncount=16+40;  
								thunderdraw=thunder;
								thunderang=angleadd(anglesub(PlayAr[0]->mover->angle,ANG60),random(ANG60<<1));
								thunderz=halfheight+random(halfheight);
							} else
								sursound=playsound(SND_THUNDER2,SM_LOWVOL-random(4)-2,SM_LOWVOL-random(4)-2,127-random(254),SYSOWN,SFL_FIXED);
						}
						break;
					case 3:
						if (windsound<0)  
							sursound=playsound(SND_GOCCIA,SM_LOWVOL-random(4),SM_LOWVOL-random(4),127-random(254),SYSOWN,SFL_FIXED);
						break;
				}
			}
			if (repaint || menustate) {
				if (repaint || menustate!=oldmenustate) {
					drawpict(dragon2,vpage);
					oldmenustate = menustate;
				}
				 
				if (viewstate<2 && viewwidth!=320)
				for (i=0;i<players;i++)
				  drawcorners(PlayVid[i].vidx-halfwidth,PlayVid[i].vidy-halfheight);
				if (viewstate<2)
				  if (menustate) {
					for (i=0;i<3;i++)
						if (InfoBarAr[i]) InfoBarAr[i]->repaint(IBR_REPNOTRANSF);
				  } else {
					for (i=0;i<3;i++)
						if (InfoBarAr[i]) InfoBarAr[i]->repaint(IBR_REPAINT);
					}
			} else
			if (viewstate<2 && ((Sqr_count && Sqr_count<=10) || message_num)) {
				setvbuff(mm_recall(dragon2[0]));
				if (message_num) ftransfblock(0,0,200,30,vpage);
							else ftransfblock(8,8,32,24,vpage);
				setvbuff(vpage);
				if (viewwidth!=320)
				for (i=0;i<players;i++)
				  drawcorners(PlayVid[i].vidx-halfwidth,PlayVid[i].vidy-halfheight);
				 
			}
			if (viewstate<2) {
				 
				for (i=0;i<players;i++)
					PlayAr[i]->draw(PlayVid[i].vidx,PlayVid[i].vidy,
									PlayVid[i].cd,PlayVid[i].fd);
				if (action==1) {
					i = (320-Big->writelen(levname)) >> 1;
					Big->colorwrite(i-1,49,48,levname);
					Big->colorwrite(i,50,50,levname);
				}

				if (message_num) {
					if (message_count<10) j=10-message_count; else j=0;
					setvrect(0,0,200,30);
					for (i=0;i<message_num;i++)
						Std->writeclip(0,(i*10)-j,112,117,message[i]);
					ftransfblock(0,0,200,message_num*10,VMEMPTR);
				}

			  if (Sqr_count) {
				TFig &Sqrf = Sqrs[Sqr_focus];
				if (Sqr_count>1) {
				  if (Sqr_focus == SqrLumaca) {
					storerfig(8,8,32,24,mm_recall(Sqrf.memidx));
				  } else {
					if (Sqr_count<=10) Sqr_ratio--;
					 else if (Sqr_ratio<10) Sqr_ratio++;
					int dx = (int)Sqrf.dx*Sqr_ratio/10,
						dy = (int)Sqrf.dy*Sqr_ratio/10;
					setvrect(0,0,320,200);
					scalerscfig(24-(dx>>1),20-(dy>>1),
								Sqrf.dx,Sqrf.dy,dx,dy,10-Sqr_ratio,
								mm_recall(Sqrf.memidx));
					}
				} else Sqr_count=0;
				ftransfblock(8,8,32,24,VMEMPTR);
			  }
			}

			handlemenu();

			if (viewstate<1)
			  if (GETFLAG(gameflags,GFL_USESYNC) && !palanimstate)
				waitvsync();
			  else pal_animate(1);
			if (viewstate<2) {
				for (i=0;i<players;i++)
					PlayAr[i]->transf(PlayVid[i].vidx,PlayVid[i].vidy);
				if (action==1)
					ftransfblock((320-(i = Big->writelen(levname)))>>1,50,
								 i,10,VMEMPTR);
			}
			if (repaint || menustate) {
				if (menustate || players>1) ftransfscreen(VMEMPTR);
									   else ftransfarea(0,168,VMEMPTR);
				repaint = 0;
			}

			 


			kaos_sdl_present(kaos_screen_buffer());

			while (!Sync);  
			 






			Sync--;
		} else {
			 
			for (i=0;i<players;i++)
				PlayAr[i]->virtualdraw();
			pal_animate(0);
			if (Sync>=8) {
				FocusSqr(SqrLumaca,54);
				Sync=0;
			} else Sync--;
		  }

			 







		if (raindraw) anim_rain();	 

		if (key_pause) {
			setvbuff(VMEMPTR);
			Big->colorwrite(110,80,0,"Paused");
			setvbuff(vpage);
			key_pause = 0;	 
			while(!key_pause);
			key_pause = 0;
			kb_reset();
			Sync=0;
			repaint=1;
		}

		if (message_count) {
			if (!(--message_count)) {
				for (i=0;i<message_num-1;i++)
					strcpy(message[i],message[i+1]);
			  if (message_num>0) {
				message_num--;
				message_count = MESSAGE_TIME;
			  }
			}
		}

		 
	if (viewstate<1) {
		if (quake) quake--;
		 
		if (actioncount--) {
			switch (action) {
				case 1:  
					if (!actioncount) repaint=1;
					break;
				case 2:	 
					if (actioncount==40) {
					  thunderdraw=-1;
					  if ((i = (PlayAr[0]->mover->angle-thunderang)) > ANG180)
							i = ANG180-i;
					  i/=(ANGLVL<<1);
					  j = SM_NORMVOL-random(4);
					  sursound=playsound(SND_THUNDER2,j,j,
								i>=0 ? -127+i : 127-i,SYSOWN,SFL_FIXED);
					} else
					if (!actioncount) {  
						playsound(SND_THUNDER1,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
					}
					break;
				case 3:	 
					if (actioncount==1) {
						if (playbackwait && !saveplayback)
							showlevelstats();
						levnum++;
						action=101;
					}
					break;
			}
			if (!actioncount) action=0;
		}

		for (i=0;i<3;i++)
			if (InfoBarAr[i]) InfoBarAr[i]->animate();

		if (GETFLAG(gameflags,GFL_KAOSENABLE)) {
		if (!(--Kaos_tic)) {	 
			if (Kaos_count>=0) {
				Kaos_count--;
				if (Kaos_count>=0 && Kaos_count <= 15)
					playsound(SND_MITRA,SM_LOWVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
			}
			Kaos_tic = 36;
		}
		if (Kaos_count>=0 && Kaos_count <= 15) {
			ftransfblock(292,4,24,16,VMEMPTR);
			if (Kaos_count || Kaos_tic>1) {
				setvbuff(VMEMPTR);
				drawnum(316,4,1,2,bignumf,Kaos_count);
				setvbuff(vpage);
			}
		}
		}

#ifdef __MAINCHECK__
		ftransfblock(128,240,80,8,VMEMPTR);
		setvbuff(VMEMPTR);
		char s[20];
		sprintf(s,"MAIN: %7ld",mm_mainmem);
		Std->write(128,240,130,134,s);
		setvbuff(vpage);
#endif

#ifdef __VISUALIZE__
		mm_visualize();

	 void far *p;
	 char far *vp = (char _seg *)0xa000 + (char near *)0;

	 for (i=0;i<MAXOBJECTS;i++)
	 if ((p = objectslist.get(i)) != NULL) {
		asm {
			MOV AX, WORD PTR p
			MOV BX, WORD PTR p[2]
			SHR AX, 5
			SHR BX, 1
			ADD AX, BX
			MOV WORD PTR vp, AX   
		}
		*(vp++) = 224;
		 
	 }
#endif

		 







		 








		if (kb_onepressed(KB_F10)) {
			if (kb_pressed(KB_F9)) {
				changegamma(STDGAMMA);
				kb_onepressed(KB_F9);
			} else changegamma(gamma+1);
		} else
		if (kb_onepressed(KB_F9)) {
			changegamma(gamma-1);
		} else
		if (kb_onepressed(KB_F11)) {
			if (changeviewsize(-1))
				playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,-127,SYSOWN,SFL_FIXED);
		} else
		if (kb_onepressed(KB_F12)) {
			if (changeviewsize(1))
				playsound(SND_UNLOCK,SM_NORMVOL,SM_NORMVOL,127,SYSOWN,SFL_FIXED);
		} else
		if (kb_onepressed(KB_F4)) checkword();
	}
		while (eventmanager.get(evnt)) {
			for (i=0;i<3;i++)
				if (InfoBarAr[i]) InfoBarAr[i]->handle_event(evnt);
			if (evnt.what & 0x80) {
			  if (evnt.what & 0x40)	actorslist.handle_event(evnt);
							   else objectslist.handle_event(evnt);
			  doormanager.handle_event(evnt);
			}

			switch (evnt.what) {
				case EV_ENDSOUND:
					if (evnt.source==sursound) sursound=-1;
					break;
				case EV_PLACTIVATE:
					i = evnt.data.msg.x;
					if (Map[i].data == T_SWITCHU &&
						Map[i].type != MTP_DOOR1 &&
						Map[i].type != MTP_DOOR2) {
							Map[i].data = T_SWITCHD;
							playsound(SND_TOGGLE,0,0,0,SYSOWN,SFL_FIXED);
							if (!GETFLAG(gameflags,GFL_COMPETITIVE)) {
								action=3;
								actioncount=36;  
							}
					}
					break;
				case EV_BOOM:
					play3Dsound(evnt.data.pos.x,evnt.data.pos.y,SND_BOOM,
								SM_MAXVOL+SM_LOWVOL,SYSOWN,SFL_NORMAL);	 
					break;
				case EV_KILL:
				case EV_DEAD:
					ownerdead(evnt.source);
					 

















					if (evnt.what==EV_KILL) objectslist.erase(evnt.source);
					break;
				case EV_ERASEACT : actorslist.erase(evnt.source); break;
				case EV_RESURRECT:
					animpal(PAA_LIGHT,4);
					playsound(SND_THUNDER1,48,48,127,SYSOWN,SFL_FIXED);
					playsound(SND_THUNDER1,48,48,-127,SYSOWN,SFL_FIXED);
					Sqr_focus=4;  
					break;
				case EV_ENDPLAYBACK:
					eventmanager.flush();
					action=199;	 
					break;
				case EV_CHGTEXTURE: {
					byte *chptr;
					switch(evnt.source) {
					case 0: {  
						chptr = (byte *)mm_recall(changewtext);
						TMapInfo &mapinf = Map[evnt.data.msg.x];
						if (mapinf.type==MTP_DOOR1 ||
							mapinf.type==MTP_DOOR2) {
							if (chptr) {
								doormanager.doortype[mapinf.data] =
									chptr[doormanager.doortype[mapinf.data]];
							}
						} else
							if (chptr) {
								mapinf.data = chptr[mapinf.data];
							}
						break;
					}
					 





					case 2:  
						if ((chptr = (byte *)mm_recall(changeftext)) != NULL) {
							floormap[evnt.data.msg.x] =
								chptr[floormap[evnt.data.msg.x]];
						}
						break;
					}
					break;
				}
				case EV_MESSAGE:
					if (GETFLAG(gameflags,GFL_MESSAGES))
						show_message(evnt.source);
					break;
				case EV_ANTIKAOS:
					if (Kaos_count>=0) {
						Kaos_count = -2;
						ftransfblock(292,4,24,16,VMEMPTR);	 
						playsound(SND_RISATA,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_FIXED);
					}  
					break;
				case EV_SCODE:	 
					if (evnt.source==SC_MNTMap)
						CHGFLAG(gameflags,GFL_TOTALMAP);
#ifdef __CANCAPT__
					else
					if (evnt.source==0xff) {
						kb_enablestd();
						getch();
						ftransfarea(168,32,VMEMPTR);
						repaint=1;
						Sync=0;
						kb_disablestd();
					}
#endif
					break;
			}
		}
	} while (action<100);
  } else {  
		clockcheck();
		if (ShutDown) action = 200;
		while(!Sync); Sync=0;
		if (++resetcnt>MAXRESETCNT) {
			mm_reset();
			resetcnt=0;
		}
		pal_animate(1);
		soundmanager(0,0,0);
		drawpict(dragon2,vpage);
		handlemenu();
		ftransfscreen(VMEMPTR);
		 

		kaos_sdl_present(kaos_screen_buffer());
		if (viewstate<2)
		if (!playbackwait) {
			if (saveplayback) i=saveplayback-1;
						 else {
							i=random(MAXPLAYBACKS);
						 }
			if (play_playback(i)) {
				if (viewstate==1) viewstate=0;
				action=101;
			} else playbackwait = 72;	 
		} else playbackwait--;
	}

	switch (action) {
		 
		case 199:  
			end_playback();
			playbackwait=MAXPLAYBACKWAIT;
			delgame();
			 






			action=0;
			break;
		case 101:	 
			kb_reset();
			action=0;
			 
			 
			MAKEFLAG(gameflags,GFL_NORANDOM,!playbackwait || saveplayback);
			loadmap(levnum);
			if (!GETFLAG(gameflags,GFL_NORANDOM|GFL_COMPETITIVE) && !ValidGame && levnum>0)
				congratulations();
			break;
		case 102:  
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
		case 108:
		case 109:
			if ((ValidGame = loadgame(action-102)) != 0) {
				fadeout(startpal,0,256,20);
			}
			action=0;
			break;
		case 110:  
			fadeout(startpal,0,256,20);
			action=0;
			break;
	}
  } while (action<200);

	saveconfig();	 
	uninstSync();

	doneL();
	donesounds();
	kb_donekey();

	ShutDown = 1;

	mm_done();

	delete Std;
	return 0;
}
