#include "std.hpp"
#include "fastmem.hpp"
#include "fixed.h"
#include "3dengine.hpp"
#include "key.hpp"
#include "mm4.hpp"
#include "crcio.hpp"
#include "events.hpp"
#include "kaos.hpp"
#include "doors.hpp"
#include "objects.hpp"
#include "objects2.hpp"
#include "player.hpp"
#include "3dsound.hpp"
#include "lgraph.hpp"
#include "infobar.hpp"
#include "messages.hpp"
#include <stdlib.h>		 
#include <cstdio>
#include <cstdint>

#include "platform_compat.hpp"

static word packcount;

Player::Player(fixed px, fixed py, int ang, byte cntrlnum)
	   :CanFireActor(px,py,F_PLAYER,8,MVT_2)
{
	object_type = OBJT_PLAYER;
	control = PControl[controlnum = cntrlnum];
	mover->angle = ang;
	mover->z = 32l<<FIXSHIFT;
	speed = 0;
	angspeed = 0;
	latspeed = 0;
	angwalk = 0;
	health = 100;
	if (GETFLAG(gameflags,GFL_COMPETITIVE)) keys = LCKT_L1|LCKT_L2;
										else keys = 0;
	 
	animtrig=0;
	mover->shdim = 4;  
	SETFLAG(armflag,AFL_ASCIA);
	takearm(ARM_PISTOLA);
	 
	 
	 
	hitspeed = 0;
	hitangle = 0;
	vangle = 0;
	vangang = 0;
	pflags = 0;
	cecita = 0;
	hardcount = 0;
	 
	handang = ANG0;
	 
	upwalk=0;
	fwfill(myseenmap,0,MAPDIM);
	setcomtype(COM_DIRECT);
	fires=0;hits=0;blood=0;kills=0;tecno=0;
	plkilled = 0;
	eventmanager.sendcom(EV_MESSAGE,MSG_GOT_PISTOLA);
	eventmanager.sendcom(EV_MESSAGE,MSG_GETANTIKAOS);
}

Player::Player(int handle)
	   :CanFireActor(handle) {
	CRC_read(handle,&controlnum,sizeof(controlnum));
	CRC_read(handle,&keys,sizeof(keys));
	 
	 
	CRC_read(handle,&cecita,sizeof(cecita));
	 
	CRC_read(handle,&hardcount,sizeof(hardcount));
	CRC_read(handle,myseenmap,MAPDIM);
	CRC_read(handle,&fires,sizeof(fires));
	CRC_read(handle,&hits,sizeof(hits));
	CRC_read(handle,&blood,sizeof(blood));
	CRC_read(handle,&kills,sizeof(kills));
	CRC_read(handle,&tecno,sizeof(tecno));
	CRC_read(handle,&plkilled,sizeof(plkilled));
	speed=0; angspeed=0; latspeed=0;
	angwalk=0; animtrig=0; hitspeed=0;
	pflags=0;
	vangle=ANG0; vangang=ANG0;
	handang=ANG0;
	control = PControl[controlnum];
	upwalk=0;
	setcomtype(COM_DIRECT);
#ifdef __SHAREWARE__
	CLRFLAG(armflag,AFL_BOMB);
	if (curarm == ARM_BOMB)
		curarm = ARM_ASCIA;
#endif
}

Player::Player(fixed px, fixed py, int ang, TPlayerData &data,
			   byte cntrlnum)	 
	   :CanFireActor(px,py,F_PLAYER,8,MVT_2) {
	object_type = OBJT_PLAYER;
	control = PControl[controlnum = cntrlnum];
	mover->angle = ang;
	mover->z = 32l<<FIXSHIFT;
	mover->shdim = 4;  
	speed = 0; angspeed = 0; latspeed = 0;
	angwalk = 0; animtrig=0; handang = ANG0;
	hitspeed = 0; hitangle = 0;
	vangle = 0; vangang = 0;
	cecita = 0;
	hardcount=0;
	pflags = 0;
	upwalk=0;
	 
	health = data.health;
	curarm = data.curarm;
	if (GETFLAG(gameflags,GFL_COMPETITIVE)) keys = data.keys;
									   else keys = 0;
	armflag = data.armflag;
	fwmove(Armbull,data.armbull,sizeof(Armbull));
	setcomtype(COM_DIRECT);
	fires=0;hits=0;blood=0;kills=0;tecno=0;
	plkilled=0;
	 
	if (health<=0) {
		health = -1;
		hwalk = 4l<<FIXSHIFT;
		cecita=((32-lshr16(hwalk))/5+1)<<5;
		vangle=anglesub(ANG0,(lshr16(hwalk)-32)*ANG30>>5);
	}
}

void Player::setcomtype(char type) {
	comtype = type;
	oldpack=0x8000;	 
	packcount=0;
}

void Player::save(int handle) {
	CanFireActor::save(handle);
	CRC_write(handle,&controlnum,sizeof(controlnum));
	CRC_write(handle,&keys,sizeof(keys));
	 
	 
	CRC_write(handle,&cecita,sizeof(cecita));
	 
	CRC_write(handle,&hardcount,sizeof(hardcount));
	CRC_write(handle,myseenmap,MAPDIM);
	CRC_write(handle,&fires,sizeof(fires));
	CRC_write(handle,&hits,sizeof(hits));
	CRC_write(handle,&blood,sizeof(blood));
	CRC_write(handle,&kills,sizeof(kills));
	CRC_write(handle,&tecno,sizeof(tecno));
	CRC_write(handle,&plkilled,sizeof(plkilled));
	 
	if (health<=0) {
		health = -1;
		hwalk = 4l<<FIXSHIFT;
		cecita=((32-lshr16(hwalk))/5+1)<<5;
		vangle=anglesub(ANG0,(lshr16(hwalk)-32)*ANG30>>5);
	}
}

