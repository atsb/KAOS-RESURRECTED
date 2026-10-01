 






 

#include "std.hpp"
#include "fastmem.hpp"
#include "fixed.h"
#include "events.hpp"
#include "mm4.hpp"	 
#include "crcio.hpp"
#include "kaos.hpp"
#include "objects.hpp"
#include "objects2.hpp"
#include "3dengine.hpp"
#include "3dsound.hpp"
#include "doors.hpp"
#include "messages.hpp"
#include <stdlib.h>			 
#include <cstdio>

 

 
char Object::doblood(int angle) {
	int oppangle;
	fixed x,y;

	if (!(figstart>=F_PLAYER && figstart<=F_MOUSE)) return 0;
	oppangle = angleadd(angle,ANG180);
	x = mover->x + fixmul(costab[oppangle],FIXONE<<mover->shdim);
	y = mover->y + fixmul(sintab[oppangle],FIXONE<<mover->shdim);
	delete mm_reserve(sizeof(FireBoom));
	objectslist.put(new FireBoom(x,y,mover->z,oppangle,BOOM_BLOOD));
	if (figstart<F_MOUSE) {
		fixed dist, z = mover->z;
		 
		int id1 = fireray(x,y,angle,ANG0,dist,z,mover->id);
		if (id1==-1 && dist<(128l<<FIXSHIFT))
			eventmanager.sendshort(EV_CHGTEXTURE,0,0,xytoidx(x,y),0);
	}
	return 1;
}

Ossa::Ossa(fixed px, fixed py, int type)
	 :Actor(px,py,type,1,MVT_0)
{
	object_type=OBJT_OSSA;
	if (type == F_OSSALIT) {
		SETFLAG(oflags,OFL_TAKEABLE);
		mover->flags = OMF_VISIBLE;
	}
	animtrig=0;
}

Ossa::Ossa(int handle)
	 :Actor(handle)	{
	animtrig=0;
}

void Ossa::animate() {
	if (--animtrig<=0) makeObject();
}

void Ossa::launch(int angle, int strong) {
	if (!animtrig) {
		animtrig = 2;
		makeActor();
		delete mm_reserve(sizeof(FlOssa));
		if (!GETFLAG(gameflags,GFL_NORANDOM))
			angle = anglesub(angleadd(angle,ANG30>>1),random(ANG30));
		objectslist.put(new FlOssa(mover->x,mover->y,angle,strong));
		play3Dsound(mover->x,mover->y,SND_OSSA,SM_NORMVOL,SYSOWN,SFL_NORMAL);
	} else animtrig++;
}

void Ossa::handle_event(TEvent &event) {
	if (event.what != EV_GOT) Object::handle_event(event);
	switch (event.what) {
		case EV_HIT:
		case EV_GOT:
			launch(event.data.msg.x,event.data.msg.y);
			 
			break;
	}
}

FlOssa::FlOssa(fixed px, fixed py, int angle, int _speed)
	   :Actor(px,py,F_TESCHIO+random(2),1,MVT_1)
{
	object_type=OBJT_FLOSSA;
	if (_speed>24) speed=24; else speed=_speed;
	mover->angle = angle;
	mover->shdim = 3;
	mover->flags = OMF_VISIBLE;
	bang = 0;
}

FlOssa::FlOssa(int handle)
	   :Actor(handle) {
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&bang,sizeof(bang));
}

void FlOssa::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&bang,sizeof(bang));
}

void FlOssa::animate() {
	Mover &mv = *mover;
	if (speed>0) {
		remove();
		fixed nx = mv.x + (costab[mv.angle]*(speed>>1)),
			  ny = mv.y + (sintab[mv.angle]*(speed>>1));
		if (!mv.canmove(nx,mv.y)) {
			if (mv.angle<=ANG180) mv.angle=ANG180-mv.angle;
							 else mv.angle=ANG180+ANG360-mv.angle;
		} else mv.x = nx;
		if (!mv.canmove(mv.x,ny)) {
			if (mv.angle!=ANG0) mv.angle=ANG360-mv.angle;
		} else mv.y = ny;
		bang = angleadd(bang,(12-(speed>>1))<<5);
		mv.z = LABS(fixmul(lshl16(speed),sintab[bang]));
	 




		place();
		speed--;
	} else
		if ((--speed)<=-20)
			if (!eventmanager.sendcom(EV_KILL,mv.id)) speed=-19;
}

ObjMover::ObjMover(int _objlinkID, int _angle, int _speed)
		 :Actor(0,0,F_TESCHIO,1,MVT_0)	 
{
	Object *objlink;

	object_type=OBJT_OBJMOVER;
	 



	objlink = objectslist.get(objlinkID = _objlinkID);
	if (_speed>24) speed=24; else speed=_speed;
	mover->angle = _angle;
	mover->x = objlink->mover->x;
	mover->y = objlink->mover->y;
	mover->z = objlink->mover->z;
	mover->shdim = 3;
	mover->flags = 0;	 
	bounceang = 0;
}

ObjMover::ObjMover(int handle)
		 :Actor(handle) {
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&bounceang,sizeof(bounceang));
	CRC_read(handle,&objlinkID,sizeof(objlinkID));
}

void ObjMover::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&bounceang,sizeof(bounceang));
	CRC_write(handle,&objlinkID,sizeof(objlinkID));
}

void ObjMover::animate() {
	Object *objlink = objectslist.get(objlinkID);
	Mover &mv = *mover;

	if (objlink==NULL) {
		eventmanager.sendcom(EV_KILL,mv.id);
		return;
	}
	if (speed>0) {
		remove();
		fixed nx = mv.x + (costab[mv.angle]*(speed>>1)),
			  ny = mv.y + (sintab[mv.angle]*(speed>>1));
		if (!mv.canmove(nx,mv.y)) {
			if (mv.angle<=ANG180) mv.angle=ANG180-mv.angle;
							 else mv.angle=ANG180+ANG360-mv.angle;
		} else mv.x = nx;
		if (!mv.canmove(mv.x,ny)) {
			if (mv.angle!=ANG0) mv.angle=ANG360-mv.angle;
		} else mv.y = ny;
		bounceang = angleadd(bounceang,(12-(speed>>1))<<4);
		objlink->mover->z = mover->z+LABS(fixmul(lshl16(speed<<1),sintab[bounceang]));
		objlink->remove();
		objlink->mover->x = mv.x;
		objlink->mover->y = mv.y;
		objlink->place();
		place();
		speed--;
	} else {
		objlink->mover->z = mv.z;
		eventmanager.sendcom(EV_KILL,mv.id);
	  }
}

void ObjMover::handle_event(TEvent &event) {
	Object::handle_event(event);
	switch (event.what) {
		case EV_KILL:
			if (event.source == objlinkID) {
				eventmanager.sendcom(EV_KILL,mover->id);
				objlinkID = -1;	 
			}
	}
}

Bonus::Bonus(fixed px, fixed py)
	  :LightActor(px,py,F_BONUS,1,2,3,MVT_0) {
	object_type=OBJT_BONUS;
	mover->z = 24l<<FIXSHIFT;
	CLRFLAG(mover->flags,OMF_UNWALKABLE|OMF_HITABLE);
	SETFLAG(oflags,OFL_TAKEABLE);
	animtrig = random(16);
	animang = random(ANG360);
	lightz = mover->z;
}

Bonus::Bonus(int handle)
	  :LightActor(handle) {
	animtrig = random(16);
	animang = random(ANG360);
}

void Bonus::animate() {
	if (++animtrig >= 16) {
		animtrig = 0;
		if (++animcount > 1) animcount = 0;
	}
	animang = angleadd(animang,ANGLVL<<2);
	lightz = mover->z = (24l<<FIXSHIFT)+(sintab[animang]<<2);
}

int Bonus::getlightfig() {
	return F_CIRCLIGHT;
}

AntiKaos::AntiKaos(fixed px, fixed py)	 
		 :LightActor(px,py,F_ANTIKAOS,-16,1,3,MVT_0) {
	object_type = OBJT_ANTIKAOS;
	lightz = mover->z = 40l<<FIXSHIFT;
	CLRFLAG(mover->flags,OMF_UNWALKABLE|OMF_HITABLE);
	SETFLAG(oflags,OFL_TAKEABLE);
	animtrig = random(ANG360);
	animang = random(ANG360);
}

AntiKaos::AntiKaos(int handle)
		 :LightActor(handle) {
	animtrig = random(ANG360 & 0x7ff8);
	animang = random(ANG360 & 0x7ffc);
}

void AntiKaos::animate() {
	if (Kaos_count == -1) {
		eventmanager.sendcom(EV_KILL,mover->id);
		eventmanager.sendcom(EV_MESSAGE,MSG_KAOS);
		delete mm_reserve(sizeof(FireBoom));
		objectslist.put(new FireBoom(mover->x,mover->y,mover->z,
						mover->angle,BOOM_SMOG));
		play3Dsound(mover->x,mover->y,SND_MISSILE,SM_NORMVOL,SYSOWN,SFL_NORMAL);
	} else {
		mover->angle = animang = angleadd(animang,ANGLVL<<2);
		lightz = mover->z = (40l<<FIXSHIFT)+(sintab[animang]<<1);
		animtrig = angleadd(animtrig,ANGLVL<<3);
		lightpow = 4+lshr16(sintab[animtrig]<<3);
	  }
}

int AntiKaos::getlightfig() {
	return F_CIRCLIGHT;
}

void AntiKaos::handle_event(TEvent &event) {
	 
	Object::handle_event(event);
	switch (event.what) {
		case EV_GOT:	 
			eventmanager.sendcom(EV_ANTIKAOS,mover->id);
			break;
	}
}

