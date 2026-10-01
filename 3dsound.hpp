#pragma once
#include "fixed.h"
 






#define MAXSOUNDS 	16
#define	MAXSNDDATA	53
#define	SYSOWN 		-1

 
#define	SFL_CONTINUE	0x01
#define	SFL_3D     		0x02
#define SFL_RAY			0x04
#define	SFL_FOLLOW		0x08
#define	SFL_AUTOSTOP	0x10
#define	SFL_OVERSOUND	0x20   
#define	SFL_FADE		0x40

#define	SFL_FIXED		0x00
#define	SFL_BASIC       SFL_3D
#define SFL_MULTI		(SFL_3D|SFL_RAY|SFL_AUTOSTOP)
#define	SFL_NORMAL		(SFL_3D|SFL_RAY|SFL_AUTOSTOP|SFL_OVERSOUND)
#define	SFL_AUTO		(SFL_3D|SFL_RAY|SFL_FOLLOW|SFL_AUTOSTOP|SFL_OVERSOUND)

#define	SM_MAXVOL		40  
#define	SM_NORMVOL		32  
#define	SM_LOWVOL		24  
#define SM_VOLSHIFT		5

extern int sm_soundnum;
extern char sm_lrinverted;
extern char sm_soundok;
extern int sm_soundvol;

extern void initsounds();
extern void donesounds();
extern void soundmanager(fixed cx, fixed cy, int angle);
extern void loadsound(int sndnum, const char *wavname);
extern int play3Dsound(fixed x, fixed y, int sndnum, byte vol,
					   int owner, byte flags);
extern int playsound(int sndnum, byte lvol, byte rvol,
					 char desync, int owner, byte flags);
extern void ownerdead(int id);
extern char stopsound(int id);
extern void stopallsounds();
