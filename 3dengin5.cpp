 






 

 

#define __DOUBLEWALLS__

 
#define HVIEWRANGE	ANG90
 
#define VVIEWRANGE	ANG60

#define VIEWGLOBAL		(64l<<FIXSHIFT)
#define MINDIST			(8l<<FIXSHIFT)
 
 
#define	VIEWWIDTH		256
#define	VIEWHEIGHT		144
#define MAXVIEWWIDTH	320
#define MAXVIEWHEIGHT	200		 

#ifdef __DOUBLEWALLS__
#define	WALLSHEIGHT		(128l<<FIXSHIFT)
#else
#define	WALLSHEIGHT		(128l<<FIXSHIFT)
#endif

#define ITORAD		(3.141592657/ANG180)
#define RADTOI		ANG180/3.141592657

#define MAXRAIN		64

#define COL_WALL	79
#define	COL_ROCK1	127
#define COL_WOOD	47
#define	COL_MUFFA	111
#define	COL_ROCK2	191

#include "std.hpp"
#include "fastmem.hpp"
#include "fixed.h"
#include "mm4.hpp"
#include "lgraph.hpp"
#include "3dengine.hpp"
#include "kaos.hpp"
#include "events.hpp"	 
#include "doors.hpp"
#include "objects.hpp"
#include "3drflat.cpp"
#include <math.h>
#include <cmath>
#include "dos_trig_tables.hpp"
#include <cstdint>

#include <stdlib.h>  
 

fixed *sintab = new fixed[ANG360],
	  *costab = new fixed[ANG360],
	  *tantab = new fixed[ANG90+1],
	  *invcostab = new fixed[ANG90+1];
int *angtab = new int[MAXVIEWWIDTH];
int viewwidth, viewheight, halfwidth, halfheight;
fixed heightnumerator;
int viewangle, rayangle;
fixed viewx, viewy, viewz;
fixed viewcos, viewsin;

std::uint16_t *ceilhid = new std::uint16_t[MAXVIEWWIDTH],
	*floorhid = new std::uint16_t[MAXVIEWWIDTH];
int minceilrow, minfloorrow;
fixed facedist, objscale;
fixed distadj;	 

 

 
fixed xpos, ypos, xposinc, yposinc;
fixed xinters, yinters, xintinc, yintinc;
fixed dist, xdist, ydist, xdistinc, ydistinc;
int idx;
char isxray,isdiag,isnowall;
word raymask;	 
int noid,		 
	textind, otextind;	 

word *map;
word *objmap;
char seenmap[4096];
char mapsquare[25];	 
byte *floormap = new byte[4096];
byte *ceilmap = new byte[4096];
DrawList dobjlist;
 
char raindraw = 1;  
 
typedef struct { fixed x,y; } TRainInfo;
TRainInfo rain_info[MAXRAIN];
fixed rain_speed[MAXRAIN];
int thunderdraw = -1, thunderang, thunderz, underblack = 0;

#ifdef __DOUBLEWALLS__
byte HighWall[MAXTEXTURES] = {
T_WALL1 		,T_WALL1M ,
T_WALL1 		,T_WALL1M ,
T_WALL1 	,T_WALL1M ,
T_WALL1 		,T_WALL1M ,
T_WALL1 		,T_WALL1M ,
T_WALL1 		,T_WALL1M ,
T_WALL1 		,T_WALL1M ,
T_WALL1 		,T_WALL1M ,
T_WALL1 	,T_WALL1M ,
T_ARC 			,T_ARCM ,
T_PIETR1 	,T_PIETR1M ,
T_PIETR1 	,T_PIETR1M ,
T_PIETR1 	,T_PIETR1M ,
T_PIETR1 		,T_PIETR1M ,
T_PIETR1 		,T_PIETR1M ,
T_PIETR1 	,T_PIETR1M ,
T_ROCK 		,T_ROCK ,
T_ROCK 		,
T_WOOD1 		,T_WOOD1 ,
T_WOOD1 	,
T_WBOX 		,T_WBOX ,
T_WBOX 		,
T_OVERDOOR 	,T_OVERDOOR ,
T_OVERDOOR 	,T_OVERDOOR ,
T_OVERDOOR 	,T_OVERDOOR ,
T_OVERDOOR 	,T_OVERDOOR ,
T_WOOD2 		,T_WOOD2 ,
T_WOOD2 	,
T_SBARRE 	,T_SBARRE ,
T_WOOD1 	,

T_WALL1,T_WALL1M,T_PIETR1,T_PIETR1M,T_ROCK,T_WOOD1,T_WBOX,T_WOOD2,
T_WALL2 		,T_WALL2M ,
T_WALL2 		,T_WALL2M ,
T_PIETR2 	,T_PIETR2M ,
T_PIETR2 	,T_PIETR2M ,
T_EARTH 		,T_EARTH ,
T_SOFT 		,T_SOFTM ,
T_SOFT 		,T_SOFTM ,

T_DRAGON  ,T_DRAGON+1,T_DRAGON+2,T_DRAGON+3,T_DRAGON+4,
T_DRAGON+5,T_DRAGON+6,T_DRAGON+7,T_DRAGON+8,

T_GRATAW 	,T_GRATAW ,
T_GRATAB 	,T_GRATAB ,
T_FLROCK 	,T_FLROCK ,
 
T_OVERDOOR,T_DOORSLOT,T_WOOD1 
	};
#endif

 
byte LastCol[MAXTEXTURES] = {
COL_WALL 		,COL_MUFFA ,
COL_WALL 	,COL_MUFFA ,
COL_WALL 	,COL_MUFFA ,
COL_WALL 		,COL_MUFFA ,
COL_WALL 		,COL_MUFFA ,
COL_WALL 		,COL_MUFFA ,
COL_WALL 		,COL_MUFFA ,
COL_WALL 	,COL_MUFFA ,
COL_WALL 	,COL_MUFFA ,
COL_WALL 		,COL_MUFFA ,
COL_ROCK1 	,COL_MUFFA ,
COL_ROCK1 	,COL_MUFFA ,
COL_ROCK1 	,COL_MUFFA ,
COL_ROCK1 		,COL_MUFFA ,
COL_ROCK1 	,COL_MUFFA ,
COL_ROCK1 	,COL_MUFFA ,
COL_ROCK1 		,COL_ROCK1 ,
COL_ROCK1 	,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 	,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 	,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 		,COL_WOOD ,
COL_WOOD 	,
COL_ROCK1 	,COL_ROCK1 ,
COL_WOOD 	,

COL_WALL,COL_MUFFA,COL_ROCK1,COL_MUFFA,COL_ROCK1,COL_WOOD,COL_WOOD,COL_WOOD,
COL_WALL 		,COL_MUFFA ,
COL_WALL 	,COL_MUFFA ,
COL_ROCK2 	,COL_MUFFA ,
COL_ROCK2 	,COL_MUFFA ,
COL_WALL 		,COL_WALL ,
COL_WALL 		,COL_MUFFA ,
COL_WALL 		,COL_WALL ,

COL_WALL,COL_WALL,COL_WALL,COL_WALL,COL_WALL,
COL_WALL,COL_WALL,COL_WALL,COL_WALL,

COL_WOOD 	,COL_WOOD ,
COL_ROCK2 	,COL_ROCK2 ,
COL_ROCK1 	,COL_ROCK1 ,
 
COL_WOOD,COL_WOOD,COL_WOOD 
	};

 
#pragma warn -rvl
int angleadd(int a, int b) {
    int result = (a + b) % ANG360;
    if (result < 0)
        result += ANG360;
    return result;
}

 
int anglesub(int a, int b) {
     
    const std::uint16_t diff =
        static_cast<std::uint16_t>(a) - static_cast<std::uint16_t>(b);
    std::int16_t ax = static_cast<std::int16_t>(diff);
    if (ax < 0)
        ax = static_cast<std::int16_t>(ax + ANG360);
    return ax;
}
#pragma warn +rvl