void Player::draw(int vidx, int vidy, char &cd, char &fd) {
	int k;
	if (MAPVIEW) {
		virtualview(mover->x,mover->y,mover->angle,mover->id);
		updatemap(myseenmap);
		drawmap(vidx,vidy,mover->x,mover->y,mover->angle,myseenmap);
	} else {
		k = cecita>>5;
		drawview(vidx,vidy,mover->x,mover->y,hwalk,
				mover->angle,vangle,mover->id,cd,fd,k);
		updatemap(myseenmap);
		if (health>0) {
			int arm, cfact;
			if (armchanging<(ARMCHGSPEED/2)) {
				arm = curarm;
				cfact = -armchanging;
			} else {
				arm = oldarm;
				cfact = armchanging-ARMCHGSPEED;
			  }
			switch(arm) {
			case ARM_ASCIA:	 
				if (armcharge<12)
					drawhand(PlayerAscia[0],50,115,-30,(cfact*115)/18,115,handang,k);
				else
				if (armcharge>28)
					drawhand(PlayerAscia[1],85,150,-40-12,0,150,handang,k);
				else
					drawhand(PlayerAscia[2],106,123,4,0,123,handang,k);
				break;
			case ARM_PISTOLA:	 
				if (armcharge<6)
					drawhand(PlayerPist[0],55,61,23,(cfact*61)/18,61,handang,k);
				else
				if (armcharge>14) {
					drawhand(PlayerPist[2],42,39,22,47,61,handang,k);
					drawhand(PlayerPist[0],55,61,23,0,61,handang,k);
				} else
					drawhand(PlayerPist[1],55,67,21,-10,67,handang,k);
				break;
			case ARM_FUCILE:	 
				if (armcharge<13)
					drawhand(PlayerFuc[0],65,53,37,(cfact*53)/18,53,handang,k);
				else
				if (armcharge>30) {
					drawhand(PlayerFuc[2],40,31,20,41,53,handang,k);
					drawhand(PlayerFuc[0],65,53,37,0,53,handang,k);
				} else
				if (armcharge>26)
					drawhand(PlayerFuc[0],65,53,37,0,53,handang,k);
				else
				 
					drawhand(PlayerFuc[1],99,108,62,0,108,handang,k);
				break;
			case ARM_MITRA:	 
				if (armcharge>2 && armcharge<5)
					drawhand(PlayerMitr[4],42,41,21,64,77,handang,k);
				drawhand(PlayerMitr[0],53,77,33,(cfact*77)/18,77,handang,k);
				switch(armcharge) {
				case 0: break;
				case 5:
				case 1:
					drawhand(PlayerMitr[1],53,31,33,46,77,handang,k);
					break;
				case 4:
				case 2:
					drawhand(PlayerMitr[2],53,31,33,46,77,handang,k);
					break;
				default:
					drawhand(PlayerMitr[3],53,31,33,46,77,handang,k);
				}
				if (armcharge)
					switch(random(2)) {
					case 0: handang=angleadd(handang,random(ANG30)); break;
					case 1: handang=anglesub(handang,random(ANG30)); break;
					}
				break;
			case ARM_BAZOOKA:
				int app = ArmList[ARM_BAZOOKA].chargetime;
				if (armcharge && !armchanging) {
					if (app-armcharge <= app>>2)
						cfact = (armcharge-app)*6/(app>>2); 	 
					else
					if (armcharge >= app>>2)
						cfact = ((app>>2)-armcharge)*6/(app>>1);	 
					else cfact=0;
				}
				drawhand(PlayerBaz,83,68,41,(cfact*68)/18,68,handang,k);
				break;
			}
		}
	  }
	k = 0;
	if (health>100) {
		putlfig(vidx-(viewwidth>>1),vidy+(viewheight>>1)-16,16,16,mm_recall(minibox_I));
		k+=16;
	}
	if (hardcount>0) putlfig(vidx-(viewwidth>>1)+k,vidy+(viewheight>>1)-16,16,16,mm_recall(minibox_F));
}

void Player::transf(int vidx, int vidy) {
	if (viewwidth==320) ftransfarea(0,168,VMEMPTR);  
	else ftransfblock(vidx-halfwidth,vidy-halfheight,viewwidth,viewheight,VMEMPTR);
}

void Player::updatecommands() {
	command.up    	= Key[control.up];
	command.down  	= Key[control.down];
	command.left	= Key[control.left];
	command.right 	= Key[control.right];
	command.open    = kb_pressed(control.open) || (controlnum == 0 && kb_mouse_right());
	command.fire    = kb_pressed(control.fire) || (controlnum == 0 && kb_mouse_left());
	command.fast    = Key[control.fast];
	command.strafe	= Key[control.strafe];
	command.chgarm  = kb_onepressed(control.chgarm);
	command.lookup  = Key[control.lookup];
	command.lookmid	= Key[control.lookmid];
	command.lookdown= Key[control.lookdown];
	command.map	  	= kb_onepressed(control.map);
}

void Player::savepack(word pack) {
	if (Key[KB_F5]) {
		if (packcount) {
			packcount |= 0x8000;	 
			CRC_write(playbackhandle,&packcount,sizeof(packcount));
		}
		pack = 0x8000;	 
		CRC_write(playbackhandle,&pack,sizeof(pack));
		eventmanager.sendcom(EV_ENDPLAYBACK,0);
		setcomtype(COM_DIRECT);
	} else
	if (pack==0x8000) {	 
		if (packcount) {
			packcount |= 0x8000;	 
			CRC_write(playbackhandle,&packcount,sizeof(packcount));
		}
		 
	} else
	if (pack!=oldpack || packcount>32700) {
		if (packcount) {
			packcount |= 0x8000;	 
			CRC_write(playbackhandle,&packcount,sizeof(packcount));
		}
		packcount=0;
		oldpack = pack;
		CRC_write(playbackhandle,&pack,sizeof(pack));
	} else packcount++;
}

word Player::loadpack() {
	word pack = 0x8000;
	if (packcount<=1) {
	   
	  CRC_read(playbackhandle,&pack,sizeof(pack));
	  if (pack != 0x8000) {
		if (pack & 0x8000) packcount=pack & 0x7fff;
		else {
			oldpack=pack;
			packcount=0;
			return pack;
		}
	  } else {
		  eventmanager.sendcom(EV_ENDPLAYBACK,0);
		  setcomtype(COM_DIRECT);
		  return 0;
		}
	} else packcount--;
	return oldpack;
}

 






