Enemy::Enemy(fixed px, fixed py, byte type)
	  :Actor(px,py,F_SPIDER,-8,MVT_2)
{
	animtrig = 0;
	walkspeed = 2;
	speed = 0;
	mover->shdim = 4;  
	animstate = 2;
	hitangle=0;
	switch (type) {
		case F_BUB:
			changefig(F_BUB,-8);
			mover->z = 32l<<16;
			health = 80;
			walkspeed = 1;
			break;
		case F_FACE:
			changefig(F_FACE,-8);
			mover->z = 32l<<16;
			health = 20;
			mover->shdim = 4;
			break;
	}
	posneg = 1;
	firstfig = figstart;
}

void Enemy::animate() {
	if (viewstate) return;
	char dir = 0;
	switch (animstate) {
		case 2:	 
			if (++animtrig > 15) animtrig = 0;
			animcount = animtrig >> 3;
		case 0:	 
			dir = static_cast<char>(mover->angle/ANG45);  
			if (!had_seen()) return;
			remove();
			if (!random(50)) dir = (dir+random(3)-1) & 0x7;
			if (have_seen() && random(100)==37) {
				switch (figstart) {
				case F_BUB:
					play3Dsound(0l,0l,SND_BUB,SM_NORMVOL,mover->id,SFL_AUTO);
					break;
				}
			}
			if (random(17)==13) posneg = -posneg;
			for (int i=0;i<8;i++) {
				if (mover->move(mover->angle,walkspeed)) i=8;
				else {
					dir=(dir+8+posneg) & 0x07;
					mover->angle = (int)dir*ANG45;
				}
			}
			place();
			break;
		case 1:	 
			if (health>0) {
				if (speed) {
					remove();
					mover->fluidmove(hitangle,lshl16(speed--));
					place();
				} else {
					animtrig=0;
					animstate=2;
					animcount=0;
					changefig(firstfig,-8);
				  }
			} else {
				if (speed) {
						remove();
						mover->fluidmove(hitangle,lshl16(speed--));
						place();
				}
				if (animtrig) {
					mover->flags = OMF_VISIBLE;
					if (!(--animtrig & 3)) animcount++;
				} else if (!speed) {
					selflight=0;
					makeObject();
					ownerdead(mover->id);
				  }
			  }
			break;
	}
}

void Enemy::handle_event(TEvent &event) {
	int dead;
	Object::handle_event(event);
	if (health && (event.what == EV_HIT ||
		(event.what == EV_SCODE && event.source == SC_MNStrg))) {
		if (event.what == EV_SCODE) {
			health=0; speed=0;
			dead=1;
		} else {
			if ((health-=event.data.msg.y) <=0) {
				health=0;
				eventmanager.senddirect(EV_ADDSTAT,mover->id,event.source,
										STT_KILL,1);
				dead=1;
			} else dead=0;
			switch (figstart) {
				case F_BUB:
					if ((speed+=event.data.msg.y>>1) > 12) speed = 12;
					break;
				case F_FACE:
					if ((speed+=event.data.msg.y<<1) > 18) speed = 18;
					break;
			}
			hitangle=event.data.msg.x;
		  }
		switch (firstfig) {
			case F_FACE:
				changefig(firstfig+10+(dead<<1),2-dead);
				animtrig=19-(dead<<2);
				break;
			case F_BUB:
				changefig(firstfig+12+(dead<<1),2-dead);
				animtrig=23-(dead<<2);
				break;
		}
		animcount=0;
		animstate=1;
	}
	 
}

Slimer::Slimer(fixed px, fixed py)
	   :Actor(px,py,F_SLIMER,-8,MVT_2) {
	object_type=OBJT_SLIMER;
	numenemies++;	 
	animstate=2;
	animtrig=0;
	mover->z = 38l<<FIXSHIFT;
	health = 28;
	mover->shdim = 5;
	scanangle = ANG0;
	objlink = -1;
	mysound = -1;
	floatangle = ANG0;
	cannoncharge = 0;
	speed = 0;
	posneg = 1;
	sndcount = 0;
	turning = 0; turnnum = 0; turnable=0;
}

Slimer::Slimer(int handle)
	   :Actor(handle) {
	numenemies++;	 
	CRC_read(handle,&animstate,sizeof(animstate));
	CRC_read(handle,&animtrig,sizeof(animtrig));
	CRC_read(handle,&posneg,sizeof(posneg));
	CRC_read(handle,&objlink,sizeof(objlink));
	CRC_read(handle,&scanangle,sizeof(scanangle));
	CRC_read(handle,&cannoncharge,sizeof(cannoncharge));
	CRC_read(handle,&turning,sizeof(turning));
	CRC_read(handle,&turnnum,sizeof(turnnum));
	CRC_read(handle,&turnable,sizeof(turnable));
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&hitangle,sizeof(hitangle));
	sndcount=0; mysound=-1; floatangle=ANG0;
}

void Slimer::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&animstate,sizeof(animstate));
	CRC_write(handle,&animtrig,sizeof(animtrig));
	CRC_write(handle,&posneg,sizeof(posneg));
	CRC_write(handle,&objlink,sizeof(objlink));
	CRC_write(handle,&scanangle,sizeof(scanangle));
	CRC_write(handle,&cannoncharge,sizeof(cannoncharge));
	CRC_write(handle,&turning,sizeof(turning));
	CRC_write(handle,&turnnum,sizeof(turnnum));
	CRC_write(handle,&turnable,sizeof(turnable));
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&hitangle,sizeof(hitangle));
}

