#pragma once
#ifndef KAOS_3DENGINE_HPP
#define KAOS_3DENGINE_HPP
 






#define ANGLVL		8
#define MAXVIEWWIDTH	320
#define MAXVIEWHEIGHT	200 	 

#define	ANG0	   	0
#define	ANG30	   	( 30*ANGLVL)
#define	ANG45  		( 45*ANGLVL)
#define	ANG60	   	( 60*ANGLVL)
#define	ANG90	   	( 90*ANGLVL)
#define	ANG145	   	(145*ANGLVL)
#define	ANG180	   	(180*ANGLVL)
#define	ANG225	   	(225*ANGLVL)
#define	ANG270		(270*ANGLVL)
#define	ANG315	 	(315*ANGLVL)
#define	ANG360    	(360*ANGLVL)

typedef	struct { byte floor, ceil; } TFCInfo;

extern int angleadd(int,int);
extern int anglesub(int,int);

 


static inline int kaos_angidx(int ang)
{
	int v = ang % ANG360;
	if (v < 0) v += ANG360;
	return v;
}

extern fixed *costab, *sintab, *tantab;
extern int *angtab;
extern int viewwidth;
extern int viewangle;
extern fixed viewx, viewy, viewz;
extern fixed viewcos, viewsin;
 
extern int viewwidth, viewheight, halfwidth, halfheight;
extern int thunderdraw, thunderang, thunderz, underblack;

extern word *map;
extern word *objmap;
extern char seenmap[4096];
 
extern byte *floormap;
extern byte *ceilmap;
extern byte *vpage;
extern char raindraw;

extern void init3DEngine();
extern void setviewsize(int width, int height);
extern void drawview(int cx, int cy, fixed x, fixed y, fixed z,
					 int hang, int vang, int _noid,
					 char &backceildraw, char &backfloordraw, char fixshade);
extern void drawhand(int handmidx, int dx, int dy, int hotx, int hoty,
					 int jmph, int angle, char shade);
extern void virtualview(fixed x, fixed y, int angle, int _noid);
extern int lookray(fixed x1,fixed y1,fixed x2,fixed y2,word mask);
extern int fireray(fixed &x, fixed &y, int angle, int vangle,  
				   fixed &_dist, fixed &z, int _noid);
extern void anim_rain();
extern void reset_rain();
extern void drawrain();
extern void drawmap(int cx, int cy, fixed x, fixed y, int ang, char *sm);
extern void updatemap(void far *sm);
#endif  