word Player::setpackbit(byte bit, byte val) {
	if (val) return (word)1<<bit;
		else return 0;
}

char Player::getpackbit(word pack, byte bit) {
	return (pack & ((word)1<<bit)) != 0;
}

word Player::packcom() {
	return setpackbit(0,command.up) |
		   setpackbit(1,command.down) |
		   setpackbit(2,command.left) |
		   setpackbit(3,command.right) |
		   setpackbit(4,command.open) |
		   setpackbit(5,command.fire) |
		   setpackbit(6,command.fast) |
		   setpackbit(7,command.strafe) |
		   setpackbit(8,command.chgarm) |
		   setpackbit(9,command.lookup) |
		   setpackbit(10,command.lookmid) |
		   setpackbit(11,command.lookdown) |
		   setpackbit(12,command.map);
}

void Player::unpackcom(word pack) {
	command.up 		= getpackbit(pack,0);
	command.down 	= getpackbit(pack,1);
	command.left 	= getpackbit(pack,2);
	command.right 	= getpackbit(pack,3);
	command.open 	= getpackbit(pack,4);
	command.fire 	= getpackbit(pack,5);
	command.fast 	= getpackbit(pack,6);
	command.strafe 	= getpackbit(pack,7);
	command.chgarm 	= getpackbit(pack,8);
	command.lookup 	= getpackbit(pack,9);
	command.lookmid	= getpackbit(pack,10);
	command.lookdown= getpackbit(pack,11);
	command.map 	= getpackbit(pack,12);
}

void Player::moveto(fixed x, fixed y, int ang) {
	 





	if ((mover->x != x) || (mover->y != y) || (mover->angle != ang)) {
	 
		mover->angle = ang;
		if (x>=mover->x) {
		 
			if (mover->canmove(x+(16l<<16),y+(8l<<16)) &&
				mover->canmove(x+(16l<<16),y-(8l<<16)))
				mover->x = x;
		} else {
			if (mover->canmove(x-(16l<<16),y+(8l<<16)) &&
				mover->canmove(x-(16l<<16),y-(8l<<16)))
				mover->x = x;
		  }
		if (y>=mover->y) {
		 
			if (mover->canmove(x+(8l<<16),y+(16l<<16)) &&
				mover->canmove(x-(8l<<16),y+(16l<<16)))
				mover->y = y;
		} else {
			if (mover->canmove(x+(8l<<16),y-(16l<<16)) &&
				mover->canmove(x-(8l<<16),y-(16l<<16)))
				mover->y = y;
		  }
		 





	}
}

