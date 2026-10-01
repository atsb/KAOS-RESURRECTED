 






#include "std.hpp"
#include "fixed.h"
#include "lgraph.hpp"
#include "mm4.hpp"
#include "events.hpp"
#include "kaos.hpp"
#include "objects.hpp"
#include "player.hpp"
#include "infobar.hpp"
#include "3dengine.hpp"

#include <stdlib.h> 	 

TFig BigBar, LitBar;

#define	C_VERDE 	100
#define	C_GIALLO	131
#define	C_GRIGIO	118

extern TFig RedKey, YellowKey;
extern TFig ArmFig[6];

InfoBar::InfoBar(int ownerid) {
	x = 0; y = 168;
	dx = 320; dy = 32;
	memidx = BigBar.memidx;
	owner = ownerid;
	animstate = 0;
	timeslice = 1;
	oldface=-1;
}

void InfoBar::drawface(int face, char transf) {
	putlfig(157,170,24,29,mm_recall(happyface[oldface=face].memidx));
	if (transf && !menustate) ftransfblock(157,170,24,29,VMEMPTR);
}

void InfoBar::repaint(byte what) {
	char repaint = GETEFLAG(what,IBR_REPAINT),
		notransf = (what==IBR_REPNOTRANSF) || menustate;
	Player *obj;
	void far *pbar;
	char validbar;
	if (owner<0 || (obj = (Player *)objectslist.get(owner)) == NULL) return;

	validbar = ((pbar = mm_recall(memidx)) != NULL);

	pbar = mm_recall(memidx);
	mm_lock(memidx);
	if (repaint)
		if (validbar) storelfig(x,y,dx,dy,pbar);
				 else ffillarea(y,dy,0x7f7f7f7fl);
	 











	if (GETFLAG(what,IBR_HEALTH)) {
		if (!GETFLAG(obj->pflags,PFL_GOD)) {
			if (!repaint) {
				if (validbar) {
					setvbuff(pbar);
					fcopyblock(92,174-168,92,174,50,11,vpage);  
					setvbuff(vpage);
				} else ffillblock(92,174,50,11,0x7f7f7f7fl);
			}
			int i;
			if ((i = obj->health)<0) i=0;
			 else if (i>100) i=100+((i-100)>>5);
			drawnum(124,174,1,2,bignumf,i);
			if (!notransf)
			  ftransfblock(92,174,50,11,VMEMPTR);
			if (!repaint) {
				animstate = 0;
				timeslice = 1;
			}
		}
	}
	if (GETFLAG(what,IBR_KEYS)) {
		if (!repaint) {
			if (validbar) {
				setvbuff(pbar);
				fcopyblock(5,172-168,284,172,34,24,vpage);  
				setvbuff(vpage);
			} else ffillblock(284,172,34,24,0x7f7f7f7fl);
		}
		if (GETFLAG(obj->keys,LCKT_L1))
			putlfig( 5,172,YellowKey.dx,YellowKey.dy,mm_recall(YellowKey.memidx));
		if (GETFLAG(obj->keys,LCKT_L2))
			putlfig( 23,172,RedKey.dx,RedKey.dy,mm_recall(RedKey.memidx));
		if (!notransf)
		  ftransfblock(5,172,34,24,VMEMPTR);
	}
	if (GETFLAG(what,IBR_ARMS)) {
		char col;
		if (!repaint) {
			if (validbar) {
				setvbuff(pbar);
				fcopyblock(204,171-168,204,171,112,28,vpage);  
				setvbuff(vpage);
			} else ffillblock(204,171,112,28,0x7f7f7f7fl);
		}
		if (GETFLAG(obj->armflag,AFL_ASCIA)) {
			if (obj->curarm == ARM_ASCIA) col = C_GIALLO;
									 else col = C_VERDE;
			 
			putlchar(207,172,ArmFig[ARM_ASCIA].dx,ArmFig[ARM_ASCIA].dy,
					col,0,mm_recall(ArmFig[ARM_ASCIA].memidx));
		}
		if (GETFLAG(obj->armflag,AFL_PISTOLA)) {
			if (obj->curarm == ARM_PISTOLA) col = C_GIALLO; else
			  if (obj->Armbull[ARM_PISTOLA-1]) col = C_VERDE; else
				col = C_GRIGIO;
			 
			putlchar(252,171,ArmFig[ARM_PISTOLA].dx,ArmFig[ARM_PISTOLA].dy,
					col,0,mm_recall(ArmFig[ARM_PISTOLA].memidx));
		}
		if (GETFLAG(obj->armflag,AFL_FUCILE)) {
			if (obj->curarm == ARM_FUCILE) col = C_GIALLO; else
			  if (obj->Armbull[ARM_FUCILE-1]) col = C_VERDE; else
				col = C_GRIGIO;
			 
			putlchar(283,171,ArmFig[ARM_FUCILE].dx,ArmFig[ARM_FUCILE].dy,
					col,0,mm_recall(ArmFig[ARM_FUCILE].memidx));
		}
		if (GETFLAG(obj->armflag,AFL_MITRA)) {
			if (obj->curarm == ARM_MITRA) col = C_GIALLO; else
			  if (obj->Armbull[ARM_MITRA-1]) col = C_VERDE; else
				col = C_GRIGIO;
			putlchar(204,186,ArmFig[ARM_MITRA].dx,ArmFig[ARM_MITRA].dy,
					col,0,mm_recall(ArmFig[ARM_MITRA].memidx));
		}
		if (GETFLAG(obj->armflag,AFL_BAZOOKA)) {
			if (obj->curarm == ARM_BAZOOKA) col = C_GIALLO; else
			  if (obj->Armbull[ARM_BAZOOKA-1]) col = C_VERDE; else
				col = C_GRIGIO;
			 
			putlchar(245,185,ArmFig[ARM_BAZOOKA].dx,ArmFig[ARM_BAZOOKA].dy,
					col,0,mm_recall(ArmFig[ARM_BAZOOKA].memidx));
		}
		if (GETFLAG(obj->armflag,AFL_BOMB)) {
			if (obj->curarm == ARM_BOMB) col = C_GIALLO; else
			  if (obj->Armbull[ARM_BOMB-1]) col = C_VERDE; else
				col = C_GRIGIO;
			 
			putlchar(297,186,ArmFig[ARM_BOMB].dx,ArmFig[ARM_BOMB].dy,
					col,0,mm_recall(ArmFig[ARM_BOMB].memidx));
		}
		if (!notransf)
		  ftransfblock(204,171,112,28,VMEMPTR);

		 
		if (!repaint) {
			if (validbar) {
				setvbuff(pbar);
				fcopyblock(46,174-168,46,174,50,11,vpage);  
				setvbuff(vpage);
			} else ffillblock(46,174,50,11,0x7f7f7f7fl);
		}
		if (obj->curarm)
			drawnum(86,174,1,2,bignumf,obj->Armbull[obj->curarm-1]);
		if (!notransf)
			ftransfblock(46,174,50,11,VMEMPTR);
		 










	}
	if (repaint) {
		 
		if (oldface>0) drawface(oldface,!notransf);
				  else drawface(0,!notransf);
		 
		if (!notransf)
		  ftransfblock(x,y,dx,dy,VMEMPTR);
	}
	mm_unlock(memidx);
}