void calctables() {
	 






	for (int i=0; i<ANG360; ++i) {
		sintab[i] = static_cast<fixed>(KAOS_DOS_SINTAB[i]);
		costab[i] = static_cast<fixed>(KAOS_DOS_COSTAB[i]);
	}
	for (int i=0; i<=ANG90; ++i) {
		tantab[i] = static_cast<fixed>(KAOS_DOS_TANTAB[i]);
		invcostab[i] = static_cast<fixed>(KAOS_DOS_INVCOSTAB[i]);
	}
}

void calcproject(fixed focal) {
	int i;
	float angfact;
	int angle;

	facedist = focal ;
	objscale = viewwidth*facedist/VIEWGLOBAL;

	 
	heightnumerator = ((std::int32_t)viewwidth*ANG60/(HVIEWRANGE-ANG30) >> 1);
				 

	float tang;
	for (i=0;i<halfwidth;i++) {
		tang = ((float)i+0.5)*VIEWGLOBAL/viewwidth/facedist;
		angle = static_cast<int>(std::atan(static_cast<double>(tang))*RADTOI+0.5);
		angtab[halfwidth+i] = angle;
		 
		angtab[halfwidth-i-1] = anglesub(ANG0,angle);
	}
	 
	 
	distadj = invcostab[ANG360-angtab[0]];	 
}

void setviewsize(int width, int height) {
	viewwidth = width;
	viewheight = height &= 0x7ffe;
	halfwidth = width >> 1;
	halfheight = height >> 1;
	calcproject(fixdiv64(0,32,tantab[HVIEWRANGE>>1]) );  
	backx = fixdiv(320l<<16,width);
	backy1 = fixdiv(200l<<16,height);  
	backy2 = backy1;
	{
		const std::uint32_t raw = static_cast<std::uint32_t>(backy2);
		const std::uint16_t hi = static_cast<std::uint16_t>(raw >> 16);
		const std::uint16_t lo = static_cast<std::uint16_t>(raw & 0xffffu);
		const std::uint16_t swapped = static_cast<std::uint16_t>(
			static_cast<std::uint16_t>(hi << 8) |
			static_cast<std::uint16_t>(hi >> 8));
		const std::uint16_t scaled_hi = static_cast<std::uint16_t>(
			swapped + static_cast<std::uint16_t>(swapped >> 2));
		backy2 = static_cast<fixed>(
			(static_cast<std::uint32_t>(scaled_hi) << 16) | lo);
	}
	reset_rain();
}

void reset_rain() {
	 
	int rainspeed = viewwidth/80;
	for (int i=0; i<MAXRAIN; i++) {
		rain_info[i].x = lshl16(random(viewwidth+4)-2);
		rain_info[i].y = lshl16(random(viewheight+4)-2);
		rain_speed[i] = lshl16(rainspeed)+lshl8(random(256));
	}
}

void near initray() {
	fixed xp, yp;
	fixed raytan, rayinvtan;
	 
	xpos = viewx & 0x7fc00000l;
	ypos = viewy & 0x7fc00000l;

	 
	 

	if (rayangle<ANG90) {
		yposinc = xposinc = (64l<<FIXSHIFT);
		xpos+=xposinc; ypos+=yposinc;
		yintinc = lshl6(raytan = tantab[rayangle]);
		xintinc = lshl6(rayinvtan = tantab[ANG90-rayangle]);  
		xdistinc = invcostab[rayangle];
		ydistinc = invcostab[ANG90-rayangle];
	} else
	if (rayangle<ANG180) {
		xposinc = -(yposinc = (64l<<FIXSHIFT));
		xpos--; ypos+=yposinc;
		yintinc = -lshl6(raytan = -tantab[ANG180-rayangle]);
		xintinc = lshl6(rayinvtan = -tantab[rayangle-ANG90]);
		xdistinc = -invcostab[ANG180-rayangle];
		ydistinc = invcostab[rayangle-ANG90];
	} else
	if (rayangle<ANG270) {
		yposinc = xposinc = -(64l<<FIXSHIFT);
		xpos--; ypos--;
		yintinc = -lshl6(raytan = tantab[rayangle-ANG180]);
		xintinc = -lshl6(rayinvtan = tantab[ANG270-rayangle]);  
		xdistinc = -invcostab[rayangle-ANG180];
		ydistinc = -invcostab[ANG270-rayangle];
	} else {
		yposinc = -(xposinc = (64l<<FIXSHIFT));
		ypos--; xpos+=xposinc;
		yintinc = lshl6(raytan = -tantab[ANG360-rayangle]);
		xintinc = -lshl6(rayinvtan = -tantab[rayangle-ANG270]);
		xdistinc = invcostab[ANG360-rayangle];
		ydistinc = -invcostab[rayangle-ANG270];
	  }
	yinters = viewy+fixmul(raytan,xp = xpos-viewx);
	xinters = viewx+fixmul(rayinvtan,yp = ypos-viewy);
	if (rayangle != ANG90 && rayangle != ANG270) {
		xdist = fixmul(xdistinc,xp);
		xdistinc = lshl6(LABS(xdistinc));
	} else xdist = 2000000000l;
	if (rayangle != ANG0 && rayangle != ANG180) {
		ydist = fixmul(ydistinc,yp);
		ydistinc = lshl6(LABS(ydistinc));
	} else ydist = 2000000000l;
}

 

















































#pragma warn -rvl
 
 
int near ray() {
     



    while (true) {
        if (xdist <= ydist) {
            const std::uint32_t xraw = static_cast<std::uint32_t>(xpos);
            const std::uint32_t yraw = static_cast<std::uint32_t>(yinters);
             
            if ((xraw >> 28) || (yraw >> 28))
                return -1;

            const int idx =
                static_cast<int>((xraw >> 16) & 0x0fc0u) +
                static_cast<int>((yraw >> 22) & 0x03ffu);

            if (!(seenmap[idx] & 1)) {
                if (objmap[idx] & OMM_VISIBLE) {
                    dist = xdist;
                    isxray = 1;
                    return idx;
                }
                ++seenmap[idx];
            }

            xpos += xposinc;
            yinters += yintinc;
            xdist += xdistinc;
        } else {
            const std::uint32_t xraw = static_cast<std::uint32_t>(xinters);
            const std::uint32_t yraw = static_cast<std::uint32_t>(ypos);
            if ((xraw >> 28) || (yraw >> 28))
                return -1;

            const int idx =
                static_cast<int>((xraw >> 16) & 0x0fc0u) +
                static_cast<int>((yraw >> 22) & 0x03ffu);

            if (!(seenmap[idx] & 1)) {
                if (objmap[idx] & OMM_VISIBLE) {
                    dist = ydist;
                    isxray = 0;
                    return idx;
                }
                ++seenmap[idx];
            }

            ypos += yposinc;
            xinters += xintinc;
            ydist += ydistinc;
        }
    }
}

#pragma warn +rvl

 






































































































void near raynext() {
    if (xdist <= ydist) {
        xpos += xposinc;
        yinters += yintinc;
        xdist += xdistinc;
    } else {
        ypos += yposinc;
        xinters += xintinc;
        ydist += ydistinc;
    }
}

 























