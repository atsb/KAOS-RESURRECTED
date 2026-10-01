 






 

#include "std.hpp"
#include "fastmem.hpp"
#include "fixed.h"
#include "events.hpp"
#include "doors.hpp"
#include "3dengine.hpp"
#include "mm4.hpp"
#include "crcio.hpp"
#include "kaos.hpp"
#include "objects.hpp"
#include "lgraph.hpp"

#include "infobar.hpp"	 

TArm ArmList[MAXARMS] = {
	{AFL_ASCIA,		8, 36, -1, -1},
	{AFL_PISTOLA, 	7, 18, 20,100},
	{AFL_FUCILE,  	8, 36, 10, 80},
	{AFL_MITRA,   	6,  5, 48,200},
	{AFL_BAZOOKA,  20, 30,  2, 20},
	{AFL_BOMB,	   20, 30,  1, 20}
};

Mover::Mover(kaos_from_file src) {	 
	int handle = src.handle;
	 
	   
	   
	CRC_read(handle,&id,sizeof(id));
	CRC_read(handle,&x,sizeof(x));
	CRC_read(handle,&y,sizeof(y));
	CRC_read(handle,&z,sizeof(z));
	CRC_read(handle,&angle,sizeof(angle));
	CRC_read(handle,&shdim,sizeof(shdim));
	CRC_read(handle,&flags,sizeof(flags));
	CRC_read(handle,&index,sizeof(index));
	CRC_read(handle,&under,sizeof(under));
}

void Mover::save(int handle) {
	CRC_write(handle,&mover_type,sizeof(mover_type));
	CRC_write(handle,&id,sizeof(id));
	CRC_write(handle,&x,sizeof(x));
	CRC_write(handle,&y,sizeof(y));
	CRC_write(handle,&z,sizeof(z));
	CRC_write(handle,&angle,sizeof(angle));
	CRC_write(handle,&shdim,sizeof(shdim));
	CRC_write(handle,&flags,sizeof(flags));
	CRC_write(handle,&index,sizeof(index));
	CRC_write(handle,&under,sizeof(under));
}

void Mover::enqueue(int idx, std::int16_t &undr) {
	undr = IDMap[idx];
	IDMap[idx] = id;
	objmap[idx] += flags;
}

void Mover::dequeue(int idx, std::int16_t undr) {
	int top;

	if ((top = IDMap[idx]) == id)
		IDMap[idx] = undr;
	else {
		Mover *mvr;
		while ((top = (mvr = objectslist.get(top)->mover)->underq(idx)) != id);
		mvr->correctq(idx,undr);
	}
	objmap[idx] -= flags;
}

#pragma warn -par
void Mover::correctq(int idx, int undid) {
	under = undid;
}
#pragma warn +par

void Mover::place() {
	enqueue(index = xytoidx(x,y), under);
}

void Mover::remove() {
	dequeue(index, under);
}

char Mover::canmove(fixed nx, fixed ny) {
	int idx, top;
	Mover *mvr;
	fixed dist;
			 
	if ((nx >= (32l<<FIXSHIFT)) && (nx <= LMAX_X-(32l<<FIXSHIFT)) &&
		(ny >= (32l<<FIXSHIFT)) && (ny <= LMAX_Y-(32l<<FIXSHIFT))) {
		if (!ObjMap[idx = xytoidx(nx,ny)].unwalkable) return 1;
		if (map[idx]) return 0;
		top = IDMap[idx];
		while (top>=0) {
			mvr = objectslist.get(top)->mover;
			if (mvr->flags & OMM_UNWALKABLE) {
				dist = (FIXONE<<shdim)+(FIXONE<<mvr->shdim);
				 



				 
				if (LABS(mvr->x-nx)+LABS(mvr->y-ny) <= dist) return 0;  
			}
			top = mvr->underq(idx);
		}
		return 1;
	}
	return 0;
}

char Mover::move(int ang, int lshift) {
	ang = kaos_angidx(ang);
	fixed nx = x + kaos_fshl(costab[ang], lshift),
		  ny = y + kaos_fshl(sintab[ang], lshift);
	if (canmove(nx,ny)) {
		x = nx;
		y = ny;
		return 1;
	}
	return 0;
}

char Mover::fluidmove(int ang, fixed mult) {
	ang = kaos_angidx(ang);
	fixed nx = x + fixmul(costab[ang],mult),
		  ny = y + fixmul(sintab[ang],mult);
	if (canmove(nx,ny)) {
		x = nx;
		y = ny;
		return 1;
	}
	return 0;
}