void Slimer::animate() {
	fixed cx, cy, dist, alt;
	Object *object;
	int i,j;
	char oldseen;

	if (viewstate) return;
	switch (animstate) {
		case 0:	 
		case 3:  
			 
			if (cannoncharge) cannoncharge--;

			cx=mover->x; cy=mover->y;
			i = fireray(cx,cy,scanangle,0,dist,alt,mover->id);
			oldseen = objseen;
			objseen = 0;
			if (i>=0) {
				switch ((object = objectslist.get(i))->figstart) {
				case F_SLIMER:
					 
					if (objlink!=i) {
						scanangle = angleadd(scanangle,ANG30>>1);
						break;
					}
				case F_SPIDER:
					 
					if (objlink>=0 && animstate==3 &&
						objectslist.get(objlink)->get_type() == OBJT_PLAYER) {
						scanangle = angleadd(scanangle,ANG30>>1);
						break;
					}
				case F_PLAYER:		 
				case F_PLAYER+32:	 
					objlink = i;
					objseen = 1;
					mover->angle = scanangle;
					turnnum=0;turning=0;
					if (dist >= 200l<<FIXSHIFT) {	 
						if (!cannoncharge) {
							animstate+=4;
							animtrig=0;
						} else
							scanangle = anglesub(scanangle,ANG30>>1);
					} else
					if (dist <= 80l<<FIXSHIFT) {	 
						animstate+=5;
						animtrig=0;
					} else
						scanangle = anglesub(scanangle,ANG30>>1);
					break;
				default: scanangle = angleadd(scanangle,ANG30>>1);
				}
			} else scanangle = angleadd(scanangle,ANG30>>1);

			 
			if (oldseen && !objseen) {
				turnable=0;
				turning=0;
				turnnum=0;
				posneg = !posneg;
				 
			}

			 
			remove();
			if (objlink>=0) {
				object = objectslist.get(objlink);
				if (object!=NULL && ((Actor *)object)->health>0) {
					if (objseen) {
						 
						for (i=3;i>=0;i--)
							if ((j = mover->move(mover->angle,i))>0) break;
						 
						if (j<2) mover->move(anglesub(mover->angle,ANG60),1);
					} else {
						 
						if (turnable) {
							 
														 
							mover->move(mover->angle,2);
							turnable--;

							 
							if (turnnum && turnable<5) {
								if (posneg) mover->move(anglesub(mover->angle,ANG60),1);
										else mover->move(angleadd(mover->angle,ANG60),1);
							}

						} else
						if (turning) {	 
							 
							 
							if (turning<0 ^ posneg)
								mover->angle = angleadd(mover->angle,ANG45>>3);
							else
								mover->angle = anglesub(mover->angle,ANG45>>3);
							if (!(mover->angle % ANG90)) {
								turning=0;
								 
								turnable=8;
							}
							mover->fluidmove(mover->angle,FIXONE>>1);
						} else
						if (turnnum) {
							turnnum--; turning--;
							 



							if (posneg) i = anglesub(mover->angle,ANG90);
								   else i = angleadd(mover->angle,ANG90);
							for (j=0; j<3; j++) {
							  if (mover->move(i,2)<2) {
								if (posneg) i = angleadd(i,ANG90);
									   else i = anglesub(i,ANG90);
								turnnum++; turning++;
							  } else break;
							}
							if (turnnum>=16) {
								turnnum=0;
								posneg=!posneg;
							}
						} else {
							int dx = (lshr16(object->mover->x - mover->x)) >> 4,
								dy = (lshr16(object->mover->y - mover->y)) >> 4,
								dir;

							if (animstate==0 && ABS(dx)+ABS(dy)>32) objlink=-1;
							if (dx>1) {
								if (dy>0) dir=1; else
								if (dy<0) dir=7; else
									dir=0;
							} else
							if (dx<1) {
								if (dy>0) dir=3; else
								if (dy<0) dir=5; else
									dir=4;
							} else {
								if (dy>0) dir=2; else
								if (dy<0) dir=6; else
									dir=-1;
							}
							if (dir>=0) {
								mover->angle = dir*ANG45;
								if (!mover->move(mover->angle,2)) {
									turnnum++;
									turning++;
								}
							} else objlink=-1;
						  }
					  }
				} else objlink = -1;
			} else {
				 
				 
				 
				animstate=0;
				if (!mover->move(mover->angle,1)) {
					if (posneg) mover->angle = anglesub(mover->angle,ANG45>>3);
						   else mover->angle = angleadd(mover->angle,ANG45>>3);
					 
					 
					 
					 
					 
				}
			  }
			place();
			 
			if (++animtrig > 31) animtrig = 0;
			animcount = animtrig >> 4;
			 
			if (!sndcount) {
			  if (mysound<0) {
				if (objlink>=0) {  
				  if (nearPlayer(192l<<FIXSHIFT)) {
					mysound = play3Dsound(0l,0l,SND_SLIME2,SM_NORMVOL,mover->id,SFL_AUTO);
					sndcount = 108;
				  }
				} else
				if (have_seen() || nearPlayer(140l<<FIXSHIFT)) {
					mysound = play3Dsound(0l,0l,SND_SLIME1,SM_NORMVOL,mover->id,SFL_AUTO);
					sndcount = 90;
					reset();
				}
			  }
			} else sndcount--;
			floatangle = angleadd(floatangle,ANGLVL<<3);
			mover->z = (38l<<FIXSHIFT)+(sintab[floatangle]<<2);
			break;
		case 1:
			if (speed) {
				remove();
				mover->fluidmove(hitangle,lshl16(speed--));
				place();
			} else {
				if (health) {
					animtrig=0;
					animstate=3;  
					animcount=0;
					changefig(F_SLIMER,-8);
				} else
				if (!animtrig) {
					selflight=0;
					makeObject();
					eventmanager.sendcom(EV_DEAD,mover->id);
					 
				}
			  }
			if (!health && animtrig) {
				if (GETFLAG(mover->flags,OMF_UNWALKABLE)) {
					mover->z = 38l<<FIXSHIFT;
					remove();
					mover->flags = OMF_VISIBLE;
					place();
					SETFLAG(oflags,OFL_TAKEABLE);
				}
				if (!(--animtrig & 3)) animcount++;
				if (animtrig==4)
					play3Dsound(mover->x,mover->y,SND_SLIMDOWN,SM_NORMVOL,
								SYSOWN,SFL_NORMAL);
			}
			break;
		case 4:  
		case 7:
			switch (animtrig++) {
				case 0: break;
				case 6:
					changefig(F_SLIMER+14,2);
					animcount=0;
					break;
				case 12:
					 
					cx=mover->x; cy=mover->y;
					fireray(cx,cy,scanangle,0,dist,alt,mover->id);

					delete mm_reserve(sizeof(Fireball));
					object = new Fireball(mover->x,mover->y,scanangle,F_SLIMFIRE,
								  mover->z,alt,dist,mover->id);
					mysound = play3Dsound(mover->x,mover->y,SND_MISSILE,SM_NORMVOL,
								  mover->id,SFL_NORMAL);
					objectslist.put(object);
					 
					cannoncharge = 48;
					break;
				case 18:
					changefig(F_SLIMER,-8);
					animcount=0;
					animtrig=0;
					animstate-=4;
					break;
			}
			break;
		case 5:  
		case 8:
			remove();mover->move(mover->angle,0);place();
			switch (animtrig++) {
				case 0: break;
				case 6:
					changefig(F_SLIMER+10,2);
					animcount=0;
					break;
				case 12:
					animcount=1;
					mysound=play3Dsound(mover->x,mover->y,SND_SLIMATT,SM_NORMVOL,
										mover->id,SFL_NORMAL);
					break;
				case 16:
					 
					cx=mover->x; cy=mover->y;
					i = fireray(cx,cy,scanangle,0,dist,alt,mover->id);
					if (i>=0 && dist<80l<<FIXSHIFT)
						 
						eventmanager.senddirect(EV_HIT,mover->id,i,mover->angle,6);
					 
					break;
				case 20: changefig(F_SLIMER,-8); break;
				case 21: animstate-=5; animtrig=0; break;
			}
			break;
		case 2:
			if (had_seen()) {
				animstate=0;
				reset();	 
			}
			break;
	}

}

void Slimer::handle_event(TEvent &event) {
	int dead;
	Object::handle_event(event);
	if (!health) return;
	if ((event.what == EV_HIT || event.what == EV_POISON ||
		(event.what == EV_SCODE && event.source == SC_MNStrg))) {
		if (event.what == EV_SCODE) {
			health=0; speed=0;
			dead=1;
		} else {
			if (event.source != mover->id) objlink = event.source;
			 
			 
			if ((health-=event.data.msg.y) <=0) {
				health=0;
				eventmanager.senddirect(EV_ADDSTAT,mover->id,event.source,
										STT_KILL,1);
				play3Dsound(mover->x,mover->y,SND_SLIMDEAD,SM_NORMVOL,
							SYSOWN,SFL_NORMAL);
				dead=1;
			} else {
				mysound=play3Dsound(0l,0l,SND_SLIMHIT,SM_NORMVOL,
									mover->id,SFL_AUTO);
				dead=0;
			  }
			if ((speed += event.data.msg.y<<1) > 18) speed = 18;
			hitangle=event.data.msg.x;
		  }
		changefig(F_SLIMER+14+(dead<<1),2-dead);
		animtrig=23-(dead<<2);
		animcount=0;
		animstate=1;
	} else
	if (mysound>=0 && event.what == EV_ENDSOUND && event.source==mysound)
		mysound=-1;
}

Spider::Spider(fixed px, fixed py)
	   :Actor(px,py,F_SPIDER,-8,MVT_2) {
	object_type=OBJT_SPIDER;
	numenemies++;	 
	animstate=2;
	animtrig=0;
	mover->z = 22l<<FIXSHIFT;
	mover->shdim=5;  
	health = 10;
	speed = 0;
	hitangle=0;
	scanangle = ANG0;
	objlink = -1;
	mysound=-1;
	cannoncharge=0;
}

Spider::Spider(int handle)
	   :Actor(handle) {
	numenemies++;	 
	CRC_read(handle,&animstate,sizeof(animstate));
	CRC_read(handle,&animtrig,sizeof(animtrig));
	CRC_read(handle,&objlink,sizeof(objlink));
	CRC_read(handle,&scanangle,sizeof(scanangle));
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&hitangle,sizeof(hitangle));
	CRC_read(handle,&cannoncharge,sizeof(cannoncharge));
	mysound=-1;
}

void Spider::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&animstate,sizeof(animstate));
	CRC_write(handle,&animtrig,sizeof(animtrig));
	CRC_write(handle,&objlink,sizeof(objlink));
	CRC_write(handle,&scanangle,sizeof(scanangle));
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&hitangle,sizeof(hitangle));
	CRC_write(handle,&cannoncharge,sizeof(cannoncharge));
}