char near raycast() {
	TMapInfo mapinf;
	int li, ls, k;
	char ok,fast;
	fixed num,den;
	Object *obj;
	 
	fixed _xinters, _yinters;
	 
	char checknowall; 

	ok = 0;
	isnowall = 0;
	checknowall = 1;
	for(;;) {
#ifdef __DEBUGMODE__
		if ((idx = ray())<0) return 0;
		 
		if (idx<0 || idx>4095) error("!!???!!?!?!?!?");
#else
		if ((idx = ray())<0) return 0;
#endif
		mapinf = Map[idx];
		if (!seenmap[idx]) {
		  seenmap[idx] |= 2;
		  if ((k = IDMap[idx]) >= 0) {
			 
			if (!mapinf.stop) seenmap[idx]++;
			 
			do {
				obj = objectslist.get(k);
				if (k != noid && GETFLAG(obj->mover->flags,OMF_VISIBLE)) {  
					 
					den = fixmul(obj->mover->x-viewx,viewcos)+
						  fixmul(obj->mover->y-viewy,viewsin);
					if (den>=MINDIST) {
						obj->seen();
						dobjlist.add(k,den);
					}
				}
			} while ((k = obj->mover->underq(idx)) >= 0);
		  }
		}

	   if (mapinf.stop ) {
		 
		switch (mapinf.shape) {
			case MSH_SQUARE:
				if (isxray) {
					 
					if (xposinc>=0l) column = lshr16(yinters) & 63;
								else column = ~lshr16(yinters) & 63;
				} else {
					 
					if (yposinc>=0l) column = ~lshr16(xinters) & 63;
								else column = lshr16(xinters) & 63;
				  }
				ok++;
				isdiag=0;
				break;
			case MSH_VERT:
				 
				if (isxray) {
					ls = (li = lshr16(yinters) & 0x7fc0) + 64;
					k = lshr16(_yinters = yinters + (yintinc>>1));
					if (k<li || k>=ls) break;
					dist = xdist + (xdistinc>>1);
					 
					if (xposinc>=0l) column = k & 63;
								else column = ~k & 63;
				} else {
					 
					if (yposinc>0l) ls = (li = lshr16(ypos)) + 64;
							   else li = (ls = lshr16(ypos)+1) - 64;
					k = lshr16(_yinters = yinters-(yintinc>>1));
					if (k<li || k>=ls) break;
					dist = xdist - (xdistinc>>1);
					 
					if (xposinc>=0l) column = k & 63;
								else column = ~k & 63;
					isxray = 1;	 
				  }
				ok++;
				isdiag=0;
				break;
			case MSH_HORIZ:
				 
				if (isxray) {
					 
					if (xposinc>0l) ls = (li = lshr16(xpos)) + 64;
							   else li = (ls = lshr16(xpos)+1) - 64;
					k = lshr16(_xinters = xinters-(xintinc>>1));
					if (k<li || k>=ls) break;
					dist = ydist - (ydistinc>>1);
					 
					if (yposinc>=0l) column = ~k & 63;
								else column = k & 63;
					isxray = 0;  
				} else {
					ls = (li = lshr16(xinters) & 0x7fc0) + 64;
					k = lshr16(_xinters = xinters + (xintinc>>1));
					if (k<li || k>=ls) break;
					dist = ydist + (ydistinc>>1);
					 
					if (yposinc>=0l) column = ~k & 63;
								else column = k & 63;
				  }
				ok++;
				isdiag=0;
				break;
			case MSH_DIAG1:
				 
				if (isxray) {
					if (xposinc>0l) {
						if ((num = yinters & 0x003fffffl) != 0l) {
							den = xposinc-yintinc;
							if (den>=num) {
								fast = 1;
								ok++;
							}
						} else {
							fast = 0;
							ok++;
						  }
					} else {
						if ((num = ~yinters & 0x003fffffl) != 0l) {
							den = yintinc-xposinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast = 0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_yinters = yinters+fixmuldiv64(yintinc,num,den);
							dist = xdist+fixmuldiv64(xdistinc,num,den);
						} else {
							_yinters = yinters;
							dist = ydist;
						  }
						 
						if (xposinc>=0l) column = lshr16(_yinters) & 63;
									else column = ~lshr16(_yinters) & 63;
					}
				} else {
					if (yposinc>0l) {
						if ((num = xinters & 0x003fffffl) != 0l) {
							den = yposinc-xintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = ~xinters & 0x003fffffl) != 0l) {
							den = xintinc-yposinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_xinters = xinters + fixmuldiv64(xintinc,num,den);
							dist = ydist + fixmuldiv64(ydistinc,num,den);
						} else {
							_xinters = xinters;
							dist = ydist;
						  }
						 
						if (yposinc>=0l) column = ~lshr16(_xinters) & 63;
									else column = lshr16(_xinters) & 63;
					}
				  }
				isdiag=1;
				break;
			case MSH_DIAG2:
				 
				if (isxray) {
					if (xposinc>0l) {
						if ((num = ~yinters & 0x003fffffl) != 0l) {
							den = xposinc+yintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = yinters & 0x003fffffl) != 0l) {
							den = -(yintinc+xposinc);
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_yinters = yinters + fixmuldiv64(yintinc,num,den);
							dist = xdist + fixmuldiv64(xdistinc,num,den);
						} else {
							_yinters = yinters;
							dist = xdist;
						  }
						 
						if (xposinc>=0l) column = lshr16(_yinters) & 63;
									else column = ~lshr16(_yinters) & 63;
					}
				} else {
					if (yposinc>0l) {
						if ((num = ~xinters & 0x003fffffl) != 0l) {
							den = yposinc+xintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = xinters & 0x003fffffl) != 0l) {
							den = -(xintinc+yposinc);
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_xinters = xinters + fixmuldiv64(xintinc,num,den);
							dist = ydist + fixmuldiv64(ydistinc,num,den);
						} else {
							_xinters = xinters;
							dist = ydist;
						  }
						 
						if (yposinc>=0l) column = ~lshr16(_xinters) & 63;
									else column = lshr16(_xinters) & 63;
					}
				  }
				isdiag=2;
				break;
			case MSH_DOOR:
				 
				if (isxray) {
					ls = (li = lshr16(yinters) & 0x7fc0) + 64;
					k = lshr16(_yinters = yinters + (yintinc>>1));
					if (k<li || k>=ls) break;
					dist = xdist + (xdistinc>>1);
					 
					column = k & 63;
					 



				} else {
					ls = (li = lshr16(xinters) & 0x7fc0) + 64;
					k = lshr16(_xinters = xinters + (xintinc>>1));
					if (k<li || k>=ls) break;
					dist = ydist + (ydistinc>>1);
					 
					column = ~k & 63;
					 



				  }
				ok++;
				isdiag=0;
				break;
		}
		if (ok) {
			switch (mapinf.type) {
				case MTP_WALL:
					 





					textind = mapinf.data;
					break;
				case MTP_SLOT:
					if (isxray)
						if (xposinc>0) k = idx-64;
									else k = idx+64;
					else
						if (yposinc>0) k = idx-1;
									else k = idx+1;
					if (Map[k].shape == MSH_DOOR) {
						textind = T_DOORSLOT;
						checknowall = 0;
					} else
						textind = mapinf.data;
					break;
				case MTP_DOOR1:
				case MTP_DOOR2:
					if ((k=doormanager.doorpos(mapinf.data))==64) {
						seenmap[idx]|=3;
						ok--;
						break;
					}
					if (mapinf.type == MTP_DOOR1) {
						ls=0;
						if (isdiag==1) {
							if (isxray) {if (xposinc<0l) ls=1;}
								   else {if (yposinc>0l) ls=1;}
						} else	 
						if (mapinf.shape != MSH_DOOR)
							if (isxray) {if (xposinc<0l) ls=1;}
								   else {if (yposinc<0l) ls=1;}
						if (ls) {
							if (column>63-k) {
								ok--;
								break;
							}
							column+=k;
						} else {
							if (column<k) {
								ok--;
								break;
							}
							column-=k;
						  }
					} else {
						k>>=1;
						if (column>=32) {
							if (column-32<k) {
								ok--;
								break;
							}
							column-=k;
						} else {
							if (31-column<k) {
								ok--;
								break;
							}
							column+=k;
						  }
					  }
					  textind = doormanager.doortype[mapinf.data];
					break;
				case MTP_GRID1:
					if (mapinf.shape != MSH_SQUARE) {
						if (isxray) k = lshr16(_yinters);
							   else k = lshr16(_xinters);
					} else
						if (isxray) k = lshr16(yinters);
							   else k = lshr16(xinters);
					 



					if (k%12 >= 4) {
						ok--;
						break;
					}
					 



					textind = mapinf.data;
					break;
			}
			 
		}
	   }
		if (ok) break;  
		raynext();
	}
	if (checknowall) {
		isnowall = (textind == T_NULL);
	}
	 
	return 1;
}