char Mover1::move(int ang, int lshift) {
	ang = kaos_angidx(ang);
	fixed ca = costab[ang],
		  sa = sintab[ang],
		  nx = x + kaos_fshl(ca, lshift),
		  ny = y + kaos_fshl(sa, lshift),
		  dim = (FIXONE<<shdim),
		  vdim = dim-(FIXONE<<2);
	char moved = 0;
	if (ca>0) {if (canmove(nx+dim,y-vdim) && canmove(nx+dim,y+vdim)) {x=nx;moved++;}} else
	 if (ca<0) {if (canmove(nx-dim,y-vdim) && canmove(nx-dim,y+vdim)) {x=nx;moved++;}}
	if (sa>0) {if (canmove(x-vdim,ny+dim) && canmove(x+vdim,ny+dim)) {y=ny;moved++;}} else
	 if (sa<0) {if (canmove(x-vdim,ny-dim) && canmove(x+vdim,ny-dim)) {y=ny;moved++;}}
	if (moved && (!ca || !sa)) moved++;
	return moved;  
	 












}

char Mover1::fluidmove(int ang, fixed mult) {
	ang = kaos_angidx(ang);
	fixed ca = costab[ang],
		  sa = sintab[ang],
		  nx = x + fixmul(ca,mult),
		  ny = y + fixmul(sa,mult),
		  dim = (FIXONE<<shdim),
		  vdim = dim-(FIXONE<<2);
	char moved=0;
	if (ca>0) {if (canmove(nx+dim,y-vdim) && canmove(nx+dim,y+vdim)) {x=nx;moved++;}} else
	 if (ca<0) {if (canmove(nx-dim,y-vdim) && canmove(nx-dim,y+vdim)) {x=nx;moved++;}}
	if (sa>0) {if (canmove(x-vdim,ny+dim) && canmove(x+vdim,ny+dim)) {y=ny;moved++;}} else
	 if (sa<0) {if (canmove(x-vdim,ny-dim) && canmove(x+vdim,ny-dim)) {y=ny;moved++;}}
	if (moved && (!ca || !sa)) moved++;
	return moved;  
	 








		  













	 








}

Mover2::Mover2(kaos_from_file src)
	   :Mover1(src) {
	int handle = src.handle;
	CRC_read(handle,&index1,sizeof(index1));
	CRC_read(handle,&under1,sizeof(under1));
}

void Mover2::save(int handle) {
	Mover::save(handle);
	CRC_write(handle,&index1,sizeof(index1));
	CRC_write(handle,&under1,sizeof(under1));
}

void Mover2::place() {
	fixed cd = kaos_fshl(costab[kaos_angidx(angle)], shdim),
		 sd = kaos_fshl(sintab[kaos_angidx(angle)], shdim);
	index = xytoidx(x+cd,y+sd);
	index1 = xytoidx(x-cd,y-sd);
	enqueue(index,under);
	if (index1!=index) enqueue(index1,under1);
}

void Mover2::remove() {
	dequeue(index,under);
	if (index1!=index) dequeue(index1,under1);
}

int Mover2::underq(int idx) {
	if (idx == index) return under;
				 else return under1;
}

void Mover2::correctq(int idx, int undid) {
	if (idx == index) under = undid;
				 else under1 = undid;
}

 































 




























































 

































Object::Object(fixed px, fixed py, int startfig,
			   char numfig, byte movertype) {
	switch (movertype) {
		case MVT_0:
			delete mm_reserve(sizeof(Mover));
			mover = new Mover;
			break;
		case MVT_1:
			delete mm_reserve(sizeof(Mover1));
			mover = new Mover1;
			break;
		case MVT_2:
			delete mm_reserve(sizeof(Mover2));
			mover = new Mover2;
			break;
		 





		default: error("Object","invalid Mover");
	}
	if (mover == NULL) error("Object","no Mover");
	mover->x = px;
	mover->y = py;
	mover->z = 0l;
	 
	mover->angle = 0;
	mover->shdim = 4;
	oflags = 0;
	selflight = 0;
	changefig(startfig,numfig);
	object_type = OBJT_OBJECT;
}