void Spider::animate() {
	fixed cx, cy, dist, alt;
	Object *object;
	int i;
	if (viewstate) return;
	switch (animstate) {
		case 3:  
		case 0:	 
			 
			if (cannoncharge) cannoncharge--;
			if (animtrig & 1) {
				 



				scanangle = angleadd(scanangle,ANG45>>1);
				cx=mover->x; cy=mover->y;
				i = fireray(cx,cy,scanangle,0,dist,alt,mover->id);
				if (i>=0)
				 switch ((object = objectslist.get(i))->figstart) {
				   case F_SLIMER:
						if (ABS(object->mover->angle-angleadd(scanangle,ANG180))<=ANG45) {
							if (GETFLAG(gameflags,GFL_NORANDOM) || random(2))
								mover->angle = angleadd(scanangle,ANG90);
							else
								mover->angle = anglesub(scanangle,ANG90);
							animstate = 3;
							objlink = i;
						}
						break;
				   case F_PLAYER:
				   case F_PLAYER+32:
						animstate=0;
						objlink = i;
						mover->angle = scanangle;
						if (dist >= 148l<<FIXSHIFT) {
							if (!cannoncharge) {
								animstate=4;
								animtrig=0;
							} else scanangle = anglesub(scanangle,ANG45>>1);
						} else
						if (dist <= 96l<<FIXSHIFT) {
							animstate=5;
							animtrig=0;
						} else scanangle = anglesub(scanangle,ANG45>>1);
						break;
				 }
			}
			remove();
			if (objlink>=0) {
				object = objectslist.get(objlink);
				if (object != NULL || ((Actor *)object)->health<=0) {
					if (mover->move(mover->angle,3)<2) {
					   
						int dx = (lshr16(object->mover->x - mover->x)) >> 5,
							dy = (lshr16(object->mover->y - mover->y)) >> 5;
						if (ABS(dx)+ABS(dy)>6) objlink=-1;
					   
					}
				} else objlink = -1;
			} else {
				if (!GETFLAG(gameflags,GFL_NORANDOM)) {
				  if (random(20)==13)
					if (random(2)) mover->angle = angleadd(mover->angle,ANG45);
							  else mover->angle = anglesub(mover->angle,ANG45);
				}
				if (mover->move(mover->angle,3)<2)
					for (i=0;i<8;i++) {
						if (mover->move(mover->angle,3)) break;
						mover->angle = angleadd(mover->angle,ANG45);
					}
			  }
			place();
			 
			if (++animtrig >= 8) animtrig = 0;
			animcount = animtrig >> 2;
			 
			if (  mysound<0 && nearPlayer(160l<<FIXSHIFT))
				mysound=play3Dsound(0l,0l,SND_SPIDNEAR,SM_NORMVOL,mover->id,SFL_AUTO);
			break;
		case 1:
			if (speed) {
				remove();
				mover->fluidmove(hitangle,lshl16(speed--));
				place();
			} else {
				if (health) {
					animtrig=0;
					animstate=0;
					animcount=0;
					changefig(F_SPIDER,-8);
				} else if (!animtrig) {
					selflight=0;
					makeObject();
					eventmanager.sendcom(EV_DEAD,mover->id);
					 
				  }
			  }
			if (!health && animtrig) {
				if (GETFLAG(mover->flags,OMF_UNWALKABLE)) {
					remove();
					mover->flags = OMF_VISIBLE;
					place();
					SETFLAG(oflags,OFL_TAKEABLE);
				}
				if (!(--animtrig & 3)) animcount++;
				if (animtrig==4)
					play3Dsound(mover->x,mover->y,SND_SPIDDOWN,SM_NORMVOL,
								SYSOWN,SFL_NORMAL);
			}
			break;
		case 4:  
			switch (animtrig++) {
				case 0: break;
				case 6:
					changefig(F_SPIDER+10,2);
					animcount=0;
					break;
				case 12:
					 
					cx=mover->x; cy=mover->y;
					fireray(cx,cy,scanangle,0,dist,alt,mover->id);

					delete mm_reserve(sizeof(Fireball));
					object = new Fireball(mover->x,mover->y,scanangle, F_SPIDFIRE,
								  mover->z,alt,dist,mover->id);
					play3Dsound(mover->x,mover->y,SND_MISSILE,SM_NORMVOL,
								mover->id,SFL_NORMAL);
					objectslist.put(object);
					scanangle = anglesub(scanangle,ANG45>>1);
					cannoncharge = 40;
					break;
				case 16: changefig(F_SPIDER,-8);
						 animcount=0; break;
				case 20: animtrig=0; animstate=0; break;
			}
			break;
		case 5:  
			remove();mover->move(mover->angle,0);place();
			switch (animtrig++) {
				case 0: break;
				case 6:
					changefig(F_SPIDER+12,2);
					animcount=0;
					play3Dsound(mover->x,mover->y,SND_SPIDATT,SM_NORMVOL,
								mover->id,SFL_NORMAL);
					break;
				case 12:
					 
					cx=mover->x; cy=mover->y;
					i = fireray(cx,cy,scanangle,0,dist,alt,mover->id);
					scanangle = anglesub(scanangle,ANG45>>1);
					if (i>=0)
						eventmanager.senddirect(EV_HIT,mover->id,i,scanangle,4);
					break;
				case 18: changefig(F_SPIDER,-8);
						 animcount=0; break;
				case 20: animtrig=0; animstate=0; break;
			}
			break;
		case 2:
			if (had_seen()) {
				animstate=0;
				reset();	 
			}
			break;
	}
}

void Spider::handle_event(TEvent &event) {
	int dead;
	Object::handle_event(event);
	if (!health) return;
	if ((event.what == EV_HIT ||
		(event.what == EV_SCODE && event.source == SC_MNStrg))) {
		if (event.what == EV_SCODE) {
			health=0; speed=0;
			dead=1;
		} else {
			if (event.source != mover->id) {
				animstate = 3;
				objlink = event.source;
				mover->angle = angleadd(event.data.msg.x,ANG90);
			}
			if ((health-=event.data.msg.y) <=0) {
				health=0;
				eventmanager.senddirect(EV_ADDSTAT,mover->id,event.source,
										STT_KILL,1);
				play3Dsound(mover->x,mover->y,SND_SPIDDEAD,SM_NORMVOL,
							SYSOWN,SFL_NORMAL);
				dead=1;
			} else {
				mysound = play3Dsound(0l,0l,SND_SPIDHIT,SM_NORMVOL,
									  mover->id,SFL_AUTO);
				dead=0;
			  }
			if ((speed += event.data.msg.y*3>>1) > 16) speed = 16;
			hitangle=event.data.msg.x;
		  }

		changefig(F_SPIDER+12+(dead<<1),2-dead);
		animtrig=19-(dead<<2);
		animcount=0;
		animstate=1;
	} else
	if (event.source==mysound && event.what == EV_ENDSOUND && mysound>=0)
		mysound=-1;
}

Soldier::Soldier(fixed px, fixed py)
		:Actor(px,py,F_SOLDIER,8,MVT_2) {
	object_type=OBJT_SOLDIER;
	numenemies++;	 
	animstate=2;
	animtrig=0;
	mover->z = 40l<<16;
	mover->shdim = 4;
	health = 100;
	speed = 0;
	hitangle=0;

	scanangle=ANG0;
	zigzagang=ANG0;
	recover=0;
	objseen=0;
	objlink=-1;

	turncount=0;
	havetoturn=0;
}

Soldier::Soldier(int handle)
		:Actor(handle) {
	numenemies++;	 
	CRC_read(handle,&animstate,sizeof(animstate));
	CRC_read(handle,&animtrig,sizeof(animtrig));
	CRC_read(handle,&objlink,sizeof(objlink));
	CRC_read(handle,&scanangle,sizeof(scanangle));
	CRC_read(handle,&zigzagang,sizeof(zigzagang));
	CRC_read(handle,&recover,sizeof(recover));
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&hitangle,sizeof(hitangle));
	CRC_read(handle,&turncount,sizeof(turncount));
	CRC_read(handle,&havetoturn,sizeof(havetoturn));
	CRC_read(handle,&oldobjx,sizeof(oldobjx));
	CRC_read(handle,&oldobjy,sizeof(oldobjy));
}

void Soldier::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&animstate,sizeof(animstate));
	CRC_write(handle,&animtrig,sizeof(animtrig));
	CRC_write(handle,&objlink,sizeof(objlink));
	CRC_write(handle,&scanangle,sizeof(scanangle));
	CRC_write(handle,&zigzagang,sizeof(zigzagang));
	CRC_write(handle,&recover,sizeof(recover));
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&hitangle,sizeof(hitangle));
	CRC_write(handle,&turncount,sizeof(turncount));
	CRC_write(handle,&havetoturn,sizeof(havetoturn));
	CRC_write(handle,&oldobjx,sizeof(oldobjx));
	CRC_write(handle,&oldobjy,sizeof(oldobjy));
}