#pragma warn -rvl
int lookray(fixed x1, fixed y1, fixed x2, fixed y2, word mask) {
     












    const std::uint16_t x1h =
        static_cast<std::uint16_t>(static_cast<std::uint32_t>(x1) >> 16);
    const std::uint16_t y1h =
        static_cast<std::uint16_t>(static_cast<std::uint32_t>(y1) >> 16);
    const std::uint16_t x2h =
        static_cast<std::uint16_t>(static_cast<std::uint32_t>(x2) >> 16);
    const std::uint16_t y2h =
        static_cast<std::uint16_t>(static_cast<std::uint32_t>(y2) >> 16);

    std::uint16_t bx = static_cast<std::uint16_t>(x1h & 0xffc0u);
    const int xstart = static_cast<int>(bx >> 6);
    const int ystart = static_cast<int>(y1h >> 6);
    bx = static_cast<std::uint16_t>(bx + static_cast<std::uint16_t>(ystart));

    std::uint16_t si = static_cast<std::uint16_t>(x2h & 0xffc0u);
    const int xend = static_cast<int>(x2h >> 6);
    si = static_cast<std::uint16_t>(si +
                                    static_cast<std::uint16_t>(y2h >> 6));

    int dex = xend - xstart;
    int dey = static_cast<int>(y2h >> 6) - ystart;
    int xdir = 1;
    int ydir = 1;

    if (dex < 0) { xdir = -1; dex = -dex; }
    if (dey < 0) { ydir = -1; dey = -dey; }

    if (bx == si)
        return 0;

    int d1, d2;
    int dx1, dx2, dy1, dy2;
    int steps;
    int err;

    if (dex >= dey) {
        steps = dex;
        d1 = dey << 1;            
        err = d1 - dex;           
        d2 = err - dex;           
        dy1 = 0;
        dy2 = 2;
        dx1 = 128;
        dx2 = 128;
    } else {
        steps = dey;
        d1 = dex << 1;
        err = d1 - dey;
        d2 = err - dey;
        dx1 = 0;
        dx2 = 128;
        dy1 = 2;
        dy2 = 2;
    }

    if (xdir < 0) {
        dx1 = -dx1;
        dx2 = -dx2;
    }
    if (ydir < 0) {
        dy1 = -dy1;
        dy2 = -dy2;
    }

    int ptr = static_cast<int>(bx) * 2;
    const int step1 = dx1 + dy1;
    const int step2 = dx2 + dy2;
    --steps;                          
    if (steps <= 0)
        return 0;

    int obstacles = 0;
    for (int n = 0; n < steps; ++n) {
        if (err >= 0) {               
            ptr += step2;
            err += d2;
        } else {
            ptr += step1;
            err += d1;
        }

        const int idx = ptr >> 1;
        if (idx < 0 || idx >= 4096)
            break;
        if (objmap[idx] & mask)
            ++obstacles;
    }

    return obstacles;
}

#pragma warn +rvl


#pragma warn -rvl
 
 
int near enhray() {
    while (true) {
        if (xdist <= ydist) {
            const std::uint32_t xraw = static_cast<std::uint32_t>(xpos);
            const std::uint32_t yraw = static_cast<std::uint32_t>(yinters);
            if ((xraw >> 28) || (yraw >> 28))
                return -1;

            const int idx =
                static_cast<int>((xraw >> 16) & 0x0fc0u) +
                static_cast<int>((yraw >> 22) & 0x03ffu);

            if (idx < 0 || idx >= 4096)
                return -1;

             
            if (objmap[idx] & raymask) {
                isxray = 1;
                return idx;
            }

            xpos += xposinc;
            yinters += yintinc;
            xdist += xdistinc;
        } else {
            const std::uint32_t xraw = static_cast<std::uint32_t>(xinters);
            const std::uint32_t yraw = static_cast<std::uint32_t>(ypos);
            if ((xraw >> 28) || (yraw >> 28))
                return -1;

             
            const int idx =
                static_cast<int>((xraw >> 16) & 0x7fc0u) +
                static_cast<int>((yraw >> 22) & 0x03ffu);

            if (idx < 0 || idx >= 4096)
                return -1;

            if (objmap[idx] & raymask) {
                isxray = 0;
                return idx;
            }

            ypos += yposinc;
            xinters += xintinc;
            ydist += ydistinc;
        }
    }
}

#pragma warn +rvl

 





















































