Object::Object(int handle) { 
	byte movertype;

	CRC_read(handle,&movertype,sizeof(movertype));
	switch (movertype) {
		case MVT_0:
			delete mm_reserve(sizeof(Mover));
			mover = new Mover(kaos_from_file(handle));
			break;
		case MVT_1:
			delete mm_reserve(sizeof(Mover1));
			mover = new Mover1(kaos_from_file(handle));
			break;
		case MVT_2:
			delete mm_reserve(sizeof(Mover2));
			mover = new Mover2(kaos_from_file(handle));
			break;
		 





		default: error("Object","invalid Mover");
	}
	if (mover == NULL) error("Object","no Mover");
	mover->mover_type = movertype;	 
	CRC_read(handle,&oflags,sizeof(oflags));
	CRC_read(handle,&selflight,sizeof(selflight));
	CRC_read(handle,&figstart,sizeof(figstart));
	CRC_read(handle,&fignum,sizeof(fignum));
}

void Object::save(int handle) {
	CRC_write(handle,&object_type,sizeof(object_type));
	mover->save(handle);  
	CRC_write(handle,&oflags,sizeof(oflags));
	CRC_write(handle,&selflight,sizeof(selflight));
	CRC_write(handle,&figstart,sizeof(figstart));
	CRC_write(handle,&fignum,sizeof(fignum));
}

void Object::changefig(int startfig, char numfig) {
	if (startfig+numfig-1 < MAXFIGURES) {
	  if (numfig<0) {
		oflags |= OFL_REVERSE;
		numfig = -numfig;
	  } else oflags &= ~OFL_REVERSE;
	  if ((numfig==16) || (numfig==8) ||
		  (numfig==4) || (numfig==2) || (numfig<=1)) {
		figstart = startfig;
		fignum = numfig;
	  } else error("Object::changefig","views must be powers of 2");
	} else error("Object::changefig","out of range");
}

int Object::getviewfig(int ang) {
	int displ = ANG360 / fignum;
	if (fignum > 1) {
		int myfig = (ang-mover->angle+(displ >> 1));
		if (myfig<ANG0) myfig+=ANG360; else
		 if (myfig>=ANG360) myfig-=ANG360;
		if (oflags & OFL_REVERSE) {
			if ((myfig = (myfig/displ)&(fignum-1))>(fignum>>1)) {
				myfig |= 0x4000;
				myfig = fignum-myfig;
			}
			return figstart+myfig;
		} else return figstart+((myfig/displ)&(fignum-1));
	} else
		return figstart;
}

void Object::handle_event(TEvent &event) {
int arctan[8][8] = {
{ANG45,ANG30,ANG30, ANG0, ANG0, ANG0, ANG0, ANG0},
{ANG60,ANG45,ANG30,ANG30,ANG30, ANG0, ANG0, ANG0},
{ANG60,ANG60,ANG45,ANG45,ANG30,ANG30,ANG30, ANG0},
{ANG90,ANG60,ANG45,ANG45,ANG45,ANG30,ANG30,ANG30},
{ANG90,ANG60,ANG60,ANG45,ANG45,ANG45,ANG45,ANG30},
{ANG90,ANG90,ANG60,ANG60,ANG45,ANG45,ANG45,ANG45},
{ANG90,ANG90,ANG60,ANG60,ANG45,ANG45,ANG45,ANG45},
{ANG90,ANG90,ANG60,ANG60,ANG60,ANG45,ANG45,ANG45}};
	switch (event.what) {
		case EV_GOT: eventmanager.sendcom(EV_KILL,mover->id); break;
		case EV_BOOM:
			fixed fdx, fdy;
			int dx = lshr16(fdx = mover->x-event.data.pos.x),
				dy = lshr16(fdy = mover->y-event.data.pos.y),
				 
				ang = 0;
			int power = fixdiv(fixdist(fdx,fdy),20l<<16);
			if (power) power = event.data.pos.z/power;
				  else power = event.data.pos.z;
			 
			if (power<4 || lookray(mover->x,mover->y,
						event.data.pos.x,event.data.pos.y,OMM_SOUNDABLE)) break;
			 
			if (!dx) if (dy>=0) ang=ANG90; else ang=ANG270; else
			if (!dy) if (dx>=0) ang=ANG0; else ang=ANG180; else {
				int d1x = dx, d1y = dy;
				if (dx>0) {if (--dx>7) dx=7;} else
						  {if (++dx<-7) dx=-7;}
				if (dy>0) {if (--dy>7) dy=7;} else
						  {if (++dy<-7) dy=-7;}
				ang = arctan[ABS(dy)][ABS(dx)];
				if (d1x<0) ang=ANG180-ang;
				if (d1y<0) ang=ANG360-ang;
				if (ang<ANG0) ang+=ANG360; else
				 if (ang>=ANG360) ang-=ANG360;
			}
			eventmanager.senddirect(EV_HIT,event.source,mover->id,ang,power);
			eventmanager.senddirect(EV_ADDSTAT,mover->id,event.source,
									STT_TECNO,1);
			if (GETFLAG(gameflags,GFL_SHOWBLOOD)) {
				Object *obj;
				if (doblood(ang))
					eventmanager.senddirect(EV_ADDSTAT,mover->id,event.source,
											STT_BLOOD,5);
				doblood(angleadd(ang,ANG45));
				doblood(anglesub(ang,ANG45));
				doblood(angleadd(ang,ANG45+ANG90));
				doblood(anglesub(ang,ANG45+ANG90));
			}
			break;
	}
}