void Soldier::animate() {
	fixed cx,cy,dist,alt;
	int i,app;
	Object *object;

	if (viewstate) return;
	if (health<100 && ++recover>=18) { health++; recover=0; }
	switch (animstate) {
		case 0:
		case 3:  
		 

			 
			if (++animtrig > 63) animtrig = 0;
			if (!(animtrig & 0x000f)) {
				switch(animtrig>>4) {
				case 0: animcount=1; break;
				case 1:
				case 3: animcount=0;
					if (nearPlayer(384l<<FIXSHIFT))
						play3Dsound(mover->x,mover->y,SND_PASSO2,SM_NORMVOL,mover->id,SFL_NORMAL);
					break;
				case 2: animcount=2; break;
				}
			}

			 
			if (animstate != 3) {
				scanangle = angleadd(scanangle,ANG45>>1);
				cx = mover->x; cy = mover->y;
				i = fireray(cx,cy,scanangle,0,dist,alt,mover->id);
				objseen = 0;	 
				if (i>=0) {		 
					object = objectslist.get(i);
					if (i == objlink ||
						(object->figstart >= F_PLAYER &&
						 object->figstart <= F_PLAYER+32)) {
						 
						objlink = i;	 
						oldobjx = object->mover->x;
						oldobjy = object->mover->y;
						objseen = 1;
						mover->angle = scanangle;	 
						if (dist<70l<<FIXSHIFT) {	 
							animstate=4;	 
							animtrig=0;
						} else {
							 
							 
							scanangle = anglesub(scanangle,ANG45>>1);
						  }
					}
				}
			}

			 
			remove();
			if (animstate == 3) {	 
				if (mover->move(mover->angle,2)<2) {	 
					animstate=0;	 
					turncount=0;
					havetoturn=0;
				}
			} else {
				if (objlink>=0) {	 
					object = objectslist.get(objlink);
					if (object != NULL && ((Actor *)object)->health>0) {
						 
						if (!objseen) {  
							 
							if (turncount) {
								if (turncount>0) {
									mover->angle = anglesub(mover->angle,ANG45>>3);
									if (!(--turncount)) havetoturn--;
								} else
								if (turncount<0) {
									mover->angle = angleadd(mover->angle,ANG45>>3);
									if (!(++turncount)) havetoturn++;
								}
								 
							} else
							if (havetoturn) {
								if (mover->move(app = anglesub(mover->angle,ANG45),2)>1) {
									turncount = 8;
								} else {
									int j;
									j = xytoidx(mover->x+(costab[app]<<6),
												mover->y+(sintab[app]<<6));
									if ((Map[j].type==MTP_DOOR1 || Map[j].type==MTP_DOOR2) &&
										(app = doormanager.doorpos(Map[j].data))<64) {
											eventmanager.sendshort(EV_ACTIVATE,mover->id,0,j,0);
											mover->move(mover->angle,0);
									} else {
										 
										if (mover->move(mover->angle,2)<2) {
											j = xytoidx(mover->x+(costab[mover->angle]<<6),
														mover->y+(sintab[mover->angle]<<6));
											if (Map[j].type==MTP_DOOR1 || Map[j].type==MTP_DOOR2) {
												if (!(app = doormanager.doorpos(Map[j].data)))
													eventmanager.sendshort(EV_ACTIVATE,mover->id,0,j,0);
												else
												if (app == 64) turncount = -8;
											} else
												turncount = -8;
										}
									  }
								  }
							} else {
								int dx = (lshr16(oldobjx - mover->x)) >> 4,
									dy = (lshr16(oldobjy - mover->y)) >> 4,
									dir;

								if (dx>0) {
									if (dy>0) dir=1; else
									if (dy<0) dir=7; else
										dir=0;
								} else
								if (dx<0) {
									if (dy>0) dir=3; else
									if (dy<0) dir=5; else
										dir=4;
								} else {
									if (dy>0) dir=2; else
									if (dy<0) dir=6; else
										dir=-1;	 
								  }
								if (dir>=0)
									mover->angle = dir*ANG45;
								if (!mover->move(mover->angle,2)) {
									int j = xytoidx(mover->x+(costab[mover->angle]<<6),
													mover->y+(sintab[mover->angle]<<6));
									if (Map[j].type==MTP_DOOR1 || Map[j].type==MTP_DOOR2)
										eventmanager.sendshort(EV_ACTIVATE,mover->id,0,j,0);
									else
										turncount = -16;
								}
							  }
						} else {
							turncount=0;
							havetoturn=0;
							 
							zigzagang=angleadd(zigzagang,ANGLVL<<3);
							i = mover->angle;
							if ((mover->angle = i+(int)fixmul(ANG45,sintab[zigzagang]))>=ANG360)
								mover->angle-=ANG360; else
							if (mover->angle<ANG0) mover->angle+=ANG360;
							if (!mover->move(mover->angle,3)) {
								int j = xytoidx(mover->x+(costab[mover->angle]<<6),
											mover->y+(sintab[mover->angle]<<6));
								if (Map[j].type==MTP_DOOR1 || Map[j].type==MTP_DOOR2)
									eventmanager.sendshort(EV_ACTIVATE,mover->id,0,j,0);
							}
							mover->angle = i;
						  }
					} else {
						 
						objlink=-1;
						turncount=0;
						havetoturn=0;
					  }
				} else {
					if (!mover->move(mover->angle,1)) {
						mover->angle = (mover->angle/ANG45)*ANG45;
						mover->angle = angleadd(mover->angle,ANG45);
					}
				  }
			  }
			place();
			break;
		case 1:  
			recover=0;
			if (speed) {
				remove();
				mover->fluidmove(hitangle,lshl16(speed--));
				if (speed) speed--;
				place();
			} else {
				if (health) {
					animtrig=0;
					animstate=0;
					animcount=0;
					turncount=0;
					havetoturn=0;
					changefig(F_SOLDIER,8);
				} else if (!animtrig) {
					selflight=0;
					makeObject();
					eventmanager.sendcom(EV_DEAD,mover->id);
					 
				  }
			  }
			if (!health && animtrig) {
				if (GETFLAG(mover->flags,OMF_UNWALKABLE)) {
					remove();
					mover->flags = OMF_VISIBLE;
					place();
					SETFLAG(oflags,OFL_TAKEABLE);
				}
				if (!(--animtrig % 6)) animcount++;
				if (animtrig==6)
					play3Dsound(mover->x,mover->y,SND_SOLDDOWN,SM_NORMVOL,
								SYSOWN,SFL_NORMAL);
			}
			break;
		case 4:  
			if (animtrig & 1) {remove(); mover->move(mover->angle,0); place();}
			switch (animtrig++) {
				case 0: animcount=0; break;
				case 3:
					changefig(F_SOLDIER+24,2);
					 
					play3Dsound(mover->x,mover->y,SND_SOLDATT,SM_NORMVOL,
								mover->id,SFL_NORMAL);
					break;
				case 8: animcount++; break;
				case 9:
					 
					cx=mover->x; cy=mover->y;
					i = fireray(cx,cy,scanangle,0,dist,alt,mover->id);
					if (i>=0 && dist<80l<<FIXSHIFT)
						eventmanager.senddirect(EV_HIT,mover->id,i,scanangle,12);
					break;
				case 14:
					changefig(F_SOLDIER,8);
					animcount=0; break;
				case 16:
					animtrig=0;
					animstate=0;
					turncount=0;
					havetoturn=0;
					break;
			}
			break;
		case 2:
			if (had_seen()) {
				animstate=0;
				turncount=0;
				havetoturn=0;
				reset();	 
			}
			break;
	}

#ifdef KAOS_DEMO_TRACE
	fprintf(stderr,
		"KAOS_TIC tic=%llu soldier=%d x=%08X y=%08X ang=%d state=%d "
		"speed=%d scan=%d objlink=%d old=%08X,%08X turn=%d have=%d seen=%d\n",
		(unsigned long long)kaos_sim_tic, mover ? mover->id : -1,
		mover ? (unsigned)(std::uint32_t)mover->x : 0u,
		mover ? (unsigned)(std::uint32_t)mover->y : 0u,
		mover ? mover->angle : 0, animstate, speed, scanangle, objlink,
		(unsigned)(std::uint32_t)oldobjx, (unsigned)(std::uint32_t)oldobjy,
		turncount, havetoturn, objseen);
#endif
}


void Soldier::handle_event(TEvent &event) {
	int dead;
#ifdef KAOS_DEMO_TRACE
	if (event.what == EV_PLNOISE || event.what == EV_ENDSOUND)
		fprintf(stderr,
			"KAOS_EVENT tic=%llu soldier=%d what=%d source=%d "
			"pos=%08X,%08X objlink=%d\n",
			(unsigned long long)kaos_sim_tic, mover ? mover->id : -1,
			event.what, event.source,
			(unsigned)(std::uint32_t)event.data.pos.x,
			(unsigned)(std::uint32_t)event.data.pos.y, objlink);
#endif
	Object::handle_event(event);
	if (!health) return;
	if (event.what == EV_ALERT) {
		animstate=3;	 
		if (anglesub(angleadd(mover->angle,ANG180),event.data.msg.x) <= ANG180)
			mover->angle = anglesub(event.data.msg.x,ANG90);
		else
			mover->angle = angleadd(event.data.msg.x,ANG90);
	} else
	if ((event.what == EV_HIT ||
		(event.what == EV_SCODE && event.source == SC_MNStrg))) {
		if (event.what == EV_SCODE) {
			health=0; speed=0;
			dead=1;
		} else {
			if ((health-=event.data.msg.y) <=0) {
				health=0;
				eventmanager.senddirect(EV_ADDSTAT,mover->id,event.source,
										STT_KILL,1);
				play3Dsound(mover->x,mover->y,SND_SOLDDEAD,SM_NORMVOL,
							SYSOWN,SFL_NORMAL);
				dead=1;
			} else {
				play3Dsound(0l,0l,SND_SOLDHIT,SM_NORMVOL,
							mover->id,SFL_AUTO);
				dead=0;
			  }
			if ((speed+=event.data.msg.y*3>>1) > 16) speed = 16;
			scanangle = angleadd(hitangle=event.data.msg.x,ANG180);
			objlink = event.source;
		  }
		changefig(F_SOLDIER+28+(dead<<1),2-dead);
		 
		animtrig=29-(dead*6);
		animcount=0;
		animstate=1;
	} else
	if (event.what==EV_PLNOISE && event.source==objlink) {
		 
			oldobjx = event.data.pos.x;
			oldobjy = event.data.pos.y;
		 
	}
}

Mouse::Mouse(fixed px, fixed py)
	  :Actor(px,py,F_MOUSE,-8,MVT_2) {
	object_type=OBJT_MOUSE;
	animstate = 2;
	health = 5;
	mover->z = 4l<<16;
	mover->shdim = 3;
	CLRFLAG(mover->flags,OMF_UNWALKABLE);
}

Mouse::Mouse(int handle)
	  :Actor(handle) {
	CRC_read(handle,&animstate,sizeof(animstate));
	CRC_read(handle,&animtrig,sizeof(animtrig));
}

void Mouse::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&animstate,sizeof(animstate));
	CRC_write(handle,&animtrig,sizeof(animtrig));
}

void Mouse::animate() {
	if (viewstate) return;
	switch (animstate) {
		case 1:	 
			if (++animtrig>=20) {
				remove();
				mover->flags = OMF_VISIBLE;
				place();
				makeObject();
				eventmanager.sendcom(EV_DEAD,mover->id);
				 
				break;
			}
			animcount=animtrig>>2;
		case 0:  
		case 3:  
			remove();
			if (mover->move(mover->angle,2) != 2) {
				if (!GETFLAG(gameflags,GFL_NORANDOM)) {	 
					if (animstate!=1 && random(10)==5)
						if (animstate) animstate=0; else animstate=3;
				}
				if (animstate) mover->angle = anglesub(mover->angle,ANG45);
						  else mover->angle = angleadd(mover->angle,ANG45);
			}
			place();
			if (have_seen() && random(50)==37) {
				play3Dsound(0l,0l,SND_TOPO,SM_NORMVOL,mover->id,SFL_AUTO);
				reset();  
			}
			break;
		case 2:
			if (had_seen()) {
				animstate=0;
				reset();	 
			}
			break;
	}
}

void Mouse::handle_event(TEvent &event) {
	 







	if (!health) return;
	Object::handle_event(event);
	if (event.what == EV_HIT ||
		(event.what == EV_SCODE && event.source == SC_MNStrg)) {
	  if (event.what == EV_SCODE || (health-=event.data.msg.y) <= 0) {
		health=0;
		animtrig=0;
		animstate=1;
		changefig(F_MOUSE+5,1);
		 




	  }
	}
}