int fireray(fixed &x, fixed &y, int angle, int vangle,  
			 fixed &_dist, fixed &z, int _noid) {
	TMapInfo mapinf;
	int li, ls, k;
	char ok, buffered, fast;
	fixed num, den, alfa;
	Object *obj;
	 
	fixed _xinters, _yinters,
			eorx=0,eory=0,eorz=0,maxdist;	 
	int eor = -1;	 

	 
	if (!vangle) {
		maxdist=4096l<<FIXSHIFT;
		z = 32l<<FIXSHIFT;
		 

	} else {
	  if (vangle<ANG90)
		maxdist=fixdiv64shl16(WALLSHEIGHT-z,tantab[vangle]);
	  else
		maxdist=fixdiv64shl16(z,tantab[ANG360-vangle]);
	   
	  eorx = x+fixmul(maxdist,costab[angle]);
	  if (eorx>=0 && eorx<(4096l<<FIXSHIFT)) {
		eory = y+fixmul(maxdist,sintab[angle]);
		if (eory>=0 && eory<(4096l<<FIXSHIFT))
			k = xytoidx(eorx,eory);
		else k=-1;
	  } else k=-1;
	  if (k>=0) {
		if (vangle<ANG90) {	 
			eor = ceilmap[k]<T_TRANSP ? -1 : -2;
			eorz = WALLSHEIGHT;
		} else {	 
			eor = floormap[k]<T_TRANSP ? -1 : -2;
			eorz = 0;
		  }
	  } else eor = -2;
	  maxdist += FIXONE<<4;	 
	}

	 
	 
	 
	viewx = x-(costab[angle]<<4);  
	viewy = y-(sintab[angle]<<4);
	rayangle = angle;
	noid = _noid;
	raymask = OMM_HITABLE;

	initray();

	ok = 0;
	for(;;) {
		buffered = 0;
		if ((idx = enhray())<0) return -2;
		 
















		mapinf = Map[idx];
		if ((k = IDMap[idx]) >= 0) {
			do {
				obj = objectslist.get(k);
				if (k != noid && (obj->mover->flags & OMF_HITABLE)) {
					if (isxray) {
						_xinters = obj->mover->x;
						alfa = LABS(fixdiv(_xinters-xpos,lshr16(xposinc)));
						_yinters = yinters + fixmul(yintinc,alfa);
					} else {
						_yinters = obj->mover->y;
						alfa = LABS(fixdiv(_yinters-ypos,lshr16(yposinc)));
						_xinters = xinters + fixmul(xintinc,alfa);
					  }
					den = FIXONE<<obj->mover->shdim;
					if (LABS(obj->mover->x-_xinters)+
						LABS(obj->mover->y-_yinters)<=den) {
						_dist = fixmul(obj->mover->x-viewx,costab[angle])+
								fixmul(obj->mover->y-viewy,sintab[angle])-
								den;
						if (_dist>maxdist) {
							x=eorx; y=eory; z=eorz; _dist=maxdist-(FIXONE<<4);
							return eor;
						}

						if (vangle<ANG90)
							num = z+fixmul(_dist,tantab[vangle]);
						else
							num = z-fixmul(_dist,tantab[ANG360-vangle]);
						 













						if (LABS(num-obj->mover->z) <= den) {
							x = _xinters;
							y = _yinters;
							z = num;
							return k;
						}
					}
				}
				k = obj->mover->underq(idx);
			} while (k>=0);
		}
		if (mapinf.stop) {

		 













		 
		switch (mapinf.shape) {
			case MSH_SQUARE:
				if (isxray) {
					if (xposinc>=0l) column = lshr16(yinters) & 63;
								else column = ~lshr16(yinters) & 63;
					_dist = xdist;
				} else {
					if (yposinc>=0l) column = ~lshr16(xinters) & 63;
								else column = lshr16(xinters) & 63;
					_dist = ydist;
				  }
				ok++;
				break;
			case MSH_VERT:
				if (isxray) {
					ls = (li = lshr16(yinters) & 0x7fc0) + 64;
					k = lshr16(_yinters = yinters + (yintinc>>1));
					if (k<li || k>ls) break;
					_xinters = xpos + (xposinc>>1);
					_dist = xdist + (xdistinc>>1);
					buffered++;
					if (xposinc>=0l) column = k & 63;
								else column = ~k & 63;
				} else {
					 
					if (yposinc>0l) ls = (li = lshr16(ypos)) + 64;
							   else li = (ls = lshr16(ypos)+1) - 64;  
					k = lshr16(_yinters = yinters-(yintinc>>1));
					if (k<li || k>ls) break;
					_xinters = xpos - (xposinc>>1);
					_dist = xdist - (xdistinc>>1);
					buffered++;
					isxray = 1;
					if (xposinc>=0l) column = k & 63;
								else column = ~k & 63;
				  }
				ok++;
				break;
			case MSH_HORIZ:
				if (isxray) {
					 
					if (xposinc>0l) ls = (li = lshr16(xpos)) + 64;
							   else li = (ls = lshr16(xpos)+1) - 64;  
					k = lshr16(_xinters = xinters-(xintinc>>1));
					if (k<li || k>ls) break;
					_yinters = ypos - (yposinc>>1);
					_dist = ydist - (ydistinc>>1);
					buffered++;
					isxray = 0;
					if (yposinc>=0l) column = ~k & 63;
								else column = k & 63;
				} else {
					ls = (li = lshr16(xinters) & 0x7fc0) + 64;
					k = lshr16(_xinters = xinters + (xintinc>>1));
					if (k<li || k>ls) break;
					_yinters = ypos + (yposinc>>1);
					_dist = ydist + (ydistinc>>1);
					buffered++;
					if (yposinc>=0l) column = ~k & 63;
								else column = k & 63;
				  }
				ok++;
				break;
			case MSH_DIAG1:
				if (isxray) {
					if (xposinc>0l) {
						if ((num = yinters & 0x003fffffl) != 0l) {
							den = xposinc-yintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = ~yinters & 0x003fffffl) != 0l) {
							den = yintinc-xposinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_xinters = xpos+fixmuldiv64(xposinc,num,den);
							_yinters = yinters + fixmuldiv64(yintinc,num,den);
							_dist = xdist + fixmuldiv64(xdistinc,num,den);
						} else {
							_xinters = xpos;
							_yinters = yinters;
							_dist = xdist;
						  }
						buffered++;
						if (xposinc>=0l) column = lshr16(_yinters) & 63;
									else column = ~lshr16(_yinters) & 63;
					}
				} else {
					if (yposinc>0l) {
						if ((num = xinters & 0x003fffffl) != 0l) {
							den = yposinc-xintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = ~xinters & 0x003fffffl) != 0l) {
							den = xintinc-yposinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_yinters = ypos+fixmuldiv64(yposinc,num,den);
							_xinters = xinters+fixmuldiv64(xintinc,num,den);
							_dist = ydist+fixmuldiv64(ydistinc,num,den);
						} else {
							_yinters = ypos;
							_xinters = xinters;
							_dist = ydist;
						  }
						buffered++;
						if (yposinc>=0l) column = ~lshr16(_xinters) & 63;
									else column = lshr16(_xinters) & 63;
					}
				  }
				break;
			case MSH_DIAG2:
				if (isxray) {
					if (xposinc>0l) {
						if ((num = ~yinters & 0x003fffffl) != 0l) {
							den = xposinc+yintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = yinters & 0x003fffffl) != 0l) {
							den = -(yintinc+xposinc);
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_xinters = xpos+fixmuldiv64(xposinc,num,den);
							_yinters = yinters + fixmuldiv64(yintinc,num,den);
							_dist = xdist + fixmuldiv64(xdistinc,num,den);
						} else {
							_xinters = xpos;
							_yinters = yinters;
							_dist = xdist;
						  }
						buffered++;
						if (xposinc>=0l) column = lshr16(_yinters) & 63;
									else column = ~lshr16(_yinters) & 63;
					}
				} else {
					if (yposinc>0l) {
						if ((num = ~xinters & 0x003fffffl) != 0l) {
							den = yposinc+xintinc;
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					} else {
						if ((num = xinters & 0x003fffffl) != 0l) {
							den = -(xintinc+yposinc);
							if (den>=num) {
								fast=1;
								ok++;
							}
						} else {
							fast=0;
							ok++;
						  }
					  }
					if (ok) {
						if (fast) {
							_yinters = ypos+fixmuldiv64(yposinc,num,den);
							_xinters = xinters + fixmuldiv64(xintinc,num,den);
							_dist = ydist + fixmuldiv64(ydistinc,num,den);
						} else {
							_yinters = ypos;
							_xinters = xinters;
							_dist = ydist;
						  }
						buffered++;
						if (yposinc>=0l) column = ~lshr16(_xinters) & 63;
									else column = lshr16(_xinters) & 63;
					}
				  }
				break;
			case MSH_DOOR:
				if (isxray) {
					ls = (li = lshr16(yinters) & 0x7fc0) + 64;
					k = lshr16(_yinters = yinters + (yintinc>>1));
					if (k<li || k>=ls) break;
					_xinters = xpos + (xposinc>>1);
					_dist = xdist + (xdistinc>>1);
					buffered++;
					if (xposinc>=0l) column = k & 63;
								else column = ~k & 63;
				} else {
					ls = (li = lshr16(xinters) & 0x7fc0) + 64;
					k = lshr16(_xinters = xinters + (xintinc>>1));
					if (k<li || k>=ls) break;
					_yinters = ypos + (yposinc>>1);
					_dist = ydist + (ydistinc>>1);
					buffered++;
					if (yposinc>=0l) column = ~k & 63;
								else column = k & 63;
				  }
				ok++;
				break;
		}
		if (ok) {
			switch (mapinf.type) {
				case MTP_WALL:
				case MTP_SLOT:	break;
				case MTP_DOOR1:
				case MTP_DOOR2:
					if ((k=doormanager.doorpos(mapinf.data))==64) {
						ok--;
						break;
					}
					if (mapinf.type == MTP_DOOR1) {
						ls=0;
						if (isdiag==1) {
							if (isxray) {if (xposinc<0l) ls++;}
								   else {if (yposinc>0l) ls++;}
						} else
						if (mapinf.shape != MSH_DOOR)
							if (isxray) {if (xposinc<0l) ls++;}
								   else {if (yposinc<0l) ls++;}
						if (ls) {
							if (column>63-k) {
								ok--;
								break;
							}
							column+=k;
						} else {
							if (column<k) {
								ok--;
								break;
							}
							column-=k;
						  }
					} else {
						k>>=1;
						if (column>=32) {
							if (column-32<k) {
								ok--;
								break;
							}
							column-=k;
						} else {
							if (31-column<k) {
								ok--;
								break;
							}
							column+=k;
						  }
					  }
					break;
				case MTP_GRID1:
					if (buffered)
						if (isxray) k = lshr16(_yinters);
							   else k = lshr16(_xinters);
					else
						if (isxray) k = lshr16(yinters);
							   else k = lshr16(xinters);
					if (isxray) {if (xposinc<0l) column = 63-column;}
						   else {if (yposinc<0l) column = 63-column;}
					if ((k % 12) >= 4) {
						ok--;
						break;
					}
					break;
			}
		}
		}

		if (ok) {
			if (_dist>maxdist) {
				x=eorx; y=eory; z=eorz; _dist=maxdist-(FIXONE<<4);
				return eor;
			}
			break;
		} else
		if (isxray) {
			if (xdist>maxdist) {
				x=eorx; y=eory; z=eorz; _dist=maxdist-(FIXONE<<4);
				return eor;
			}
		} else {
			if (ydist>maxdist) {
				x=eorx; y=eory; z=eorz; _dist=maxdist-(FIXONE<<4);
				return eor;
			}
		  }
		 
		raynext();
	}
	if (buffered) {
		y = _yinters;
		x = _xinters;
	} else {
		if (isxray) {
			x = xpos;
			y = yinters;
		} else {
			x = xinters;
			y = ypos;
		  }
	  }
	 
	 
	_dist-=FIXONE<<4;
	if (vangle) {
		if (vangle<ANG90)
			z = z+fixmul(_dist,tantab[vangle]);
		else
			z = z-fixmul(_dist,tantab[ANG360-vangle]);
	} else z = 32l<<FIXSHIFT;
	 
	return -1;
}

 




















