void InfoBar::animate() {
	Player *obj;
	int i, newface;
	if (animstate == 5) return;
	if (owner<0 || (obj = (Player *)objectslist.get(owner)) == NULL) return;
	if (GETFLAG(obj->pflags,PFL_GOD) || obj->health>100) {
		if (animstate != 4) {
			animstate=4;
			drawface(13,1);
		}
	} else
	if (animstate==4) {
		animstate=0;
		timeslice=1;
	} else
	if (obj->health<=0) {	 
		animstate=5;
		drawface(3,1);
	}
	switch (animstate) {
		case 0:
			if (--timeslice > 0) return;
			timeslice = 14+random(18);
			newface = obj->health/33;
			if (newface>2) newface=0; else newface=2-newface;
			switch (random(7)) {
				case 0: break;
				case 1:
				case 2:
				case 3: newface+=4; break;
				case 4:
				case 5:
				case 6: newface+=10; break;
			}
			if (newface!=oldface) drawface(newface,1);
			break;
		case 1:
			if (timeslice==28) {
				i = obj->health/33;
				if (i>2) i=0; else i=2-i;
				drawface(7+i,1);
				timeslice--;
			} else {
				if (!(--timeslice)) {
					animstate=0;
					timeslice=1;
				}
			  }
			break;
		case 2:
			if (timeslice==36) {
				i = obj->health/33;
				if (i>2) i=0; else i=2-i;
				drawface(14+i,1);
				timeslice--;
			} else {
				if (!(--timeslice)) {
					animstate=0;
					timeslice=1;
				}
			  }
			break;
	}
}