FireBoom::FireBoom(fixed px, fixed py, fixed h, int angle, byte boomtype)
		 :LightActor(px,py,F_FIREBOOM,1,0,10,MVT_0) {
	object_type=OBJT_FIREBOOM;
	lightz = mover->z = h;
	animtrig = 0;  
	mover->flags = OMF_VISIBLE;
	switch (type = boomtype) {	 

		case BOOM_ASCIA: animcount++;
		case BOOM_PALLOT: mover->shdim=2; break;
		case BOOM_SPIDER: animcount+=3; selflight=2;
		case BOOM_SLIMER: lightrange-=3; animcount+=6;
		 
		case BOOM_MISSIL:
			mover->shdim=4;
			changefig(F_BOOM,1);
			lightrange+=3;
			break;
		case BOOM_BLOOD:
			decay = 0l;
			 





				decayspd = (16384l * (random(8)+1)) >> 3;  
				animtrig=random(4);
			 
			mover->shdim=3;
			animcount=0;
			selflight=0;
			changefig(F_BLOOD,1);
			break;
		case BOOM_SMOG:
			changefig(F_SMOG,1);
			 
			selflight=0;
			 
			lightrange=0;
			break;
		default: type = BOOM_PALLOT;
	}
	mover->x -= costab[angle]<<(mover->shdim-1);
	mover->y -= sintab[angle]<<(mover->shdim-1);
	if (type != BOOM_MISSIL) CLRFLAG(oflags,OFL_HASLIGHT);
}

FireBoom::FireBoom(int handle)
		 :LightActor(handle) {
	CRC_read(handle,&animtrig,sizeof(animtrig));
	CRC_read(handle,&type,sizeof(type));
}

void FireBoom::save(int handle) {
	LightActor::save(handle);
	CRC_write(handle,&animtrig,sizeof(animtrig));
	CRC_write(handle,&type,sizeof(type));
}

void FireBoom::animate() {
const byte light[6] = {2,4,5,3,2,1};
	switch(type) {
		case BOOM_PALLOT:
			mover->z+=FIXONE>>1;
			break;
		case BOOM_BLOOD:
			if ((mover->z-=decay)<FIXONE) mover->z=FIXONE;
			decay += decayspd;
			break;
		case BOOM_SMOG:
			mover->z+=FIXONE>>1;
			 
			break;
	}
	if (++animtrig >= 4) {
		animtrig = 0;
		animcount++;
		switch (type) {
			case BOOM_PALLOT:
				if (animcount==2 && mover->under<0  )
					play3Dsound(mover->x,mover->y,SND_PROIETTILE,SM_NORMVOL,
								SYSOWN,SFL_NORMAL & ~SFL_AUTOSTOP);
			case BOOM_ASCIA:
				if (animcount<=3) return;
				animcount=3;
				break;
			case BOOM_MISSIL:
			 
				 
				if (animcount<=4) {
					remove();
					lightrange = light[animcount];
					place();
					return;
				}
				animcount=4;
				break;
			case BOOM_BLOOD:
				if (animcount<=4) return;
				animcount=4;
				eventmanager.sendshort(EV_CHGTEXTURE,2,0,mover->index,0);
				break;
			case BOOM_SMOG:
				if (animcount<=2) return;
				animcount=2;
				break;
			 










			case BOOM_SLIMER:
				 
				if (animcount<=8) return;
				animcount=8;
				break;
			case BOOM_SPIDER:
				 
				if (animcount<=11) return;
				animcount=11;
				break;
		}
		if (!eventmanager.sendcom(EV_KILL,mover->id)) {
			animtrig=4;
			animcount--;
		}
	}
}

int FireBoom::getlightfig() {
	 
	return F_SPIKELIGHT;
	 
}

Fireball::Fireball(fixed px, fixed py, int fireangle,   int type,
				   fixed h1, fixed h2, fixed dist, int _ownerid)
		 :LightActor(px,py,F_MISSILE,-8,3,5,MVT_1)	 

{
	object_type=OBJT_FIREBALL;
	mover->angle = fireangle;
	mover->shdim = 3;
	animtrig=0;
	 
	switch (type) {
		case F_MISSILE:
			shspeed = 4;
			 
			CLRFLAG(oflags,OFL_HASLIGHT);
			break;
		case F_SLIMFIRE:
			changefig(F_SLIMFIRE,1);
			shspeed = 3;
			break;
		case F_SPIDFIRE:
			changefig(F_SPIDFIRE,1);
			shspeed = 3;
			CLRFLAG(oflags,OFL_HASLIGHT);
			break;
	}
	if (dist>>=16+shspeed) deh = fixdiv(h2-h1,dist);
					  else deh = 0l;
	lightz = mover->z = h1;
	ownerid = _ownerid;
	sendsound=1;
	boomcount=1365;  
}

Fireball::Fireball(int handle)
		 :LightActor(handle) {
	CRC_read(handle,&ownerid,sizeof(ownerid));
	CRC_read(handle,&deh,sizeof(deh));
	CRC_read(handle,&boomcount,sizeof(boomcount));
	CRC_read(handle,&shspeed,sizeof(shspeed));	 
	sendsound=1;
}

void Fireball::save(int handle) {
	LightActor::save(handle);
	CRC_write(handle,&ownerid,sizeof(ownerid));
	CRC_write(handle,&deh,sizeof(deh));
	CRC_write(handle,&boomcount,sizeof(boomcount));
	CRC_write(handle,&shspeed,sizeof(shspeed));	 
}

void Fireball::animate() {
	fixed &x = mover->x,
		  &y = mover->y,
		  &z = mover->z;
	fixed cn, sn, cl, sl;
	fixed cx,cy,dist,alt;
	int idx, objid, altok;
	int i = 0;
	Object *owner;
	Mover *mvr;

	if (viewstate) {
		sendsound=1;
		return;
	}

	if (sendsound) {
		if (figstart==F_MISSILE) i=SM_MAXVOL; else i=SM_NORMVOL;
		play3Dsound(0l,0l,SND_MISSILGO,i,mover->id,SFL_AUTO|SFL_CONTINUE);
		sendsound=0;
	}

	switch (figstart) {
		case F_MISSILE:
			if (animtrig++ >= 3) {
				animtrig=0;
				delete mm_reserve(sizeof(FireBoom));
				objectslist.put(new FireBoom(mover->x,mover->y,mover->z,
											mover->angle,BOOM_SMOG));
			}
			break;
		case F_SLIMFIRE:
			if (++animtrig>7) {
				animcount = (animcount+1) & 1;
				animtrig = 0;
			}
			break;
		case F_SPIDFIRE:
			deh-=256l;
			if (++animtrig>4) {
				animcount = (animcount+1) & 3;
				animtrig = 0;
			}
			break;
	}

	remove();
	if ((owner = objectslist.get(ownerid)) != NULL) {
		 
		owner->remove();
	}
	cn = costab[mover->angle];
	sn = sintab[mover->angle];
	 





	objid = -1;
	if (health) {
		int old = boomcount;
		 




		cx=x-(sn<<mover->shdim); cy=y+(cn<<mover->shdim);
		objid = fireray(cx,cy,mover->angle,0,dist,alt,mover->id);
	   if (objid==-2) boomcount=old=0; else {
		altok = (objid<0) || (LABS(z-alt)<=FIXONE<<mover->shdim);
		if (!(i = (dist<=64l<<FIXSHIFT && altok))) {
		 cx=x+(sn<<mover->shdim); cy=y-(cn<<mover->shdim);
		 objid = fireray(cx,cy,mover->angle,0,dist,alt,mover->id);
		if (objid==-2) boomcount=old=0; else {
		 altok = (objid<0) || (LABS(z-alt)<=FIXONE<<mover->shdim);
		 if (!(i= (dist<=64l<<FIXSHIFT && altok))) {
			cx=x; cy=y;
			objid = fireray(cx,cy,mover->angle,0,dist,alt,mover->id);
		   if (objid==-2) boomcount=old=0; else {
			altok = (objid<0) || (LABS(z-alt)<=FIXONE<<mover->shdim);
			i = (dist<=64l<<FIXSHIFT && altok);
		   }
		 }
		}
		}
	   }
		if (i)
			if ((boomcount=(lshr16(dist)>>(shspeed )))<0 || boomcount>old)
				boomcount = old;
		if (objid>=0 && objectslist.get(objid)->figstart==F_SOLDIER)
				eventmanager.senddirect(EV_ALERT,mover->id,objid,mover->angle,0);
	} else {
		i=1;
		boomcount=0;
	  }
	if (boomcount && (z+deh>FIXONE<<mover->shdim) &&
					 (z+deh<(128l<<FIXSHIFT)-(FIXONE<<mover->shdim))) {
					  
		boomcount--;
		x += cn<<shspeed;
		y += sn<<shspeed;
		lightz = (z += deh);
	} else {
	   
	   













	  cx=x; cy=y;
	  objid = fireray(cx,cy,mover->angle,0,dist,alt,mover->id);
	  altok = (objid<0) || (LABS(z-alt)<=FIXONE<<mover->shdim);
	  if (objid==-2 || (dist<64l<<FIXSHIFT && altok)) {
	   if (objid>=0) {
		 
		switch (figstart) {
			case F_SLIMFIRE:
				eventmanager.senddirect(EV_HIT,ownerid,objid,mover->angle,10);
				break;
			case F_SPIDFIRE:
				eventmanager.senddirect(EV_POISON,ownerid,objid,mover->angle,6);
				break;
			default:
				eventmanager.senddirect(EV_HIT,ownerid,objid,mover->angle,ArmList[ARM_BAZOOKA].power);
		}
	   }
	  }
	  if (eventmanager.sendcom(EV_KILL,mover->id) && objid>-2) {
		 
		 
		switch (figstart) {
			case F_SPIDFIRE:
				play3Dsound(x,y,SND_VENOM,SM_NORMVOL,SYSOWN,SFL_NORMAL);
				i=BOOM_SPIDER;
				break;
			case F_SLIMFIRE:
				eventmanager.sendpos(EV_BOOM,ownerid,x,y,8l);  
				i=BOOM_SLIMER;
				 
				break;
			default:
				eventmanager.sendpos(EV_BOOM,ownerid,x,y,
								static_cast<fixed>(ArmList[ARM_BAZOOKA].power));
				i=BOOM_MISSIL;
				quake = 14;
				break;
		}
		delete mm_reserve(sizeof(FireBoom));
		objectslist.put(new FireBoom(x,y,z,mover->angle,i));
	  }
	}
	if (owner) owner->place();
	place();
}