static void calc_floor_ceiling_increments(int row, int horizon,
                                              fixed numerator,
                                              fixed leftcos, fixed leftsin,
                                              bool floor_row)
{
     
    const int delta = floor_row ? (row - horizon + 1) : (horizon - row);
    if (delta <= 0) return;

    dist = numerator / delta;
    const fixed d = fixmul(dist, distadj);
    const fixed desty = fixmul(d, leftcos);
    const fixed destx = fixmul(d, leftsin);

    curx = viewx + desty;
    cury = viewy + destx;

    const fixed targetx = viewx - destx;
    const fixed targety = viewy + desty;

     

    curincx = (targetx - curx) / viewwidth;
    curincy = (targety - cury) / viewwidth;
}

void drawview(int cx, int cy, fixed x, fixed y, fixed z,
			  int hang, int vang, int _noid,
			  char &backceildraw, char &backfloordraw, char fixshade) {
	const int quakesh[8]  = { 1, 2, 0,-3,-1, 1,-1, 0};
	const int quakeshz[8] = { 0, 1,-2, 0,-1, 3, 1,-1};
	int i, j;
	int *angptr, angrel;
	int height, startrow, endrow;
	fixed lstart, lend;
	int shade1;
	int horizont;
	word *coldistptr = coldist;
	Object *obj;
	int objid;
	char rain_near;
	byte oldlast;

	 
	rain_near = (i = ceilmap[xytoidx(x,y)])==T_DOORSLOT || i>=T_TRANSP;

	viewtop = cy-halfheight;
	viewbottom = cy+halfheight;
	viewleft = cx-halfwidth;
	viewright = cx+halfwidth;

	viewangle = hang;
	viewcos = costab[viewangle];
	viewsin = sintab[viewangle];

	 
	if (quake) {
		x += fixmul(-viewsin,kaos_fshl(quakesh[quake & 7],FIXSHIFT-1));
		y += fixmul( viewcos,kaos_fshl(quakesh[quake & 7],FIXSHIFT-1));
		z += kaos_fshl(quakeshz[quake & 7],FIXSHIFT-2);
	}

	viewx = x; viewy = y; viewz = z;

	noid = _noid;

	if (vang) {
		if (vang<ANG90) {
			horizont = cy+fixmul(heightnumerator,tantab[vang]);
		}
 





		else
			horizont = cy+fixmul(heightnumerator,-tantab[ANG360-vang]);
	} else horizont = cy;

#ifdef __DEBUGMODE__
	backceildraw=0;
	backfloordraw=0;
#else
	i = -1;
	if ((oldlast = backceildraw) != 0) {	 
	  if ((texture = mm_recall(cbackg0)) != NULL) {
		drawbackground(i = (int)fixdiv((std::int32_t)hang*viewwidth,ANG90) % viewwidth,
					   horizont-halfheight);
		if (horizont>cy)
		  if ((texture = mm_recall(cbackg1)) != NULL)
			drawbackground(i,horizont-viewheight);
	  } else
		if (horizont>viewtop)
		  ffillblock(viewleft,viewtop,viewwidth,horizont-viewtop,0x7f7f7f7fl);
		   
	  if (raindraw && !rain_near && !underblack) {
		drawrain();
		oldlast = 0;	 
	  }
	  if (thunderdraw!=-1 &&
		  (texture = mm_recall(thunderdraw)) != NULL) {
		if (thunderz+128>viewtop &&
			(j = cx+(viewwidth*4/360)*(thunderang-hang)/ANGLVL) >= viewleft-80 &&
			 j<=viewright) {
			 




			setvrect(viewleft,viewtop,viewwidth,viewheight);
			scalerscfig(j,horizont-thunderz,80,128,
						(80l*viewwidth)/320,(128l*viewwidth)/320,
						fixshade,texture);
		}
	  }
	  backceildraw=0;
	}
	if (backfloordraw) {
	  if ((texture = mm_recall(fbackg0)) != NULL) {
		if (i<0)
			drawbackground(i = (int)fixdiv((std::int32_t)hang*viewwidth,ANG90) % viewwidth,
						   horizont);
		else
			drawbackground(i,horizont);
		if (horizont<cy)
		  if ((texture = mm_recall(fbackg1)) != NULL)
			drawbackground(i,horizont+halfheight);
	  } else
		if (horizont<viewbottom)
		  ffillblock(viewleft,horizont,viewwidth,viewbottom-horizont,0x7f7f7f7fl);
	  backfloordraw=0;
	}
	if (oldlast && raindraw && !rain_near) drawrain();
#endif

	angptr = angtab;

	fdfill(seenmap,0l,4096);  

	minceilrow = viewtop;
	minfloorrow = viewbottom;

	fixed prez  = z*heightnumerator,
#ifndef __DOUBLEWALLS__
		  preiz = (4194304l-z)*heightnumerator,
#else
		  preiz = ((4194304l<<1)-z)*heightnumerator,
#endif
		  prehoriz = lshl16(horizont);

	std::uint16_t *chid = ceilhid,
		*fhid = floorhid;

	otextind = -1;	 
	oldlast = COL_ROCK1;

	for (linex=viewleft; linex<viewright; linex++,chid++,fhid++,coldistptr++) {
		if ((rayangle = hang + (angrel = *(angptr++))) >= ANG360) rayangle -= ANG360;
		initray();
		if (raycast()) {
			 







			 
			 





			 
			 
			dist = fixmul(dist,costab[angrel]);
			if (dist<MINDIST) dist = MINDIST;

			*coldistptr = lshr16(dist);
			 




			lstart = prehoriz-fixdiv64shl16(preiz,dist);
			liney = lshr16(lstart+FIXONE-1);
			lend = prehoriz+fixdiv64shl16(prez,dist);
			endrow = lshr16(lend);
			lineh = endrow-liney;	 
			 










			 











			if (liney<viewtop) {
				lineh += liney-viewtop;  
				liney = viewtop;
			}
			if (endrow>viewbottom) {
				endrow = viewbottom;
				lineh = viewbottom-liney;
			}

			if (lineh<=2) {
				 
				 
				liney = horizont-1;
				minceilrow = horizont-2;
				minfloorrow = horizont+1;
				*chid = minceilrow-viewtop;
				*fhid = viewbottom-minfloorrow;
				lineh=2;  
				vline(oldlast = LastCol[textind]);
				continue;
			}
#ifndef __DOUBLEWALLS__
			wstep = fixdiv64(0l,1024l,lend-lstart);
#else
			wstep = fixdiv64(0l,2048l,lend-lstart);
#endif
			 




			 


			if ((curx = lshl16(liney)-lstart) != 0l) curx--;
			woffs = fixmul(wstep,curx);
			if (liney>minceilrow) minceilrow = liney-1;
			if (endrow<minfloorrow) minfloorrow = endrow;
			*chid = liney-viewtop;
			*fhid = viewbottom-endrow;

			if (isnowall) {
				backfloordraw = backceildraw = 1;
				continue;
			}

			shade = ((lshr16(dist)-16)>>5)+2-LightMap[idx]+fixshade;
			if (isxray)
				switch(isdiag) {
					case 0: shade -= ((LABS(costab[rayangle])-1)>>14); break;
					case 1: shade -= ((LABS(costab[angleadd(rayangle,ANG45)])-1)>>14); break;
					case 2: shade -= ((LABS(costab[anglesub(rayangle,ANG45)])-1)>>14); break;
				}
			else
				switch(isdiag) {
					case 0: shade -= ((LABS(sintab[rayangle])-1)>>14); break;
					case 1: shade -= ((LABS(sintab[anglesub(rayangle,ANG45)])-1)>>14); break;
					case 2: shade -= ((LABS(sintab[angleadd(rayangle,ANG45)])-1)>>14); break;
				}
			if (dist<24l<<FIXSHIFT)		 
				shade += lshr16((24l<<FIXSHIFT)-dist)>>1;
			if (shade<0) shade=0; else
			 if (shade>MAXSHADE) {
				shade = MAXSHADE;
#ifdef __DEBUGMODE__
				vline(128-10);
#else
				 
				vline(oldlast = LastCol[textind]);
#endif
				continue;
			 }
			if (textind != otextind) {
#ifdef __DOUBLEWALLS__
				 
				 
				 
				 
				 if (textind==T_ARC) {
					texture = mm_recall(j = T_WALL1);
					otextind=textind;
				 } else
				 if (textind==T_ARCM) {
					texture = mm_recall(j = T_WALL1M);
					otextind=textind;
				 } else
#endif
				texture = mm_recall(otextind = j = textind);
#ifdef __DOUBLEWALLS__
				i = HighWall[textind];
				if (i != j ) {
					mm_lock(j );
					htexture = mm_recall(i);
					mm_unlock(j );
				}
#endif
			}
#ifdef __DOUBLEWALLS__
			if (i == j ) drawwallHP(shade);
							  else drawwallHP2(shade);
#else
			drawwallHP(shade);
#endif
			 
			 
		} else {
			 
			 
			 
			*fhid = *chid = 0;
			*coldistptr = 0l;  
		  }
	}

	idx = xytoidx(x,y);
	seenmap[idx]|=3;
	objid = IDMap[idx];
	while (objid>=0) {
		obj = objectslist.get(objid);
		if (objid != noid) {
			dist = fixmul(obj->mover->x-viewx,viewcos)+
				   fixmul(obj->mover->y-viewy,viewsin);
			if (dist>MINDIST) {
				obj->seen();
				dobjlist.add(objid,dist);
			}
		}
		objid = obj->mover->underq(idx);
	}

#ifndef __DEBUGMODE__
	  
	if (minceilrow>=viewbottom) minceilrow=viewbottom-1;
	  
	if (minfloorrow<viewtop) minfloorrow=viewtop;

	hang = angleadd(viewangle,angtab[0]);
	fixed leftcos = costab[hang],
		  leftsin = sintab[hang];
	maxcolumn = viewwidth;
	hid = floorhid;
	otextind = -1;	 
	 
	for (liney=minfloorrow, lineh=viewbottom-minfloorrow-1; liney<viewbottom; liney++, lineh--) {
	  
		 
		calc_floor_ceiling_increments(liney, horizont, prez, leftcos, leftsin, true);

		shade1 = ((lshr16(dist)-16)>>5)-1+fixshade;
		 












		column = 0;
		linex = viewleft;
		init_fc();
		idx = xytoidx(curx,cury);
		while (column<maxcolumn) {
			 








			if (idx < 0 || idx >= 4096 || !seenmap[idx]) {
				idx = fast_passfc();
				continue;
			}

			if ((textind=floormap[idx])!=otextind)
				if (textind >= T_NULL) {
					idx = fast_passfc();
					backfloordraw = 1;
					continue;
				} else texture = mm_recall(otextind=textind);
			shade = shade1-LightMap[idx];
			if (shade<0) shade=0; else
			 if (shade>MAXSHADE) {
				idx = fast_drawfcblack(oldlast);
				continue;
			 }
			if (textind<T_TRANSP)
				idx = fast_drawfc();
			else {
				if (underblack && textind<T_FLROCK)
					idx = fast_drawtfc(127);
				else {
					idx = fast_drawtfc(0);
					backfloordraw = 1;
				  }
			}
		}
	}

	if (otextind==T_DOORSLOT) otextind = -1;	 
	hid = ceilhid;
	for (liney=minceilrow, lineh=minceilrow-viewtop; liney>=viewtop; liney--, lineh--) {
	    
		 
		calc_floor_ceiling_increments(liney, horizont, preiz, leftcos, leftsin, false);

		shade1 = ((lshr16(dist)-16)>>5)-1+fixshade;
		column = 0;
		linex = viewleft;
		init_fc();
		idx = xytoidx(curx,cury);
		while (column<maxcolumn) {
			 









			if (idx < 0 || idx >= 4096 || !seenmap[idx]) {
				idx = fast_passfc();
				continue;
			}

			if ((textind = ceilmap[idx])!=otextind)
				if (textind>=T_NULL) {
					otextind=-1;
					idx = fast_passfc();
					backceildraw = 1;
					continue;
				} else texture = mm_recall(otextind=textind);
			shade = shade1-LightMap[idx];
			if (shade<0) shade=0; else
			 if (shade>MAXSHADE) {
				idx = fast_drawfcblack(oldlast);
				continue;
			 }
			if (textind<T_TRANSP)
				idx = fast_drawtfc(127);
			else {
				idx = fast_drawtfc(0);
				backceildraw = 1;
			}
		}
	}
#endif

#ifndef __DEBUGMODE__
	 
	int app, vx, ovx, idist;
	fixed destx, desty;
	while ((objid = dobjlist.get(dist))>=0) {
			obj = objectslist.get(objid);

			shade = (((idist = lshr16(dist))-16)>>5)-1-obj->selflight-
					LightMap[xytoidx(obj->mover->x,obj->mover->y)]+fixshade;
			if (dist<24l<<FIXSHIFT)		 
				shade += lshr16((24l<<FIXSHIFT)-dist)>>1;
			if (shade<0) shade=0; else
				if (shade>MAXSHADE)
					 
					continue;  

			destx = fixmul(obj->mover->y-y,viewcos)-
					fixmul(obj->mover->x-x,viewsin);
			ovx = vx = cx+(app = (int)fixdiv((destx>>2)*objscale,dist>>2));
			app += halfwidth;
			if (app<0) app=0; else
			 if (app>=viewwidth) app=viewwidth-1;
			app = viewangle+angtab[app];
			if (app<0) app+=ANG360; else
			 if (app>=ANG360) app-=ANG360;

			 
			TFigure &fig = FigList[(i = obj->getviewfig(app)) & 0x3fff];  
			texture = mm_recall(fig.memidx);

			 
			destx = z - obj->mover->z - lshl16(fig.height);
			lend = prehoriz+fixdiv64shl16(destx*heightnumerator,dist);
			lstart = prehoriz+fixdiv64shl16((destx-lshl16(fig.dy))*heightnumerator,dist);
			if (lshr16(lstart)>=viewbottom || lshr16(lend)<viewtop) continue;
			height = lshr16(lend-lstart);
			if (height<=0) continue;
		    
			j = ((std::int32_t)height*fig.dx*15/fig.dy)>>4;
			if (j<=0) continue;
			vx -= j>>1;
			if (vx+j<viewleft || vx>=viewright) continue;

			if (i & 0x4000)	 
				drawrcobjHP(vx,lshr16(lstart),fig.dx,fig.dy,
					   j,height,idist-(1<<(obj->mover->shdim-1)),shade,texture);
			else {
				if (GETFLAG(obj->oflags,OFL_ISLIGHT)) {
					shade = (7-shade-((LightActor *)obj)->lightpow);
					if (shade<-1) shade=-1;
					drawclightHP(vx,lshr16(lstart),fig.dx,fig.dy,
					   j,height,idist-(1<<(obj->mover->shdim-1)),shade,texture);
					break;
				} else
					drawcobjHP(vx,lshr16(lstart),fig.dx,fig.dy,
					   j,height,idist-(1<<(obj->mover->shdim-1)),shade,texture);

				if (GETFLAG(obj->oflags,OFL_HASLIGHT)) {
				 i = ((LightActor *)obj)->getlightfig();
				 if (i > -1) {
				  shade = (((idist = lshr16(dist))-16)>>4)-1-  
						LightMap[xytoidx(obj->mover->x,obj->mover->y)]+fixshade;
				  if (shade<0) shade=0; else
					if (shade>MAXSHADE) shade=MAXSHADE;
				  TFigure &lightfig = FigList[i];  
				  texture = mm_recall(lightfig.memidx);
				  destx = z - ((LightActor *)obj)->lightz - lshl16(lightfig.height);
				  lend = prehoriz+fixdiv64shl16(destx*heightnumerator,dist);
				  lstart = prehoriz+fixdiv64shl16((destx-lshl16(lightfig.dy))*heightnumerator,dist);
				  if (lshr16(lstart)>=viewbottom || lshr16(lend)<viewtop) continue;
				  height = lshr16(lend-lstart);
				  j = ((std::int32_t)height*lightfig.dx*15/lightfig.dy) >> 4;
				  ovx -= j >> 1;
				  shade = (7-shade);
				  if (shade <= 0) shade = ((-shade) >> 2) + 1;
				  shade -= ((LightActor *)obj)->lightpow;
				  if (shade<-1) shade=-1;
				  drawclightHP(ovx,lshr16(lstart),lightfig.dx,lightfig.dy,
					   j,height,idist-(lightfig.dx >> 1),shade,texture);
				 }
				}
			}
	}
#else
	dobjlist.flush();
#endif
	if (raindraw && rain_near) drawrain();
}