void InfoBar::handle_event(TEvent &event) {
	switch(event.what) {
		case EV_KILL:
		case EV_DEAD:
			if (event.source==owner) owner=-1;
			break;
		case EV_UPDATEBAR:
			if (event.source==owner) {
				repaint(event.data.msg.x);
				if (GETFLAG(event.data.msg.x,IBR_FACEHIT)) {
					animstate=1;
					timeslice=28;
				} else
				if (GETFLAG(event.data.msg.x,IBR_SMILE)) {
					animstate=2;
					timeslice=36;
				}
			}
			break;
	}
}

LittleInfoBar::LittleInfoBar(int ownerid,int _x,int _y)
			  :InfoBar(ownerid) {
	x = _x; y = _y;
	dx = LitBar.dx; dy = LitBar.dy;
	memidx = LitBar.memidx;
}

void LittleInfoBar::repaint(byte what) {
	char repaint = GETEFLAG(what,IBR_REPAINT),
		notransf = (what==IBR_REPNOTRANSF) || menustate;
	Player *obj;
	void far *pbar;
	if (owner<0 || (obj = (Player *)objectslist.get(owner)) == NULL) return;
	if ((pbar = mm_recall(memidx)) != NULL) storelfig(x,y,dx,dy,pbar);
									   else ffillblock(x,y,dx,dy,0x7f7f7f7fl);

	if (GETFLAG(what,IBR_HEALTH)) {
		 
			int i;
			if ((i = obj->health)<0) i=0;
			 else if (i>100) i=100+((i-100)>>5);
			drawnum(x+82,y+2,1,2,normnumf,i);
			if (!notransf)
				ftransfblock(x+82-22,y+2,22,9,VMEMPTR);
		 
	}
	 
	if (GETFLAG(what,IBR_ARMS)) {
		char col;
		int dx,dy,arm = obj->curarm;

		if (arm) {
			if (obj->Armbull[arm-1]) col=C_GIALLO;
								else col=C_GRIGIO;
			drawnum(x+122,y+2,1,2,normnumf,obj->Armbull[arm-1]);
		} else col = C_GIALLO;
		dx = (41-ArmFig[arm].dx)>>1;
		dy = (13-ArmFig[arm].dy)>>1;
		putlchar(x+11+dx,y+3+dy,ArmFig[arm].dx,ArmFig[arm].dy,
				col,0,mm_recall(ArmFig[arm].memidx));
		if (!notransf) {
			ftransfblock(x+11,y+3,42,13,VMEMPTR);
			ftransfblock(x+122-22,y+2,22,9,VMEMPTR);
		}
	}
	if (GETFLAG(what,IBR_PLKILLED)) {
		drawnum(x+145,y+11,1,2,litnumf,obj->plkilled);
		if (!notransf)
			ftransfblock(x+145-12,y+11,12,5,VMEMPTR);
	}
	if (repaint && !notransf)
		ftransfblock(x,y,dx,dy,VMEMPTR);
}

void LittleInfoBar::animate() {
	 
}