void Fireball::handle_event(TEvent &event) {
	if (!health) return;
	Object::handle_event(event);
	if (event.what == EV_HIT && event.data.msg.y>=10) health=0;
}

int Fireball::getlightfig() {
	return F_CIRCLIGHT;
}

Bomb::Bomb(fixed px, fixed py, fixed h, int angle, int strng, int _ownerid)
	 :Actor(px,py,F_BOMB,-8) {
	object_type = OBJT_BOMB;
	if (!(strenght = strng)) makeObject();	 
	mover->z = h;
	mover->angle = angle;
	mover->shdim = 3;	 
	timer = 5*36;
	mover->flags = OMF_VISIBLE|OMF_HITABLE;
	SETFLAG(oflags,OFL_TAKEABLE);
	animtrig = 0;
	decay = FIXONE;
	ownerid = _ownerid;
}

Bomb::Bomb(int handle)
	 :Actor(handle) {
	CRC_read(handle,&ownerid,sizeof(ownerid));
	CRC_read(handle,&decay,sizeof(decay));
	CRC_read(handle,&strenght,sizeof(strenght));
	CRC_read(handle,&timer,sizeof(timer));
	CRC_read(handle,&animtrig,sizeof(animtrig));
}

void Bomb::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&ownerid,sizeof(ownerid));
	CRC_write(handle,&decay,sizeof(decay));
	CRC_write(handle,&strenght,sizeof(strenght));
	CRC_write(handle,&timer,sizeof(timer));
	CRC_write(handle,&animtrig,sizeof(animtrig));
}

void Bomb::animate() {
	 
	if (viewstate) return;
	if (!timer--) {
		 
		eventmanager.sendcom(EV_KILL,mover->id);
		eventmanager.sendpos(EV_BOOM,ownerid,mover->x,mover->y,
								ArmList[ARM_BOMB].power);
		delete mm_reserve(sizeof(FireBoom));
		objectslist.put(new FireBoom(mover->x,mover->y,mover->z+(8l<<16),
									 mover->angle,BOOM_MISSIL));
		quake = 21;	 
	} else {
		if (animtrig>=0) {
			if (++animtrig >= 6) {
				animtrig = 0;
				animcount = (animcount+1) % 3;
			}
			mover->z += (decay -= FIXONE>>2);	 
			if (mover->z <= 0) {
				mover->z = 0;
				decay = -(decay>>1);
				if (decay < (FIXONE>>4)) animtrig = -1;
				strenght >>= 1;
				play3Dsound(mover->x,mover->y,SND_BBOUNCE,SM_NORMVOL,mover->id,SFL_NORMAL);
			}
			if (strenght) {
				Object *obj = objectslist.get(ownerid);
				remove();
				if (obj) obj->remove();
				fixed nx = mover->x + (costab[mover->angle]*(strenght>>1)),
					  ny = mover->y + (sintab[mover->angle]*(strenght>>1));
				if (!mover->canmove(nx,mover->y)) {
					if (mover->angle<=ANG180) mover->angle=ANG180-mover->angle;
										 else mover->angle=ANG180+ANG360-mover->angle;
					play3Dsound(mover->x,mover->y,SND_BBOUNCE,SM_NORMVOL,mover->id,SFL_NORMAL);
				} else mover->x = nx;
				if (!mover->canmove(mover->x,ny)) {
					if (mover->angle!=ANG0) mover->angle=ANG360-mover->angle;
					play3Dsound(mover->x,mover->y,SND_BBOUNCE,SM_NORMVOL,mover->id,SFL_NORMAL);
				} else mover->y = ny;
				if (obj) obj->place();
				place();
				 
			}
		}
	  }
}

void Bomb::handle_event(TEvent &event) {
	if (event.what == EV_GOT) {
		if (!GETFLAG(oflags,OFL_ISACTOR)) {
			Object::handle_event(event);
			return;
		}
	} else Object::handle_event(event);
	switch (event.what) {
	case EV_HIT:
	case EV_GOT:
		 
		 
		CLRFLAG(oflags,OFL_TAKEABLE);
		eventmanager.sendcom(EV_KILL,mover->id);
		eventmanager.sendpos(EV_BOOM,ownerid,mover->x,mover->y,
								ArmList[ARM_BOMB].power);
		delete mm_reserve(sizeof(FireBoom));
		objectslist.put(new FireBoom(mover->x,mover->y,mover->z+(8l<<16),
									 mover->angle,BOOM_MISSIL));
		quake = 21;	 
		break;
	}
}

Torcia::Torcia(fixed px, fixed py, int fig)
	:LightActor(px,py,fig,1,2,4,MVT_0)
{
	object_type=OBJT_TORCIA;
	if (fig==F_BRAZIER) {
		mover->z = 20l<<16;
		mover->shdim = 4;
		CLRFLAG(oflags,OFL_HASLIGHT);
	} else {
		lightz = mover->z = (32l+20)<<16;
		mover->shdim = 2;
		mover->flags = OMF_VISIBLE;
	  }
	health = 10;
	if (GETFLAG(gameflags,GFL_NORANDOM)) {
		animcount = (lshr16(mover->x)>>6) % 3;
		animtrig = (lshr16(mover->y)>>6) % 3;
	} else {
		animcount = random(3);
		animtrig = random(3);
	  }
	fireid = -1;
}

Torcia::Torcia(int handle)
	   :LightActor(handle) {
	if (GETFLAG(gameflags,GFL_NORANDOM)) {
		animcount = (lshr16(mover->x)>>6) % 3;
		animtrig = (lshr16(mover->y)>>6) % 3;
	} else {
		animcount = random(3);
		animtrig = random(3);
	  }
	fireid = -1;
}

void Torcia::animate() {
	char lr;
	if (animtrig>=100) {
		if (++animtrig >= 116) {
			animcount=3;
			selflight=0;
			 
			 
			CLRFLAG(oflags,OFL_HASLIGHT);
			if (lightrange) {
				remove();
				lightrange=0;
				place();
			}
			if (fireid>=0) {
				stopsound(fireid);
				fireid=-1;
				 
			}
			makeObject();
		} else animcount = (animtrig/6) % 3;
		return;
	}

	if (nearPlayer(128l<<FIXSHIFT)) {
			if (fireid<0)
				if (figstart==F_BRAZIER)
					fireid=play3Dsound(mover->x,mover->y,SND_FIRE,0,mover->id,SFL_NORMAL|SFL_CONTINUE);
				else
					fireid=play3Dsound(mover->x,mover->y,SND_FIRE,SM_LOWVOL,mover->id,SFL_NORMAL|SFL_CONTINUE);
	   } else
			if (fireid>=0)
				if (stopsound(fireid)) fireid=-1;

	if (!have_seen()) {
		if (lightrange) {
			remove();
			lightrange=0;
			place();
		}
		 



		return;
	} else reset();	 

	 












	if (figstart==F_BRAZIER) {
		if (++animtrig >= 8) {
			animtrig = 0;
			if (++animcount > 2) animcount = 0;
		}
		if (!(animtrig & 3))
		  if (GETFLAG(gameflags,GFL_NORANDOM)) {
				if (lightrange != animcount+2) {
					remove();
					lightrange = animcount+2;
					place();
				}
		  } else {
				if (lightrange != (lr = random(2)+2)) {
					remove();
					lightrange = lr;
					place();
				}
			}
	} else {
		if (++animtrig > 5) {
			animtrig = 0;
			if (++animcount > 2) animcount = 0;	 
		}
		if (animtrig & 1) {
			 
			if (GETFLAG(gameflags,GFL_NORANDOM)) {
				if (lightrange != animcount+1) {
					remove();
					lightrange = animcount+1;
					place();
				}
			} else {
				if (lightrange != (lr = random(2)+1)) {
					remove();
					lightrange = lr;
					place();
				}
			}
		}
	  }
}

void Torcia::handle_event(TEvent &event) {
	if (!health) return;
	if (event.what == EV_BOOM &&
		(LABS(mover->x-event.data.pos.x)<=48l<<FIXSHIFT) &&
		(LABS(mover->y-event.data.pos.y)<=48l<<FIXSHIFT)) {
	  if ((int)event.data.pos.z > health) {
		health = 0;
		animtrig = 100;
	  }
	} else
	if (event.what == EV_SCODE && event.source == SC_MNDark) {
		health = 0;
		animtrig = 100;
	} else
	if (fireid>=0 && event.what == EV_ENDSOUND && event.source==fireid)
		fireid=-1;
}

int Torcia::getlightfig() {
	return F_TORCLIGHT;
}