void Player::animate() {
	fixed nx = mover->x, ny = mover->y;
	int i, j, newang = mover->angle;

	if (viewstate>=2) return;	 

	switch(comtype) {
	case COM_DIRECT:
		if (!viewstate) updatecommands();
		break;
	case COM_LOAD: unpackcom(loadpack()); break;
	case COM_SAVE:
		updatecommands();
		savepack(packcom());
		break;
	}

	const int mouse_turn = (!viewstate && comtype == COM_DIRECT && controlnum == 0) ? kb_mouse_dx() : 0;

	if (health<0) {
		if (!viewstate)
		 if (command.left) mover->angle = anglesub(newang,ANG45>>3); else
		  if (command.right) mover->angle = angleadd(newang,ANG45>>3);
		return;
	}

	remove();
  if (!viewstate) {
	CanFireActor::animate();
	if (health==0) {
		changefig(F_PLAYER+34,1);
		CLRFLAG(mover->flags,OMF_HITABLE|OMF_UNWALKABLE);
		if (ABS(newang-angleadd(hitangle,ANG180))>ANG45)
			newang=angleadd(newang,ANG45>>3);
		if (hitspeed) {
			nx+=costab[hitangle]*hitspeed;
			ny+=sintab[hitangle]*hitspeed;
			vangang=angleadd(vangang,ANGLVL*12);
			vangle = (int)fixmul((-sintab[vangang])*hitspeed/30,ANG30);
			if (vangle<0) vangle+=ANG360;
			hwalk += hitspeed*sintab[vangang>>1];
			if (hwalk>62l<<FIXSHIFT) hwalk=62l<<FIXSHIFT;
			hitspeed--;
			animcount=0;
		} else {
			if (hwalk > 32l<<FIXSHIFT) hwalk-=FIXONE<<1; else
			if (hwalk > 4l<<FIXSHIFT) hwalk-=FIXONE;
			else {
				health=-1;
				 
				play3Dsound(mover->x,mover->y,SND_SOLDDOWN,0,mover->id,SFL_NORMAL);
				eventmanager.sendcom(EV_MESSAGE,MSG_DEAD);
			}
			if (hwalk<32l<<FIXSHIFT) cecita=((32-lshr16(hwalk))/5+1)<<5;
			 
			vangle=anglesub(ANG0,(lshr16(hwalk)-32)*ANG30>>5);
			animcount = 4-((lshr16(hwalk)-4)/7);
			if (animcount<0) animcount=0; else
			 if (animcount>4) animcount=4;
		  }
		moveto(nx,ny,newang);
		place();
		return;
	}

	if (cecita) cecita--;
	if (hardcount) {
		if (!(--hardcount)) animpal(PAA_UNHARD,0);
	}
	if (upwalk)
		if (upwalk>0) upwalk=-upwalk;	 
				 else upwalk+=2;		 
	if (health>100) {
		if (--health == 100) {
			 
			animpal(PAA_UNGOD,0);
			 
		}
		if (!((health-100) & 31))
			eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_HEALTH,0);
	}

	 
	if (armcharge>0) {
	  if (!curarm) {
		if (armcharge==30) {
			play3Dsound(nx,ny,SND_SOLDATT,SM_NORMVOL,mover->id,SFL_NORMAL);
		} else
		if (armcharge==24) {
			updatestat(STT_FIRE,1);
			fixed cx = nx, cy = ny,
				  alt = hwalk, dist;
			i = fireray(cx,cy,newang,vangle,dist,alt,mover->id);
			if (dist<=74l<<FIXSHIFT) {
				if (i>-2) makeboom(cx,cy,alt,newang,i,BOOM_ASCIA);
				if (i>=0)
					eventmanager.senddirect(EV_HIT,mover->id,i,
											newang,ArmList[ARM_ASCIA].power);
				if (i>=-1)
					play3Dsound(cx,cy,SND_ASCIAHIT,0,SYSOWN,SFL_NORMAL);
			 





			}
		}
	  } else
	  if (curarm==ARM_FUCILE && armcharge==30)
		play3Dsound(mover->x,mover->y,SND_CHARGE,SM_LOWVOL,SYSOWN,SFL_NORMAL);
	}

	if (command.map) CHGFLAG(pflags,PFL_MAPVIEW);  

	if (command.open)
		eventmanager.sendshort(EV_PLACTIVATE,mover->id,0,
							   xytoidx(mover->x+(costab[newang]<<5),
									   mover->y+(sintab[newang]<<5)),
							   keys);
	if (command.chgarm) {
		j = i = curarm;
		 
		do {
		  do {
			j = (j+1)%(ARM_BOMB+1);
		  } while (j && !Armbull[j-1]);
		} while (!setarm(j));
		if (i!=curarm)
			eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);
	} else
	if (players<2 && comtype==COM_DIRECT) {
		j = curarm;
		if (kb_onepressed(KB_1)) i=0; else
		if (kb_onepressed(KB_2)) i=1; else
		if (kb_onepressed(KB_3)) i=2; else
		if (kb_onepressed(KB_4)) i=3; else
		if (kb_onepressed(KB_5)) i=4; else
		if (kb_onepressed(KB_6)) i=5; else i=-1;
		if (i>=0) setarm(i);
		if (j!=curarm)
			eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);
	}
	if (command.fire && fire(newang)) {
		animtrig = 200+armcharge;
		animcount = 1;
	}
	 











	i=0; j=0;
	if (command.strafe) {
		j++;
		if (command.left) {
			if (command.fast)
				{if (latspeed>-8) {if ((latspeed-=2)<-8) latspeed=-8;}}
			else
				{if (latspeed>-6) {if ((latspeed-=2)<-6) latspeed=-6;}}
		} else
		if (command.right) {
			if (command.fast)
				{if (latspeed<8) {if ((latspeed+=2)>8) latspeed=8;}}
			else
				{if (latspeed<6) {if ((latspeed+=2)>6) latspeed=6;}}
		} else i++;
	} else {
		i++;
		if (command.left) {
			if (command.fast) {
				if (angspeed>-5) angspeed-=2;
							else angspeed=-6;
			} else {
				if (angspeed>-3) angspeed-=2;
							else angspeed=-4;
			  }
		} else
		if (command.right) {
			if (command.fast) {
				if (angspeed<5) angspeed+=2;
						   else angspeed=6;
			} else {
				if (angspeed<3) angspeed+=2;
						   else angspeed=4;
			  }
		} else j++;
	  }
	if (i) {
		if (latspeed>0) latspeed--; else
		 if (latspeed<0) latspeed++;
	}
	if (j) {
		if (angspeed>0) angspeed--; else
		 if (angspeed<0) angspeed++;
	}

	if (angspeed) {
		if (angspeed>=0) newang = angleadd(newang,ANGLVL*angspeed);
					else newang = anglesub(newang,ANGLVL*(-angspeed));
	}

	if (mouse_turn) {
		newang = angleadd(newang, (mouse_turn * (ANGLVL + (ANGLVL >> 3))) / 2);
	}

	if (latspeed) {
		i = newang+ANG90; if (i>=ANG360) i-=ANG360;
		i = kaos_angidx(i);
		j = ABS(latspeed)>>1;
		 
		if (latspeed>0) {
			nx += kaos_fshl(costab[i], j);
			ny += kaos_fshl(sintab[i], j);
		} else {
			nx -= kaos_fshl(costab[i], j);
			ny -= kaos_fshl(sintab[i], j);
		  }
	}

	if (command.up) {
		if (command.fast)
			{if (speed<36) {if ((speed+=4)>36) speed=36;} else speed=36;}
		else
			{if (speed<28) {if ((speed+=4)>28) speed=28;} else speed=28;}
	} else
	if (command.down) {
		if (command.fast)
			{if (speed>-36) {if ((speed-=4)<-36) speed=-36;} else speed=-36;}
		else
			{if (speed>-28) {if ((speed-=4)<-28) speed=-28;} else speed=-28;}
	} else {
		if (speed>0) speed--; else
		  if (speed<0) speed++;
	  }
	if (hitspeed<0) {
		i = ANGLVL<<2;
		if (command.lookup) {
			vangle=angleadd(vangle,i);
			if (vangle>ANG30-i && vangle<ANG360-ANG30+i) vangle=ANG30-i;
		} else
		if (command.lookdown) {
			vangle=anglesub(vangle,ANGLVL<<2);
			if (vangle>ANG30-i && vangle<ANG360-ANG30+i) vangle=ANG360-ANG30+i;
		} else
		if (command.lookmid) hitspeed=0;
	}

  } else {
	  if (!viewstate) {
		if (latspeed>0) latspeed--; else
		 if (latspeed<0) latspeed++;
		if (angspeed>0) angspeed--; else
		 if (angspeed<0) angspeed++;
		if (speed>0) speed--; else
		  if (speed<0) speed++;
	  }
	}

   if (!viewstate) {
	if (speed) {

		if (GETFLAG(gameflags,GFL_AUTOCENTER))
			if (ABS(speed)>24 && hitspeed<0 && vangle) hitspeed=0;

		nx += costab[kaos_angidx(newang)]*speed>>2;
		ny += sintab[kaos_angidx(newang)]*speed>>2;
	   
		if (!GODMODE) {
			i = angwalk;
			angwalk = (angwalk+((speed*ANGLVL<<1)/3)+ANG360) % ANG360;
			hwalk = (FIXONE*ABS(upwalk))+(32l<<16)+(sintab[angwalk]*ABS(speed)>>4);
			 
			if (abs(angwalk-ANG270)<=ANG30 &&
				abs(i-ANG270)>ANG30)
				play3Dsound(0l,0l,SND_PASSO,SM_LOWVOL,mover->id,SFL_MULTI|SFL_FOLLOW);
		} else {
			angwalk = (angwalk+(ANGLVL<<2)+ANG360) % ANG360;
			hwalk = (FIXONE*ABS(upwalk))+(44l<<16)+(sintab[angwalk] );
			 
		  }
		handang = (handang+(speed*ANGLVL>>1)+ANG360) % ANG360;
		 
	   
		if (animtrig<200) {
			if (speed>0) {
				if ((animtrig+=speed>>2) >= 128) animtrig -= 128;
			} else {
				if ((animtrig+=speed>>2) < 0) animtrig += 128;
			  }
			switch (animtrig >> 5) {
				case 0:
				case 2: animcount = 0; break;
				case 1: animcount = 3; break;
				case 3: animcount = 2;
			}
		}

	} else {
		if (GODMODE) {
			angwalk = (angwalk+(ANGLVL<<2)+ANG360) % ANG360;
			hwalk = (44l<<16)+sintab[angwalk];
		} else {
			angwalk = 0;
			hwalk = (FIXONE*ABS(upwalk))+(32l<<16);
		  }
		if (animtrig<200) {
			animtrig=0;
			animcount=0;
		}
	  }

	if (hitspeed>0) {
		nx+=costab[hitangle]*hitspeed;
		ny+=sintab[hitangle]*hitspeed;
		vangang=angleadd(vangang,ANGLVL*12);
		vangle = (int)fixmul((-sintab[vangang])*hitspeed/30,ANG30);
		if (vangle<0) vangle+=ANG360;
		hwalk += hitspeed*sintab[vangang>>1];
		hitspeed--;
	} else
	  if (!hitspeed) {
		if (vangle) {
			if (vangle<=ANG90) vangle >>= 1; else
			  if ((vangle = ANG360-((ANG360-vangle)>>1)) == ANG360) vangle=0;
		} else hitspeed--;
		vangang = 0;
	  }

	if (animtrig>=200) {  
		if (--animtrig <= 200) {
			animtrig = 0;
			animcount = 0;
			changefig(F_PLAYER,8);
		}
	}

	moveto(nx,ny,newang);

	Object *obj;
	j = mover->index;
	if (players<2) {
	if ((i = ceilmap[j])==T_DOORSLOT || i>=T_TRANSP) {
		if (windsound<0)
			windsound=playsound(SND_WIND,SM_NORMVOL,SM_NORMVOL,0,SYSOWN,SFL_CONTINUE|SFL_FADE);
			 
			 
	} else
		if (windsound>=0)
			if (stopsound(windsound)) windsound=-1;
	}
	if ((i =  IDMap[j]) >= 0) 		 
		do {
			obj = objectslist.get(i);
			if (GETFLAG(obj->oflags,OFL_TAKEABLE))
				eventmanager.senddirect(EV_GET,i,mover->id,0,0);
		} while ((i = obj->mover->underq(j)) >= 0);
   }
   place();
}