Object::~Object() {
	if (!ShutDown) mover->remove();
	delete mover;
}

Actor::Actor(int handle)
	  :Object(handle) {  
	CRC_read(handle,&animcount,sizeof(animcount));
	CRC_read(handle,&health,sizeof(health));
}

void Actor::save(int handle) {
	Object::save(handle);
	CRC_write(handle,&animcount,sizeof(animcount));
	CRC_write(handle,&health,sizeof(health));
}

void Actor::setid(int the_id) {
	Object::setid(the_id);
	makeActor();
};

char Actor::makeActor() {
	if (!(oflags & OFL_ISACTOR)) {
		actorslist.add(mover->id);
		oflags |= OFL_ISACTOR;
		return 1;
	}
	return 0;
}

char Actor::makeObject() {
	 
	if (oflags & OFL_ISACTOR) {
		if (eventmanager.sendcom(EV_ERASEACT,mover->id)) {
			oflags &= ~OFL_ISACTOR;
			return 1;
		}
	}
	return 0;
}

char Actor::nearPlayer(fixed maxdist) {
	return (is_nearPlayer(mover->x,mover->y,maxdist) >= 0);
}

 








int Actor::getviewfig(int ang) {
	int displ = ANG360 / fignum;
	if (fignum > 1) {
		int myfig = (ang-mover->angle+(displ >> 1));
		if (myfig<ANG0) myfig+=ANG360; else
		 if (myfig>=ANG360) myfig-=ANG360;
		if (oflags & OFL_REVERSE) {
			if ((myfig = (myfig/displ)&(fignum-1))>(fignum>>1)) {
				myfig |= 0x4000;
				myfig = fignum-myfig;
			}
			return figstart+(animcount*((fignum>>1)+1))+myfig;
		} else return figstart+(animcount*fignum)+((myfig/displ)&(fignum-1));
	} else
		return figstart+animcount;
}

 










void Actor::draw(int vidx, int vidy, char &cd, char &fd) {
	drawview(vidx,vidy,mover->x,mover->y,mover->z,
			 mover->angle,0,mover->id,cd,fd,0);
}

void Actor::virtualdraw() {
	virtualview(mover->x,mover->y,mover->angle,mover->id);
}

void Actor::transf(int vidx, int vidy) {
	 



	ftransfblock(vidx-halfwidth,vidy-halfheight,viewwidth,viewheight,VMEMPTR);
}

Actor::~Actor() {
	if (!ShutDown) actorslist.erase(mover->id);
	 
}

LightActor::LightActor(int handle)
		   :Actor(handle) {	  
	CRC_read(handle,&lightrange,sizeof(lightrange));
	if (GETFLAG(oflags,OFL_HASLIGHT)) {
		CRC_read(handle,&lightz,sizeof(lightz));
		CRC_read(handle,&lightpow,sizeof(lightpow));
	}
}

void LightActor::save(int handle) {
	Actor::save(handle);
	CRC_write(handle,&lightrange,sizeof(lightrange));
	if (GETFLAG(oflags,OFL_HASLIGHT)) {
		CRC_write(handle,&lightz,sizeof(lightz));
		CRC_write(handle,&lightpow,sizeof(lightpow));
	}
}