Lampadario::Lampadario(fixed px, fixed py, int fig)
		   :LightActor(px,py,fig,1,3,6,MVT_0)
{
	object_type=OBJT_LAMPADARIO;
	if (GETFLAG(gameflags,GFL_NORANDOM))
		animtrig = (lshr16(px)>>6) % 6;
	else
		animtrig = random(6);
	mover->flags = OMF_VISIBLE|OMF_HITABLE;
	if (fig==F_LAMP) {
		mover->z = (68l+16l)<<FIXSHIFT;  
		lightz = mover->z+(FIXONE<<1);
	} else {
		mover->z=32l<<FIXSHIFT;
		SETFLAG(mover->flags,OMF_UNWALKABLE|OMF_HITABLE);
		lightz = 52l<<FIXSHIFT;
	}
	lightpow = 1;
	mover->shdim = 2+1;	 
	health = 18;
	decay = 0;
}

Lampadario::Lampadario(int handle)
		   :LightActor(handle) {
	if (GETFLAG(gameflags,GFL_NORANDOM))
		animtrig = (lshr16(mover->x)>>6) % 6;
	else
		animtrig = random(6);
	decay=0;
}

void Lampadario::animate() {
	char lr;
	if (animtrig==127) {
		makeObject();
		return;
	}
	if (animtrig>=100) {
		if (animtrig==100) {
			selflight=0;
			changefig(F_LAMP+2,1);
			animcount=0;
			animtrig++;
			CLRFLAG(oflags,OFL_HASLIGHT);
			if (lightrange) {
				remove();
				lightrange=0;
				place();
			}
			 
		} else
		if (animtrig == 101) {
			decay+=FIXONE;
			if ((mover->z -= decay)<=0l) {
				animtrig++;
				mover->z=0l;
				changefig(F_LAMP+3,1);
				play3Dsound(mover->x,mover->y,SND_GLASS,0,
						mover->id,SFL_NORMAL & ~SFL_AUTOSTOP);
			}
		} else {
			if (++animtrig >= 108) {
				changefig(F_LAMPX,1);
				remove();
				mover->flags = OMF_VISIBLE;
				place();
				makeObject();
			}
		  }
		return;
	}
	if (!have_seen()) {
		if (lightrange) {
			remove();
			lightrange=0;
			place();
		}
		return;
	} else reset();	 
	if (++animtrig >= 8) {
		animtrig = 0;
		if (++animcount > 1) animcount = 0;
	}
	if (!(animtrig & 3)) {
		if (GETFLAG(gameflags,GFL_NORANDOM)) {
			if (lightrange != animcount+2) {
				remove();
				lightrange = animcount+2;
				place();
			}
		} else
			if (lightrange != (lr = random(2)+2)) {
				remove();
				lightrange = lr;
				place();
			}
	}
}

void Lampadario::handle_event(TEvent &event) {
	Object::handle_event(event);
	if (animtrig<100 && event.what == EV_HIT) {
		 
		 
	  if (event.data.msg.y>7 && (health-=(int)event.data.msg.y) <= 0) {
		health=0;
		animtrig=100;
		 
		makeActor();
	  }
	} else
	if (animtrig<100 && (oflags & OFL_ISACTOR) &&
		event.what == EV_SCODE && event.source == SC_MNDark) {
		 
		remove();
		lightrange=0;
		selflight=0;
		place();
		animcount=0;
		if (figstart==F_LAMP) changefig(F_LAMPOFF,1);
						 else changefig(F_TRESPOLOFF,1);
		CLRFLAG(oflags,OFL_HASLIGHT);
		animtrig=234;
		makeObject();
	}
}

int Lampadario::getlightfig() {
	return F_LAMPLIGHT;
}

Barile::Barile(fixed px, fixed py)
	   :LightActor(px,py,F_BARILE,1,0,0,MVT_1)
{
	object_type=OBJT_BARILE;
	health = 8;
	speed = 0;
	mover->z = 24l<<16;
	 
	animtrig = 0;
	ownerid = -1;
}

Barile::Barile(int handle)
	   :LightActor(handle) {
	CRC_read(handle,&animtrig,sizeof(animtrig));
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&ownerid,sizeof(ownerid));
}

void Barile::save(int handle) {
	LightActor::save(handle);
	CRC_write(handle,&animtrig,sizeof(animtrig));
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&ownerid,sizeof(ownerid));
}

void Barile::animate() {
	 
	const byte animlight[15] = {0,0,1,1,2,3,4,3,3,2,2,1,1,0,0};
	if (health<=0) {
		selflight = 8;
		animcount = ++animtrig/6;  
	}
	remove();
	if (speed) {
		 






		mover->fluidmove(mover->angle,lshl16(speed));
		if (!(--speed) && !animtrig)
			 
			if (!makeObject()) speed=1;
	}
	 







	 
	if (animtrig == 11) {
		if (ownerid>=0)
			 
			eventmanager.sendpos(EV_BOOM,ownerid,mover->x,mover->y,32l);
		else eventmanager.sendpos(EV_BOOM,mover->id,mover->x,mover->y,32l);
		quake = 28;	 
	}
	if (animcount > 4) {
		changefig(F_BARILX,1);
		 
		mover->flags = OMF_VISIBLE;
		if (makeObject()) {
			lightrange=0;
			selflight=0;
			animcount=0;
		}
	} else lightrange = animlight[animtrig>>1];
	place();
}

void Barile::handle_event(TEvent &event) {
	if (animtrig) return;
	Object::handle_event(event);
	if (event.what == EV_HIT) {
	  if ((health-=event.data.msg.y) < 0) {
		health=0;
		ownerid = event.source;
	  }
	  makeActor();	 
	  mover->angle = event.data.msg.x;
	  if ((speed += event.data.msg.y>>1)>9) speed=9;
	} else
	 







	if (event.what == EV_SCODE && event.source == SC_MNBoom) {
		health = 0;
		makeActor();
		mover->angle = 0;
		speed = 0;
	}
}

Vaso::Vaso(fixed px, fixed py, int fig, byte objin)
	 :Actor(px,py,fig,1,MVT_1) {
	object_type=OBJT_VASO;
	animtrig = 0;
	speed = 0;
	mover->z = 24l<<FIXSHIFT;
	health = 1;
	if (fig == F_VASO) mover->shdim = 3;
	whatin = objin;
}

Vaso::Vaso(int handle)
	 :Actor(handle) {
	CRC_read(handle,&animtrig,sizeof(animtrig));
	CRC_read(handle,&speed,sizeof(speed));
	CRC_read(handle,&whatin,sizeof(whatin));
}

void Vaso::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&animtrig,sizeof(animtrig));
	CRC_write(handle,&speed,sizeof(speed));
	CRC_write(handle,&whatin,sizeof(whatin));
}

void Vaso::animate() {
	if (health<=0)
		animcount = (++animtrig/6)+1;  
	remove();
	if (speed) {
		mover->fluidmove(mover->angle,lshl16(speed));
		if (!(--speed) && !animtrig)
			if (!makeObject()) speed++;
	}
	 
	if ((figstart == F_VASO && animcount >= 5) ||
		(figstart == F_PIANTA && animcount >= 4)) {
		mover->flags = OMF_VISIBLE;
		if (!makeObject()) animtrig--;
	}
	place();
}

void Vaso::handle_event(TEvent &event) {
	if (animtrig) return;
	Object::handle_event(event);
	if (event.what == EV_HIT) {
	  if ((health-=event.data.msg.y) < 0) {
		health=0;
		if (figstart==F_VASO)
			play3Dsound(mover->x,mover->y,SND_VASO1,SM_NORMVOL,mover->id,SFL_NORMAL & ~SFL_AUTOSTOP);
		else
			play3Dsound(mover->x,mover->y,SND_VASO2,SM_NORMVOL,mover->id,SFL_NORMAL & ~SFL_AUTOSTOP);
	  }
	  makeActor();	 
	  animtrig++;  
	  mover->angle = event.data.msg.x;
	  if ((speed += event.data.msg.y>>1)>9) speed=9;

	  remove();
	  mover->flags = OMF_VISIBLE;
	  place();

	   
	   
		int angle,
			strength = event.data.msg.y;
		word fig;
		Object *obj;

		if (!GETFLAG(gameflags,GFL_NORANDOM))
			angle = anglesub(angleadd(event.data.msg.x,ANG180+(ANG30>>1)),random(ANG30));
		else
			angle = angleadd(event.data.msg.x,ANG180);
		 




		switch (whatin) {
			case 0: fig = 0; break;
			case 1: fig = F_FUCILE; break;
			case 2: fig = F_MITRA; break;
			case 3: fig = F_BAZOOKA; break;
			case 4: fig = F_BOMB; break;
			case 5: fig = F_CARTUCCIA1; break;
			case 6: fig = F_CARTUCCIA2; break;
			case 7: fig = F_CARTUCCIA3; break;
			case 8: fig = F_CARTUCCIA4; break;
			case 9: fig=0; break; 
		}
		if (fig) {
			delete mm_reserve(sizeof(Object));
			obj = new Object(mover->x,mover->y,fig,1,MVT_0);
			CLRFLAG(obj->mover->flags,OMF_UNWALKABLE|OMF_HITABLE);
			SETFLAG(obj->oflags,OFL_TAKEABLE);
			objectslist.put(obj);
			delete mm_reserve(sizeof(ObjMover));
			objectslist.put(new ObjMover(obj->getid(),angle,strength));
		} else
		if (whatin==9) {
			eventmanager.sendpos(EV_BOOM,mover->id,mover->x,mover->y,
								static_cast<fixed>(ArmList[ARM_BAZOOKA].power));
			delete mm_reserve(sizeof(FireBoom));
			objectslist.put(new FireBoom(mover->x,mover->y,mover->z,
										mover->angle,BOOM_MISSIL));
			quake=14;
		}
	   
	}
	 







	 






}