void Player::handle_event(TEvent &event) {
    Object *object = nullptr;
    char token = 0;
	int msg = -1;
	if (event.what != EV_KILL) Object::handle_event(event);
	if (health<0) {
		if (event.what==EV_SCODE && event.source==SC_MNNico) {
			eventmanager.sendcom(EV_RESURRECT,mover->id);
			changefig(F_PLAYER,8); animcount=0;
			health=100+400; cecita=0;
			eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_HEALTH,0);
			remove();
			SETFLAG(mover->flags,OMF_HITABLE|OMF_UNWALKABLE);
			place();
			eventmanager.sendcom(EV_MESSAGE,MSG_RESURRECT);
		}
		return;
	}
	switch (event.what) {
		case EV_ENDSOUND:
#ifdef KAOS_DEMO_TRACE
			fprintf(stderr,
				"KAOS_EVENT tic=%llu player=%d EV_ENDSOUND source=%d -> EV_PLNOISE pos=%08X,%08X\n",
				(unsigned long long)kaos_sim_tic, mover ? mover->id : -1,
				event.source, (unsigned)(std::uint32_t)mover->x,
				(unsigned)(std::uint32_t)mover->y);
#endif
			eventmanager.sendpos(EV_PLNOISE,mover->id,mover->x,mover->y,0);
			break;
		case EV_POISON:
			if (GODMODE || health<=0 || health>100) break;
			if ((cecita += event.data.msg.y<<5) > 384) cecita=384;
			eventmanager.sendcom(EV_MESSAGE,MSG_POISONED);
			 
			 
		case EV_HIT:
			if (health<=0) break;
			if (!GODMODE && health<=100 && event.data.msg.y>=4) {
				if ((health-=event.data.msg.y)<0) {
					health=0;
					eventmanager.sendshort(EV_PLKILLED,mover->id,event.source,0,0);
					 
				}
				hitangle = event.data.msg.x;
				if ((hitspeed = event.data.msg.y) >= 26) hitspeed=26;
				if (hitspeed>=10) play3Dsound(mover->x,mover->y,SND_COLPO2,0,mover->id,SFL_NORMAL); else
				 if (hitspeed>=6) play3Dsound(mover->x,mover->y,SND_COLPO1,0,mover->id,SFL_NORMAL);
				changefig(F_PLAYER+32,2);
				animcount=0; animtrig=220;
				vangle = 0;
				vangang = 0;
				animpal(PAA_RED,hitspeed>>4);
				eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_FACEHIT|IBR_HEALTH,0);
			}
			break;
		case EV_PLKILLED:
			if (event.data.msg.dest == mover->id &&
				event.source != mover->id) {	 
				plkilled++;
				eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_PLKILLED,0);
			}
			break;
		case EV_GET:
			object = objectslist.get(event.source);
			if (object != NULL) {	 
				char i = 1;
				 



				switch (object->figstart) {
					case F_KEYELLOW:
						if (!GETFLAG(keys,LCKT_L1)) {
							SETFLAG(keys,LCKT_L1);
							eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_KEYS,0);
							playsound(SND_KEYS,16,16,0,SYSOWN,SFL_FIXED);
							animpal(PAA_YKEY,0);
							eventmanager.sendcom(EV_MESSAGE,MSG_GOT_KEY_YELLOW);
							 
						} else i--;
						break;
					case F_KEYRED:
						if (!GETFLAG(keys,LCKT_L2)) {
							SETFLAG(keys,LCKT_L2);
							eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_KEYS,0);
							playsound(SND_KEYS,16,16,0,SYSOWN,SFL_FIXED);
							animpal(PAA_RKEY,0);
							eventmanager.sendcom(EV_MESSAGE,MSG_GOT_KEY_RED);
							 
						} else i--;
						break;
					case F_FUCILE:
					case F_MITRA:
					case F_BAZOOKA:
						byte arm;
						switch(object->figstart) {
							case F_FUCILE: arm = ARM_FUCILE; break;
							case F_MITRA: arm = ARM_MITRA; break;
							case F_BAZOOKA: arm = ARM_BAZOOKA; break;
						}
						token = takearm(arm);

						if (token>1) {	 
							switch(arm) {
							case ARM_FUCILE: msg=MSG_GOT_FUCILE; break;
							case ARM_MITRA: msg=MSG_GOT_MITRA; break;
							case ARM_BAZOOKA: msg=MSG_GOT_BAZOOKA; break;
							}
							animpal(PAA_ARM,1);
							if (arm==ARM_BAZOOKA) {
								playsound(SND_RISATA,0,0,0,SYSOWN,SFL_FIXED);
								playsound(SND_CHARGE2,0,0,127,SYSOWN,SFL_FIXED);
								eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_SMILE,0);
							} else
								playsound(SND_CHARGE,0,0,127,SYSOWN,SFL_FIXED);
						} else
						if (token>0) {	 
							switch(arm) {
							case ARM_FUCILE: msg=MSG_GOT_AMMO_FUCILE; break;
							case ARM_MITRA: msg=MSG_GOT_AMMO_MITRA; break;
							case ARM_BAZOOKA: msg=MSG_GOT_AMMO_BAZOOKA; break;
							}
							animpal(PAA_ARM,0);
							if (arm==ARM_BAZOOKA)
								playsound(SND_CHARGE2,0,0,0,SYSOWN,SFL_FIXED);
							else
								playsound(SND_CHARGE,0,0,0,SYSOWN,SFL_FIXED);
						} else {
							i--;
							break;
						  }
						eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);
						if (msg>=0) eventmanager.sendcom(EV_MESSAGE,msg);
						break;
					case F_CARTUCCIA1:
					case F_CARTUCCIA2:
					case F_CARTUCCIA3:
					case F_CARTUCCIA4:
						switch(object->figstart) {
							case F_CARTUCCIA1:
								if (!takeammo(ARM_PISTOLA,ARM_ADDPISTOLA)) i--;
								msg = MSG_GOT_AMMO_PISTOLA;
								break;
							case F_CARTUCCIA2:
								if (!takeammo(ARM_FUCILE,ARM_ADDFUCILE)) i--;
								msg = MSG_GOT_AMMO_FUCILE;
								break;
							case F_CARTUCCIA3:
								if (!takeammo(ARM_MITRA,ARM_ADDMITRA)) i--;
								msg = MSG_GOT_AMMO_MITRA;
								break;
							case F_CARTUCCIA4:
								if (!takeammo(ARM_BAZOOKA,ARM_ADDBAZOOKA)) i--;
								msg = MSG_GOT_AMMO_BAZOOKA;
								break;
							 



						}
						if (i) {
							eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);
							if (msg>=0) eventmanager.sendcom(EV_MESSAGE,msg);
							if (object->figstart == F_CARTUCCIA4)
								playsound(SND_CHARGE2,0,0,0,SYSOWN,SFL_FIXED);
							else
								playsound(SND_CHARGE,0,0,0,SYSOWN,SFL_FIXED);
							animpal(PAA_ARM,0);
						}
						break;
					case F_FARMABIG:
					case F_FARMALIT:
						msg = MSG_GOT_FARMA_LIT;
						if (health>=100) {	 
							i--; break;
						}
						if (object->figstart == F_FARMABIG) {
							health+=20;
							msg = MSG_GOT_FARMA_BIG;
							 
						}
						health+=10;
						if (health>100) health=100;
						animpal(PAA_GOLD,0);
						eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_HEALTH,0);
						playsound(SND_KEYS,16,16,0,SYSOWN,SFL_FIXED);
						eventmanager.sendcom(EV_MESSAGE,msg);
						 
						break;
					case F_SOLDOBIG:
						 




						if ((hardcount += 720) > 2160) hardcount = 2160;
						playsound(SND_BONUS,0,0,0,SYSOWN,SFL_FIXED);
						eventmanager.sendcom(EV_MESSAGE,MSG_GOT_BIGCOIN);
						break;
					case F_SOLDOLIT:
						 
						animpal(PAA_GOLD,0);
						 
						playsound(SND_KEYS,16,16,0,SYSOWN,SFL_FIXED);
						eventmanager.sendcom(EV_MESSAGE,MSG_GOT_LITCOIN);
						 
						break;
				 



				 
				case F_BONUS:
					 
					if (health<=100) {
						health=100;
						 
					}
					if ((health += 720) > 2160) health = 2160;
					if (armflag && AFL_PISTOLA)
						Armbull[ARM_PISTOLA-1] = ArmList[ARM_PISTOLA].max_ammo;
					if (armflag && AFL_FUCILE)
						Armbull[ARM_FUCILE-1] = ArmList[ARM_FUCILE].max_ammo;
					if (armflag && AFL_MITRA)
						Armbull[ARM_MITRA-1] = ArmList[ARM_MITRA].max_ammo;
					if (armflag && AFL_BAZOOKA)
						Armbull[ARM_BAZOOKA-1] = ArmList[ARM_BAZOOKA].max_ammo;
					if (armflag && AFL_BOMB)
						Armbull[ARM_BOMB-1] = ArmList[ARM_BOMB].max_ammo;
						 



					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,
						IBR_SMILE| IBR_HEALTH|IBR_ARMS,0);
					playsound(SND_BONUS,0,0,0,SYSOWN,SFL_FIXED);
						 
					eventmanager.sendcom(EV_MESSAGE,MSG_GOT_BONUS);
					break;
				case F_ANTIKAOS:
					animpal(PAA_LIGHT,0);
					playsound(SND_BONUS,0,0,0,SYSOWN,SFL_FIXED);
					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_SMILE,0);
					eventmanager.sendcom(EV_MESSAGE,MSG_GOT_KEY_ANTIKAOS);
					break;
				 







				 
				case F_OSSALIT:
					if (!GODMODE) {
						if (GETFLAG(gameflags,GFL_NORANDOM))
							angwalk = angleadd(angwalk,ANG30);
						else
							angwalk = angleadd(angwalk,random(ANG30)<<1);
						speed=(speed<<1)/3;
						latspeed=(latspeed<<1)/3;
					}
					break;
				case F_SLIMER+16:
				case F_SPIDER+14:
				case F_SOLDIER+30:
				 




					if (!GODMODE) {
						upwalk=ABS(upwalk);
						if (upwalk<8) upwalk+=2;
						 
					}
					i--;
					break;
				 
				case F_BOMB:
					if (((Bomb *)object)->getownerid() == mover->id) {
						i--;
						break;
					}
					if (takeammo(ARM_BOMB,ARM_ADDBOMB)) {
#ifndef __SHAREWARE__
						SETFLAG(armflag,AFL_BOMB);
#endif
						if (GETFLAG(object->oflags,OFL_ISACTOR))
							takeammo(ARM_BOMB,-ARM_ADDBOMB);
						else {
							eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);
							playsound(SND_CHARGE,0,0,0,SYSOWN,SFL_FIXED);
							animpal(PAA_ARM,0);
						}
						eventmanager.sendcom(EV_MESSAGE,MSG_GOT_BOMBA);
					} else i--;
					break;
				}
				if (i) {
					eventmanager.senddirect(EV_GOT,mover->id,event.source,mover->angle,speed>>1);
					 
				}
			}
			break;
		case EV_SCODE:
			switch (event.source) {
				case SC_MNBGod:
					 
					CHGFLAG(pflags,PFL_GOD);
					if (GODMODE) {
						animpal(PAA_GOD,0);
						oldarms = armflag;
						oldkeys = keys;
#ifndef __SHAREWARE__
						SETFLAG(armflag,AFL_PISTOLA|AFL_FUCILE|AFL_MITRA|AFL_BAZOOKA|AFL_BOMB);
#else
						SETFLAG(armflag,AFL_PISTOLA|AFL_FUCILE|AFL_MITRA|AFL_BAZOOKA);
#endif
						curarm = ARM_BAZOOKA;
						SETFLAG(keys,LCKT_L1|LCKT_L2);
						eventmanager.sendcom(EV_MESSAGE,MSG_GODMODE);
					} else {
						armflag = oldarms;
						keys = oldkeys;
						curarm = ARM_PISTOLA;
						animpal(PAA_UNGOD,0);
					  }
					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS|IBR_KEYS,0);
					break;
				case SC_MNNico:
					health = 100+400;
					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_HEALTH,0);
					eventmanager.sendcom(EV_MESSAGE,MSG_INVINCIBLE);
					break;
				case SC_MNMaxy:
#ifndef __SHAREWARE__
					SETFLAG(armflag,AFL_PISTOLA|AFL_FUCILE|AFL_MITRA|AFL_BAZOOKA|AFL_BOMB);
#else
					SETFLAG(armflag,AFL_PISTOLA|AFL_FUCILE|AFL_MITRA|AFL_BAZOOKA);
#endif
					Armbull[ARM_PISTOLA-1] = ArmList[ARM_PISTOLA].max_ammo;
					Armbull[ARM_FUCILE-1] = ArmList[ARM_FUCILE].max_ammo;
					Armbull[ARM_MITRA-1] = ArmList[ARM_MITRA].max_ammo;
					Armbull[ARM_BAZOOKA-1] = ArmList[ARM_BAZOOKA].max_ammo;
					Armbull[ARM_BOMB-1] = ArmList[ARM_BOMB].max_ammo;
					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS|IBR_SMILE,0);
					break;
				case SC_MNAndy:
					SETFLAG(keys,LCKT_L1|LCKT_L2);
					eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_KEYS|IBR_SMILE,0);
					break;
				case SC_MNHard:
					 
					hardcount = 2160;
					eventmanager.sendcom(EV_MESSAGE,MSG_HARDMODE);
					break;
				case SC_MNInvi:
					mover->remove();
					CHGFLAG(mover->flags,OMF_HITABLE);
					mover->place();
					if (GETFLAG(mover->flags,OMF_HITABLE))
						eventmanager.sendcom(EV_MESSAGE,MSG_HARDMODE);
					break;
			}
			 
			break;
		case EV_ADDSTAT:
			updatestat(event.data.msg.x,event.data.msg.y);
			break;
	}
}