void invpix(int x, int y, char shader) {
    if (!vpage || x < 0 || x >= 320 || y < 0 || y >= 200)
        return;

    byte value = vpage[y * 320 + x];

    byte intensity = static_cast<byte>(15 - value);
    intensity = static_cast<byte>((-static_cast<signed char>(intensity)) & 0x0f);
    intensity = static_cast<byte>(intensity | 0xe0);

    byte shaded = static_cast<byte>(intensity + static_cast<byte>(shader));
    const byte hi = static_cast<byte>((intensity ^ shaded) & HISHMASK);

    if (hi)
        shaded = static_cast<byte>((shaded ^ hi) | LOSHMASK);

    vpage[y * 320 + x] = shaded;
}


void drawrain() {
	byte col;
	int sx = viewleft, sy = viewtop;
	int i,j,k,ix,iy,
		app = viewwidth>>7;
	fixed x,rs,dex;
	for (i=0; i<MAXRAIN; i++) {
		TRainInfo &ri = rain_info[i];
		x = ri.x; iy = lshr16(ri.y);
		k = fixmul(app,(rs = rain_speed[i])>>1);
		dex = fixmul(rs>>4,viewsin);
		for (j=0,col=k;j<k;j++,iy++,col--) {
			ix = lshr16(x); x += dex;
			if (ix>=0 && ix<viewwidth && iy>=0 && iy<viewheight)
				invpix(ix+sx,iy+sy,col);
				 
				 














		}
	}
}

 















































































