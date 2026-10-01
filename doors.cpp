 






 

#include "std.hpp"
#include "fastmem.hpp"
#include "events.hpp"
#include "doors.hpp"

#include "mm4.hpp"		 
#include "crcio.hpp"	 
#include "kaos.hpp"

#include "fixed.h"
#include "3dengine.hpp"

#include "3dsound.hpp"

#include "messages.hpp"

#define	MAXDOORS		96
#define	MAXFLDOORS		24	 
#define MAXDOORWAIT		80

 
#define	DST_OPENING		0
#define	DST_WAITING		1
#define	DST_CLOSING		2

#define DOORFLAGS 	OMF_STANDARD

DoorManager::DoorManager() {
	doorlist = new byte[MAXDOORS];
	doortype = new byte[MAXDOORS];
	fldoorlist = new TDoor[MAXFLDOORS];
	doornum = 0;
	fldoornum = 0;
	 
}

void DoorManager::load(int handle) {
	CRC_read(handle,&doornum,sizeof(doornum));
	CRC_read(handle,doorlist,MAXDOORS);
	CRC_read(handle,doortype,MAXDOORS);
	CRC_read(handle,&fldoornum,sizeof(fldoornum));
	CRC_read(handle,fldoorlist,fldoornum*sizeof(TDoor));
}

void DoorManager::save(int handle) {
	CRC_write(handle,&doornum,sizeof(doornum));
	CRC_write(handle,doorlist,MAXDOORS);
	CRC_write(handle,doortype,MAXDOORS);
	CRC_write(handle,&fldoornum,sizeof(fldoornum));
	CRC_write(handle,fldoorlist,fldoornum*sizeof(TDoor));
}

int DoorManager::add(byte image) {
	if (doornum < MAXDOORS) {
		doortype[doornum] = image;
		doorlist[doornum] = 0;
		return doornum++;
	} else error("DoorList::add","too many doors");
}

void DoorManager::makeslots() {
	TMapInfo *mapptr = Map+65;
	byte m;
	for (int i=0; i<3966  ; i++, mapptr++) {
		if ((m = mapptr->type) == MTP_DOOR1 ||
			m == MTP_DOOR2) {
			mapptr-=64; if (mapptr->stop && mapptr->type == MTP_WALL) mapptr->type = MTP_SLOT;
			mapptr+=63; if (mapptr->stop && mapptr->type == MTP_WALL) mapptr->type = MTP_SLOT;
			mapptr+= 2; if (mapptr->stop && mapptr->type == MTP_WALL) mapptr->type = MTP_SLOT;
			mapptr+=63; if (mapptr->stop && mapptr->type == MTP_WALL) mapptr->type = MTP_SLOT;
			mapptr-=64;
		}
	}
}

char DoorManager::open(int idx, byte key) {
	int i, index;
	byte t;

	t = Map[idx].type;
	if (t == MTP_DOOR1 || t == MTP_DOOR2) {
	 i = doorlist[index = Map[idx].data];
	 if (i || fldoornum<MAXFLDOORS) {
		if (!i) {
			t = doortype[index];
			if (t == T_DOORY && !(key & LCKT_L1)) return -2;
			if (t == T_DOORR && !(key & LCKT_L2)) return -3;
			if (t==T_DOORY || t==T_DOORR)
				play3Dsound(lshl16((idx & 0x7fc0)+32),
							lshl16(((idx & 63)<<6)+32),SND_UNLOCK,
							0,SYSOWN,SFL_NORMAL);
			TDoor &door = fldoorlist[fldoornum];
			door.idx = idx;
			door.opennum = 0;
			door.animcount = 0;
			door.status = DST_OPENING;
			doorlist[index] = ++fldoornum;
		} else {
			 
			if (fldoorlist[--i].status == DST_CLOSING) {
				fldoorlist[i].status = DST_OPENING;
			} else return -1;
		  }
		return 1;
	 }
	}
	return 0;
}

void DoorManager::animate() {
	int i, j;

	for (i=0;i<fldoornum;i++) {
		TDoor &door = fldoorlist[i];
		if (door.status == DST_OPENING) {
			if (!door.opennum)
				play3Dsound(lshl16((door.idx & 0x7fc0)+32),
							lshl16(((door.idx & 63)<<6)+32),SND_PORTA1,
							0,SYSOWN,SFL_NORMAL);
			if ((door.opennum += 4) >= 64) {
				door.opennum = 64;
				door.status = DST_WAITING;
				door.animcount = 0;
				objmap[door.idx] -= DOORFLAGS;
				play3Dsound(lshl16((door.idx & 0x7fc0)+32),
							lshl16(((door.idx & 63)<<6)+32),SND_CLOSE,
							0,SYSOWN,SFL_NORMAL);
			}
		} else
		if (door.status == DST_CLOSING) {
			if ((door.opennum -= 4) <= 0) {
				play3Dsound(lshl16((door.idx & 0x7fc0)+32),
							lshl16(((door.idx & 63)<<6)+32),SND_CLOSE,
							0,SYSOWN,SFL_NORMAL);
				doorlist[Map[door.idx].data] = 0;
				if (i != --fldoornum) {
					door = fldoorlist[fldoornum];
					 
					doorlist[Map[door.idx].data] = i+1;
				}
				i--;
			}
		} else
		if (door.status == DST_WAITING) {
			if (door.animcount >= MAXDOORWAIT) {
				 
				 
				if (IDMap[door.idx]<0) {
					door.status = DST_CLOSING;
					objmap[door.idx] += DOORFLAGS;
					play3Dsound(lshl16((door.idx & 0x7fc0)+32),
							lshl16(((door.idx & 63)<<6)+32),SND_PORTA2,
							0,SYSOWN,SFL_NORMAL);
				}
			} else ++door.animcount;
		}
	}
}

int DoorManager::doorpos(int indx) {
	register int i;
	if ((i = doorlist[indx]) != 0) return fldoorlist[i-1].opennum;
	return 0;
}

void DoorManager::eraseall() {
	doornum=0; fldoornum=0;
}

void DoorManager::handle_event(TEvent &event) {
	switch (event.what) {
		case EV_ACTIVATE:
			open(event.data.msg.x,event.data.msg.y);
			break;
		case EV_PLACTIVATE:
			int msg = -1;
			switch (open(event.data.msg.x,event.data.msg.y)) {
			case -3: msg = MSG_NEEDKEY_RED; break;
			case -2: msg = MSG_NEEDKEY_YELLOW; break;
			}
			if (msg>=0) eventmanager.sendcom(EV_MESSAGE,msg);
			break;
		 













	}
}