void Player::makeboom(fixed x, fixed y, fixed z, int angle, int id, byte type) {
	if (hardcount && curarm>0) {
		eventmanager.sendpos(EV_BOOM,mover->id,x,y,40l);
		delete mm_reserve(sizeof(FireBoom));
		objectslist.put(new FireBoom(x,y,z,angle,BOOM_MISSIL));
		quake=14;	 
	} else {
		int btype = type,
			fig;
		if (id>=0) {
			fig = objectslist.get(id)->figstart;
			updatestat(STT_HIT,1);
			if (fig>=F_PLAYER && fig<=F_MOUSE) {
				btype = BOOM_BLOOD;
				updatestat(STT_BLOOD,1);
			}
		}
		if (btype!=BOOM_BLOOD || GETFLAG(gameflags,GFL_SHOWBLOOD)) {
			delete mm_reserve(sizeof(FireBoom));
			objectslist.put(new FireBoom(x,y,z,angle,btype));
			if (btype==BOOM_BLOOD && fig<F_MOUSE) {
				 
				fixed dist;
				 
				int id1 = fireray(x,y,angle,vangle,dist,z,id);
				if (id1==-1 && dist<(96l<<FIXSHIFT))
					eventmanager.sendshort(EV_CHGTEXTURE,0,0,xytoidx(x,y),0);
			}
		}
	  }
}