void anim_rain() {
	int i;
	fixed rb = lshl16(viewwidth+2),
		  bb = lshl16(viewheight+2),
		  spd = lshl16(viewwidth/80),
		  app;
	 
	for (i=0; i<MAXRAIN; i++) {
		TRainInfo &ri = rain_info[i];
		ri.x += fixmul((app = rain_speed[i])>>1,viewsin);
		ri.y += app;
		rain_speed[i] += app>>4  ;
		if (ri.x < -2l<<FIXSHIFT) ri.x = rb; else
			if (ri.x > rb) ri.x = -2l<<FIXSHIFT;
		if (ri.y > bb) {
			ri.y = -2l<<FIXSHIFT;
			ri.x = lshl16(random(viewwidth+4)-2);
			rain_speed[i] = spd-16384l+lshl8(random(2048));
		}
	}
}

void drawhand(int handmidx, int dx, int dy, int hotx, int hoty,
			  int jmph, int angle, char shade) {
#ifndef	__DEBUGMODE__
	int real_dx,real_dy,real_hotx,real_hoty;
	fixed presin = sintab[angle]>>2;
	real_dx = ((std::int32_t)dx*viewwidth)/320;
	real_dy = ((std::int32_t)dy*viewwidth)/320;
	real_hotx = ((std::int32_t)hotx*viewwidth)/320;
	real_hoty = ((std::int32_t)(dy+hoty)*viewwidth)/320;
	jmph = ((std::int32_t)jmph*halfwidth)/320;
	setvrect(viewleft,viewtop,viewwidth,viewheight);
	scalerscfig(viewleft+halfwidth-real_hotx+fixmul(jmph,presin>>1),
				viewbottom-real_hoty+(jmph>>2)+fixmul(jmph,presin)+1,
				dx,dy,real_dx,real_dy,shade,mm_recall(handmidx));
	 




#endif
}

void virtualview(fixed x, fixed y, int angle, int _noid) {
	viewx = x; viewy = y;
	viewangle = angle;
	viewcos = costab[viewangle];
	viewsin = sintab[viewangle];
	noid = _noid;
	int *angptr = angtab;
	fdfill(seenmap,0l,4096);  
	for (linex=viewwidth;linex--;) {
		if ((rayangle = angle + *(angptr++)) >= ANG360) rayangle -= ANG360;
		initray();
		raycast();
	}
	dobjlist.flush();
	 
	 
}

void drawmap(int cx, int cy, fixed x, fixed y, int ang, char *sm) {
	int i,j,k,l,curx,cury,bx,by,bdx,bdy,sqrdim;
	byte col;
	setvrect(cx-halfwidth,cy-halfheight,viewwidth,viewheight);
	 
	sqrdim = (viewwidth+32)>>6;
	bdx = (viewwidth+((sqrdim-1)<<1))/sqrdim;
	bdy = (viewheight+((sqrdim-1)<<1))/sqrdim;
	if ((bx=(lshr16(x)>>6)-(bdx>>1))<0) {bdx+=bx;bx=0;}
	if ((by=(lshr16(y)>>6)-(bdy>>1))<0) {bdy+=by;by=0;}
	if (bx+bdx>64) bdx=64-bx;
	if (by+bdy>64) bdy=64-by;
	 
	 
	curx = cx-((int)fixmul(sqrdim,x-lshl16(bx<<6))>>6);
	cury = cy-((int)fixmul(sqrdim,y-lshl16(by<<6))>>6);
	bx<<=6;
	ffillblock(cx-halfwidth,cy-halfheight,
			   viewwidth,viewheight,0x7f7f7f7fl);  
	if (GETFLAG(gameflags,GFL_TOTALMAP)) {
	for (i=bdx;i--;bx+=64,curx+=sqrdim)
	  for (j=bdy,k=by,l=cury;j--;k++,l+=sqrdim) {
		TMapInfo &mapinf = Map[bx+k];
		if (mapinf.stop)
			if (mapinf.shape==MSH_DOOR) {
				if (doormanager.doorpos(mapinf.data)==64) col=3; else
				if (doormanager.doortype[mapinf.data]==T_DOORY) col=132; else
				if (doormanager.doortype[mapinf.data]==T_DOORR) col=82; else
					col = 232;
			} else
			if (mapinf.type==MTP_DOOR1 || mapinf.type==MTP_DOOR2) {
				if (doormanager.doorpos(mapinf.data)==64) col=3;
													 else col=118;
			} else
			if (mapinf.data==T_SWITCHU) col=160;
			else col=118;
		else
		if (IDMap[bx+k]>=0) col=126;  
					   else col=3;
		putlcchar(curx,l,sqrdim,sqrdim,col,0,mapsquare);
	  }
	} else {
	for (i=bdx;i--;bx+=64,curx+=sqrdim)
	  for (j=bdy,k=by,l=cury;j--;k++,l+=sqrdim)
		if (sm[bx+k]) {
		  TMapInfo &mapinf = Map[bx+k];
		  if (mapinf.stop)
			if (mapinf.shape==MSH_DOOR) {
				if (doormanager.doorpos(mapinf.data)==64) col=6; else
				if (doormanager.doortype[mapinf.data]==T_DOORY) col=132; else
				if (doormanager.doortype[mapinf.data]==T_DOORR) col=82; else
					col = 232;
			} else
			if (mapinf.type==MTP_DOOR1 || mapinf.type==MTP_DOOR2) {
				if (doormanager.doorpos(mapinf.data)==64) col=6;
													 else col=118;
			} else
			if (mapinf.data==T_SWITCHU) col=160;
			else col=118;
		  else col=6;
			 
		  putlcchar(curx,l,sqrdim,sqrdim,col,0,mapsquare);
		}
	}
	putpix(cx+fixmul(1,costab[ang]),cy+fixmul(1,sintab[ang]),113);
	putpix(cx+fixmul(2,costab[ang]),cy+fixmul(2,sintab[ang]),112);
	putpix(cx,cy,114);
}

void updatemap(void far *sm) {
    if (!sm) return;
    const std::uint32_t *src = reinterpret_cast<const std::uint32_t *>(seenmap);
    std::uint32_t *dst = static_cast<std::uint32_t *>(sm);
    for (int i = 0; i < 1024; ++i)
        dst[i] |= src[i];
}

void init3DEngine() {
	map = (word *)Map;
	objmap = (word *)ObjMap;
	fwfill(mapsquare,0x01010101l,25);
	 
	setvbuff(vpage);	 
	calctables();
	 
	setviewsize(320,200-32);
	 
}