void LightActor::lights() {
	if (!lightrange) return;

	int i,j,k;
	int bx = lshr16(mover->x) >> 6,
		by = lshr16(mover->y) >> 6,
		lr2 = (lightrange<<1)-1;
	char *map1 = &(LightMap[bxytoidx(bx-lightrange,by-lightrange)]),
		 *map2;

	for (i=-lightrange;i<=lightrange;i++,map1+=MAP_DX)
	  if ((bx+i) >= 0 && (bx+i) < MAP_DX )
		for (j=-lightrange,map2=map1;j<=lightrange;j++,map2++)
		  if ((by+j) >= 0 && (by+j) < MAP_DY )
			if ((k = lr2-(ABS(i)+ABS(j))) > 0) *map2 += k;
}

void LightActor::unlights() {
	if (!lightrange) return;

	int i,j,k;
	int bx = lshr16(mover->x) >> 6,
		by = lshr16(mover->y) >> 6,
		lr2 = (lightrange<<1)-1;
	char *map1 = &(LightMap[bxytoidx(bx-lightrange,by-lightrange)]),
		 *map2;

	for (i=-lightrange;i<=lightrange;i++,map1+=MAP_DX)
	  if ((bx+i) >= 0 && (bx+i) < MAP_DX )
		for (j=-lightrange,map2=map1;j<=lightrange;j++,map2++)
		  if ((by+j) >= 0 && (by+j) < MAP_DY )
			if ((k = lr2-(ABS(i)+ABS(j))) > 0) *map2 -= k;
}

void LightActor::place() {
	mover->place();
	lights();
}

void LightActor::remove() {
	mover->remove();
	unlights();
}

LightActor::~LightActor() {
	if (!ShutDown) unlights();
}

char CanFireActor::setarm(byte armtype) {
	if (armtype == curarm) return 1;
	if (GETFLAG(armflag,ArmList[armtype].flag) && !armcharge) {
		if (armchanging<(ARMCHGSPEED/2)) {
			oldarm = curarm;
			armchanging = ARMCHGSPEED-armchanging;
		}
		curarm = armtype;
		return 1;
	}
	return 0;
}

 
char CanFireActor::fire(int angle) {
	if (!armcharge && !armchanging &&
		(!curarm || Armbull[curarm-1]>0)) {
		armcharge = ArmList[curarm].chargetime;
		if (curarm) Armbull[curarm-1]--;
		return 1;
	}
	return 0;
}
 

CanFireActor::CanFireActor(fixed px, fixed py, int startfig,
							char numfig, byte movertype)
			 :LightActor(px,py,startfig,numfig,0,0,movertype),
			  curarm(0), oldarm(0),
			  armcharge(0), armchanging(0), armflag(0)
{	object_type=OBJT_CANFIREACTOR;
	fwfill(Armbull,0,sizeof(Armbull));
}

CanFireActor::CanFireActor(int handle)
			 :LightActor(handle) {  
	CRC_read(handle,&armflag,sizeof(armflag));
	CRC_read(handle,&curarm,sizeof(curarm));
	CRC_read(handle,&armcharge,sizeof(armcharge));
	CRC_read(handle,Armbull,sizeof(Armbull));
	armchanging=0;  
}

void CanFireActor::save(int handle) {
	LightActor::save(handle);
	CRC_write(handle,&armflag,sizeof(armflag));
	CRC_write(handle,&curarm,sizeof(curarm));
	CRC_write(handle,&armcharge,sizeof(armcharge));
	CRC_write(handle,Armbull,sizeof(Armbull));
}

void CanFireActor::animate() {
	 
	if (armchanging) {
		armchanging--;
	} else {
		 
		if (curarm) {
			lightrange = (armcharge+8)>>4;	 
			if (!armcharge && !Armbull[curarm-1]) {
				byte old = curarm;
				byte app = curarm;
				do {
					while (app && !Armbull[app-1]) app--;
				} while (!setarm(app--));
				 
				 
				if (old!=curarm)
					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);
			}
		}
		if (armcharge) armcharge--;
	  }
}

char CanFireActor::takearm(byte armtype) {
	TArm &arm = ArmList[armtype];
	byte token = 0;
	if (!GETFLAG(armflag,arm.flag)) {
		SETFLAG(armflag,arm.flag);
		token |= 2;
		 
		if (curarm<armtype) setarm(armtype);
	}
	token |= takeammo(armtype,arm.pre_ammo);
	return token;
}

char CanFireActor::takeammo(byte armtype, int ammo) {
	TArm &arm = ArmList[armtype];
	if (Armbull[armtype-1] < arm.max_ammo) {
		if ((Armbull[armtype-1] += ammo) > arm.max_ammo)
			Armbull[armtype-1] = arm.max_ammo;
		if ( curarm<armtype) setarm(armtype);
		return 1;
	}
	return 0;
}