char Player::fire(int angle) {
	fixed cx, cy;
	int id;
	fixed alt, dist;
	Object *object;

	if (armcharge) return 0;

	 
	if (!CanFireActor::fire(angle)) return 0;
	if (GODMODE && curarm) Armbull[curarm-1]++;
	if (curarm)
		eventmanager.sendshort(EV_UPDATEBAR,mover->id,0,IBR_ARMS,0);

	 
	switch (curarm) {
		case ARM_ASCIA: break;
		case ARM_PISTOLA:
			updatestat(STT_FIRE,1);
			play3Dsound(mover->x,mover->y,SND_PISTOLA,SM_NORMVOL,mover->id,SFL_NORMAL);
			cx=mover->x; cy=mover->y;
			alt = hwalk;
			 
			id = fireray(cx,cy,angle,vangle,dist,alt,mover->id);
			if (id>-2) makeboom(cx,cy,alt,angle,id,BOOM_PALLOT);
			if (id>=0)
				eventmanager.senddirect(EV_HIT,mover->id,id,
										angle,ArmList[ARM_PISTOLA].power);
			 
			 
			break;
		case ARM_FUCILE:
			updatestat(STT_FIRE,3);
			play3Dsound(mover->x,mover->y,SND_FUCILE,SM_NORMVOL,mover->id,SFL_NORMAL);
			cx=mover->x; cy=mover->y;
			alt = hwalk;	 
			id = fireray(cx,cy,anglesub(angle,ANGLVL<<1),vangle,dist,alt,mover->id);
			if (id>-2) makeboom(cx,cy,alt,angle,id,BOOM_PALLOT);
			if (id>=0)
				eventmanager.senddirect(EV_HIT,mover->id,id,
										angle,ArmList[ARM_FUCILE].power);
			cx=mover->x; cy=mover->y;
			alt = hwalk;
			id = fireray(cx,cy,angle,vangle,dist,alt,mover->id);
			if (id>-2) {
				makeboom(cx,cy,alt,angle,id,BOOM_PALLOT);
				makeboom(cx,cy,alt+lshr6(dist),angle,id,BOOM_PALLOT);
				makeboom(cx,cy,alt-lshr6(dist),angle,id,BOOM_PALLOT);
			}
			if (id>=0)
				eventmanager.senddirect(EV_HIT,mover->id,id,
										angle,ArmList[ARM_FUCILE].power);
			 
			 
			cx=mover->x; cy=mover->y;
			alt = hwalk;
			id = fireray(cx,cy,angleadd(angle,ANGLVL<<1),vangle,dist,alt,mover->id);
			if (id>-2) makeboom(cx,cy,alt,angle,id,BOOM_PALLOT);
			if (id>=0)
				eventmanager.senddirect(EV_HIT,mover->id,id,
										angle,ArmList[ARM_FUCILE].power);
			if ((speed-=24)<-32) speed=-32;
			break;
		case ARM_MITRA: {
			updatestat(STT_FIRE,1);
			play3Dsound(mover->x,mover->y,SND_MITRA,SM_NORMVOL,mover->id,SFL_NORMAL);
			cx=mover->x; cy=mover->y;
			alt = hwalk;
			int ang = angle;
			if (!GETFLAG(gameflags,GFL_NORANDOM))
				ang += (ANGLVL<<2)-random((ANGLVL<<3)+1);
			if (ang<0) ang+=ANG360; else
			 if (ang>=ANG360) ang-=ANG360;
			id = fireray(cx,cy,ang,vangle,dist,alt,mover->id);
			 
			 
			if (id>-2) makeboom(cx,cy,alt,angle,id,BOOM_PALLOT);
			if (id>=0)
				eventmanager.senddirect(EV_HIT,mover->id,id,
										angle,ArmList[ARM_MITRA].power);
			break;
			}
		case ARM_BAZOOKA:
		 
			updatestat(STT_FIRE,1);
			cx=mover->x; cy=mover->y;
			alt = hwalk;
			fireray(cx,cy,angle,vangle,dist,alt,mover->id);
			cx = costab[angle]; cy = sintab[angle];
			delete mm_reserve(sizeof(Fireball));
			object = new Fireball(mover->x+(cx<<4) ,
								  mover->y+(cy<<4) ,
								  angle, F_MISSILE,
								  mover->z-(8l<<FIXSHIFT),alt,dist,mover->id);
			play3Dsound(mover->x,mover->y,SND_MISSILE,0,mover->id,SFL_NORMAL);
			 
			objectslist.put(object);
			if ((speed-=16)<-16) speed=-16;
			break;
		case ARM_BOMB:
			updatestat(STT_FIRE,1);
			cx = costab[angle]; cy = sintab[angle];
			play3Dsound(mover->x,mover->y,SND_SLIMATT,0,mover->id,SFL_NORMAL);
			delete mm_reserve(sizeof(Bomb));
			objectslist.put(new Bomb(mover->x+(cx<<4) ,
									 mover->y+(cy<<4) ,
									 mover->z-(8l<<FIXSHIFT),angle,20,
									 mover->id));
			break;
	}
	return 1;
}

void Player::getdata(TPlayerData &data) {
	 
	data.health = health;
	data.curarm = curarm;
	data.keys = keys;
	data.armflag = armflag;
	fwmove(data.armbull,Armbull,sizeof(Armbull));
}

void Player::updatestat(int type, int num) {
	switch(type) {
	case STT_FIRE:	fires+=num; break;
	case STT_HIT:	hits+=num; break;
	case STT_BLOOD:	blood+=num; break;
	case STT_KILL:	kills+=num; break;
	case STT_TECNO:	tecno+=num; break;
	}
}

void Player::getstats(std::int32_t &_fires,std::int32_t &_hits,std::int32_t &_blood,
					  std::int32_t &_tecno,int &_kills) {
	_fires = fires;
	_hits = hits;
	_blood = blood;
	_kills = kills;
	_tecno = tecno;
}

Player::~Player() {
	 
}