ObjectsList::ObjectsList() {
	objnum = 0;
	fdfill(&objectslist,0l,MAXOBJECTS * sizeof(Object *));
}

void ObjectsList::loadobj(Object *object,byte objtype) {
	objlist[object->mover->id]=object;
	if ((object->oflags & OFL_ISACTOR) == OFL_ISACTOR)
		actorslist.add(object->mover->id);
	object->object_type=objtype;	 
	objnum++;
}

void ObjectsList::save(int handle) {
	int objid=0, num=objnum;
	while (num) {
		if (objlist[objid] != NULL) {
			objlist[objid]->save(handle);
			num--;
		}
		objid++;
	}
	objid = OBJT_END;
	CRC_write(handle,&objid,1);	 
}

void ObjectsList::put(Object *object) {
	if (object == NULL) error("ObjectsList::put","no Object");
	if (objnum < MAXOBJECTS) {
		int i = 0;
		while (objlist[i] != NULL) i++;
		objlist[i] = object;
		object->setid(i);
		object->place();	 
		objnum++;
	} else delete object;	 
	 
}

Object *ObjectsList::get(int objidx) {
	if (objidx < MAXOBJECTS)
		return objlist[objidx];
	return NULL;
}

void ObjectsList::erase(int objidx) {
	if (objidx < MAXOBJECTS)
		if (objlist[objidx] != NULL) {
			delete objlist[objidx];
			objlist[objidx] = NULL;
			objnum--;
		}
}

void ObjectsList::eraseall() {
	int objidx=0;
	while (objnum) {
		if (objlist[objidx] != NULL) {
			delete objlist[objidx];
			objlist[objidx] = NULL;
			objnum--;
		}
		objidx++;
	}
}

 









void ObjectsList::handle_event(TEvent &event) {
	for (int i=0, j=0; j<objnum, i<MAXOBJECTS; i++)
		if (objlist[i] != NULL) {
			objlist[i]->handle_event(event);
			j++;
		}
}

void ObjectsList::preloadall() {
	int k;
	for (int i=0, j=0; j<objnum, i<MAXOBJECTS; i++)
		if (objlist[i] != NULL) {
			Object &obj = *objlist[i];
			if (obj.fignum>0) k=obj.fignum;
			 else k=(obj.fignum>>1)+1;
			for(;k--;) mm_recall(FigList[obj.figstart+k].memidx);
			j++;
		}
}

ObjectsList::~ObjectsList() {
	int i=0;
	while (objnum) {
		while (!objlist[i]) i++;
		erase(i++);
	}
}

void ActorsList::add(int objidx) {
	register int i;
	if (actorsnum < MAXACTORS) {
		i=0;
		while ((i<actorsnum) && (actorslist[i] != objidx)) i++;
		if (i == actorsnum)
			actorslist[actorsnum++] = objidx;
		 
	} else error("ActorList::add","too many Actors");
	 




}

void ActorsList::erase(int objidx) {
	for (int i=0; i<actorsnum; i++)
		if (actorslist[i] == objidx) {
			actorslist[i] = actorslist[--actorsnum];
			return;
		}
}

void ActorsList::eraseall() {
	actorsnum=0;
}

void ActorsList::animate() {
	for (int i=0; i<actorsnum; i++)
		((Actor *)objectslist.get(actorslist[i]))->animate();
}

void ActorsList::handle_event(TEvent &event) {
	for (int i=0; i<actorsnum; i++)
		objectslist.get(actorslist[i])->handle_event(event);
}

void ActorsList::reset() {
	 
	for (register int i=0;i<actorsnum;i++)
		objectslist.objlist[actorslist[i]]->reset();
}

void DrawList::add(int objid, fixed objdist) {
	int i;
	if (drawnum < MAXDRAWING) {
		i = drawnum++;
		while (i && distlist[i-1]>objdist) i--;
		if (drawnum-i > 1) {
			fodmove(&drawlist[i+1],&drawlist[i],(drawnum-i-1)*sizeof(objid));
			fodmove(&distlist[i+1],&distlist[i],(drawnum-i-1)*sizeof(objdist));
		}
		 






		drawlist[i] = objid;
		distlist[i] = objdist;
	}  
}

int DrawList::get(fixed &objdist) {
	if (drawnum) {
		objdist = distlist[--drawnum];
		return drawlist[drawnum];
	} else return -1;
}