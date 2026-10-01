 






 

#include "std.hpp"
#include "fastmem.hpp"
#include "vgatool3.hpp"
#include "lgraph.hpp"
#include "lmouse.hpp"
#include "mm4.hpp"
#include "lfont2.hpp"
#include "kaosc.hpp"
#include <stdlib.h>
#include <new>
#include <cstdint>
#include "platform_compat.hpp"
#include "win32_editor_platform.hpp"
#include <ctype.h>
#include <string.h>
#include "platform_compat.hpp"
#include <fcntl.h>
#include <errno.h>
#include "crcio.hpp"

#define	MAXHOTRECTS		32

#define	NUM_TEXTURES  	TEXTNUM
#define	NUM_OBJECTS     (T_OBJECTS-T_TEXTURES)
#define	NUM_ENEMIES		(T_ENEMIES-T_OBJECTS)
#define	MAXTEXTURES		(NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES+12)

#define	LVL_FLOOR		0
#define	LVL_WALL		1
#define	LVL_CEILING		2

#define	MLV_TEXTURES1	0
#define	MLV_TEXTURES2	1
#define	MLV_OBJECTS		2
#define	MLV_ACTORS		3
#define	MLV_TABLES		4

#define	COM_LOAD		1
#define	COM_SAVE		2
#define	COM_LEVELC		3
#define	COM_LEVELW		4
#define	COM_LEVELF		5
#define COM_NOOBJ		6
#define	COM_NOUNDER		7

typedef struct {
		int x,y,dx,dy;
		const char *decr;
	} TArea;
typedef struct {
		int x,y,dx,dy;
		const char *descr;
	} THotRect;
typedef struct {
		char active;
		char name[24];
		byte backnum;
		int wallidx,flooridx,ceilidx;
	} TLevel;
typedef struct {
		char x, y;
	} TPlayerpos;

const char *strKaosfirm = "The KAOS Editor v0.5beta",
	 *strLoad 	  = "Load .DLF file  [L]",
	 *strSave 	  = "Save .DLF file  [S]",
	 *strclickput = "Left:put, Right:delete, Cursor:scroll",
	 *strmenu01	  = "Left:get, Right:change list (Textures 1)",
	 *strmenu02	  = "Left:get, Right:change list (Textures 2)",
	 *strmenu1	  = "Left:get, Right:change list (Objects)",
	 *strmenu2	  = "Left:get, Right:change list (Actors)",
	 *strmenu3	  = "Left:sel/activ, Right:change, under:background",
	 *strshape0	  = "Shape: square",
	 *strshape1	  = "Shape: vertical",
	 *strshape2	  = "Shape: horizontal",
	 *strshape3	  = "Shape: diagonal (1)",
	 *strshape4	  = "Shape: diagonal (2)",
	 *strshape5	  = "Shape: h/v inverted (doors)",
	 *strtype0	  = "Type: wall",
	 *strtype1	  = "Type: door (slide right)",
	 *strtype2	  = "Type: door (middle)",
	 *strtype3	  = "Type: grid (4-8)",
	 *strceiling  = "Edit Level: Ceiling  [1]",
	 *strfloor    = "Edit Level: Floor  [2]",
	 *strwalls    = "Edit Level: Walls  [3]",
	 *strtable	  = "Select Tables",
	 *strmap	  = "Global map",
	 *strgetmap	  = "Get from map",
	 *strgetsee	  = "Left:get, Right:see, Cursor:scroll",
	 *strnoobj	  = "No objects drawing  [O]",
	 *strnounder  = "No under-wall textures in floor/ceiling map [U]",
	 *strfill	  = "Fills the entire plane with current texture";
char strlname[40];

const char idDLF[9] = "NICdlf\x7\x1a";
char filename[9] = "KAOS";
char *editor_vpage = new char[64000];
PPal pal = new char[768];
Font *litfont = NULL;
MouseCursor crsr_std, crsr_get;
char *undermem = new char[32*32];	 
THotRect *HotArray = new THotRect[MAXHOTRECTS];
int numrects = 0;
int oldrect = -1;
TLevel Level[16];
char **TextAr = new char *[MAXTEXTURES];
byte *floormap, *ceilmap;
TMapInfo *map, curstore, nullinf = {0,0,0,0,0};
int curlvl = -1,
	levelnum = -1,
	menulvl = -1,
	curshape = 0,
	curtype = 0;
char getmode = 0,
	 nounder = 0,
	 noobj = 0;

int command;  

TPlayerpos playerpos[16][3];
int viewleft = 0, viewtop = 0;
char **MaskAr = new char *[6];
char *nullText = new char[64],
	 *door1Text = new char[64],
	 *door2Text = new char[64],
	 *gridText = new char[64],
	 *selText = new char[64],
	 *udoffText = new char[64],
	 *udonText = new char[64],
	 *rainoffText = new char[64],
	 *rainonText = new char[64],
	 *menuText;
char **TableNum = new char *[17];
int p_noobj, p_nounder;

void *ddf = NULL;
std::uint16_t ddfnum = 0;  

void alloc_ddf(int num) {
	if (ddf) {
		delete[] static_cast<byte *>(ddf);
		ddf = NULL;
	}
	ddfnum = static_cast<std::uint16_t>(num);
	if (ddfnum)
		ddf = mm_reserve(static_cast<word>(static_cast<std::uint32_t>(ddfnum) << 2));
}

int addrect(int _x,int _y,int _dx,int _dy, const char *_descr) {
	if (numrects<MAXHOTRECTS) {
		THotRect &hr = HotArray[numrects];
		hr.x = _x; hr.y = _y;
		hr.dx=_dx; hr.dy=_dy;
		hr.descr = _descr;
		return numrects++;
	} else error("addrect","overflow");
	return -1;	 
}

void delrect() {
	if (numrects) numrects--;
}

const char *getstr(int n) {
	if (n>0 && n<numrects) return HotArray[n].descr;
	return NULL;
}

void changestr(int n, const char *str) {
	if (n>0 && n<numrects)
		HotArray[n].descr = str;
}

char pointinrect(int x, int y, THotRect &rect) {
	return (x>=rect.x && x<rect.x+rect.dx &&
			y>=rect.y && y<rect.y+rect.dy);
}

int recthit(int x, int y) {
	for (int i=numrects;i--;)
		if (pointinrect(x,y,HotArray[i])) return i;
	return -1;
}

void vtransf(int x, int y, int dx, int dy) {
	int dx1, dy1;
	if (dx<16) dx1=16; else dx1 = dx;
	if (dy<16) dy1=16; else dy1 = dy;
	mouse.update(x,y,dx1,dy1);
	ftransfblock(x,y,dx,dy,VMEMPTR);
	mouse.show();
}

void statuswrite(const char *s) {
	ffillblock(4,191,208,8,0x72727272l);
	litfont->write(4,192,127,117,s);
	vtransf(4,191,208,8);
	for (int i=0;i<40;i++) waitvsync();
	oldrect=-1;
}

void paint(int x, int y, char transf) {
	if (x<viewleft || x>=viewleft+30 ||
		y<viewtop || y>=viewtop+19) return;
	int vx = 4+((x-viewleft)<<3),
		vy = 36+((y-viewtop)<<3),
		idx = (x<<6)+y;
	byte mask;
	char wall=0;
	 
	if (map[idx].stop) {
		if ((mask=map[idx].shape)!=0) {
			storelfig(vx,vy,8,8,nullText);
			storelmfig(vx,vy,8,8,MaskAr[mask],TextAr[map[idx].data]);
		} else
			storelfig(vx,vy,8,8,TextAr[map[idx].data]);
		switch(map[idx].type) {
			case MTP_DOOR1:
				putlfig(vx,vy,8,8,door1Text);
				break;
			case MTP_DOOR2:
				putlfig(vx,vy,8,8,door2Text);
				break;
			case MTP_GRID1:
				putlfig(vx,vy,8,8,gridText);
				break;
		}
		wall++;
	} else {
		storelfig(vx,vy,8,8,nullText);
		if (map[idx].none) {
			if (!noobj) putlfig(vx,vy,8,8,TextAr[map[idx].data]);
		}
	  }
	if (curlvl==LVL_FLOOR) {
		if (!(nounder && wall))
			storelmfig(vx,vy,8,8,MaskAr[0],TextAr[floormap[idx]]);
	} else
	if (curlvl==LVL_CEILING) {
		if (!(nounder && wall))
			storelmfig(vx,vy,8,8,MaskAr[0],TextAr[ceilmap[idx]]);
	}
	if (transf) vtransf(vx,vy,8,8);
}

void paintrect(int x, int y, int dx, int dy) {
	int i,j;
	if (x<viewleft) x=viewleft; else
	 if (x+dx>viewleft+30) dx=viewleft+30-x;
	if (y<viewtop) y=viewtop; else
	 if (y+dy>viewtop+19) dy=viewtop+19-y;
	for (i=x;i<x+dx;i++)
	 for (j=y;j<y+dy;j++)
		paint(i,j,0);
	vtransf(4+((x-viewleft)<<3),36+((y-viewtop)<<3),dx<<3,dy<<3);
}

#define	SEL_X	216
#define	SEL_Y	191
void changemenu(int lvlnum);

void updatesel() {
	byte mask;
	if (curlvl==LVL_WALL) {
		if (curstore.stop) {
			if ((mask=curstore.shape)!=0) {
				storelfig(SEL_X,SEL_Y,8,8,nullText);
				storelmfig(SEL_X,SEL_Y,8,8,MaskAr[mask],TextAr[curstore.data]);
			} else
				storelfig(SEL_X,SEL_Y,8,8,TextAr[curstore.data]);
			switch(curstore.type) {
				case MTP_DOOR1:
					putlfig(SEL_X,SEL_Y,8,8,door1Text);
					break;
				case MTP_DOOR2:
					putlfig(SEL_X,SEL_Y,8,8,door2Text);
					break;
				case MTP_GRID1:
					putlfig(SEL_X,SEL_Y,8,8,gridText);
					break;
			}
		} else {
			storelfig(SEL_X,SEL_Y,8,8,nullText);
			if (curstore.none) {
				putlfig(SEL_X,SEL_Y,8,8,TextAr[curstore.data]);
			}
		  }
	} else
	if (curlvl==LVL_FLOOR || curlvl==LVL_CEILING)
		storelfig(SEL_X,SEL_Y,8,8,TextAr[curstore.data]);
	vtransf(SEL_X,SEL_Y,8,8);

	int app = menulvl;
	menulvl = -1;
	changemenu(app);
}

void scroll(int dx, int dy) {
	int newl = viewleft+dx,
		newt = viewtop+dy;
	if (newl<0) newl=0; else
	 if (newl+30>64) newl=64-30;
	if (newt<0) newt=0; else
	 if (newt+19>64) newt=64-19;

	mouse.update(4,36,30<<3,19<<3);
	if (newl!=viewleft) {
	  if (newl>viewleft) {
		dx = newl-viewleft;
		viewleft = newl;
		fcopyblock(4+(dx<<3),36,4,36,(30-dx)<<3,19<<3,VMEMPTR);
		setvbuff(VMEMPTR);
		ftransfblock(4,36,(30-dx)<<3,19<<3,editor_vpage);
		setvbuff(editor_vpage);
		paintrect(newl+30-dx,newt,dx,19);
	  } else {
			dx = viewleft-newl;
			viewleft = newl;
			fcopyblock(4,36,4+(dx<<3),36,(30-dx)<<3,19<<3,VMEMPTR);
			setvbuff(VMEMPTR);
			ftransfblock(4+(dx<<3),36,(30-dx)<<3,19<<3,editor_vpage);
			setvbuff(editor_vpage);
			paintrect(newl,newt,dx,19);
		}
	}
	if (newt!=viewtop) {
	  if (newt>viewtop) {
		dy = newt-viewtop;
		viewtop = newt;
		fcopyblock(4,36+(dy<<3),4,36,30<<3,(19-dy)<<3,VMEMPTR);
		setvbuff(VMEMPTR);
		ftransfblock(4,36,30<<3,(19-dy)<<3,editor_vpage);
		setvbuff(editor_vpage);
		paintrect(newl,newt+19-dy,30,dy);
	  } else {
			dy = viewtop-newt;
			viewtop = newt;
			fcopyblock(4,36,4,36+(dy<<3),30<<3,(19-dy)<<3,VMEMPTR);
			setvbuff(VMEMPTR);
			ftransfblock(4,36+(dy<<3),30<<3,(19-dy)<<3,editor_vpage);
			setvbuff(editor_vpage);
			paintrect(newl,newt,30,dy);
		}
	}
	mouse.show();
}

void store(int x,int y) {
	int idx = (x<<6)+y;
	if (curstore.data<NUM_TEXTURES) {
		switch(curlvl) {
			case LVL_WALL: map[idx] = curstore; break;
			case LVL_FLOOR: floormap[idx] = curstore.data; break;
			case LVL_CEILING: ceilmap[idx] = curstore.data; break;
		}
	} else
	if (curstore.data<NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES) {
		 
		map[idx] = curstore;
	} else
	if (curstore.data<NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES+12) {
		int i = (curstore.data-(NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES))>>2;
		if (playerpos[levelnum][i].x>=0) {
			map[(playerpos[levelnum][i].x<<6)+playerpos[levelnum][i].y] = nullinf;
			paint(playerpos[levelnum][i].x,playerpos[levelnum][i].y,1);
		}
		playerpos[levelnum][i].x = x; playerpos[levelnum][i].y = y;
		map[idx] = curstore;
	} else return;
	paint(x,y,1);
}

void changelevel(int num) {
	if (num != levelnum && num>=0 && num<16) {
		if (levelnum != -1) {
			mm_unlock(Level[levelnum].wallidx);
			mm_unlock(Level[levelnum].flooridx);
			mm_unlock(Level[levelnum].ceilidx);
		}
		levelnum = num;
		mm_reset();
		map = (TMapInfo *)mm_recall(Level[levelnum].wallidx);
		mm_lock(Level[levelnum].wallidx);
		floormap = (byte *)mm_recall(Level[levelnum].flooridx);
		mm_lock(Level[levelnum].flooridx);
		ceilmap = (byte *)mm_recall(Level[levelnum].ceilidx);
		mm_lock(Level[levelnum].ceilidx);
		paintrect(viewleft,viewtop,30,19);
		storelfig(168,8,8,8,TableNum[levelnum]);
		vtransf(168,8,8,8);
		strcpy(strlname,"Change name (");
		strcat(strlname,Level[levelnum].name);
		strcat(strlname,")");
	}
}

void changelvl(int lvlnum) {
	if (curlvl != lvlnum) {
		curlvl=lvlnum;
		ffillblock(228,191,88,8,0x72727272l);
		switch(lvlnum) {
			case LVL_WALL:
				litfont->write(228,192,127,117,"Level: Walls");
				break;
			case LVL_FLOOR:
				litfont->write(228,192,127,117,"Level: Floor");
				break;
			case LVL_CEILING:
				litfont->write(228,192,127,117,"Level: Ceiling");
				break;
			default: error("changelvl","level unknown");
		}
		vtransf(228,191,88,8);
		paintrect(viewleft,viewtop,30,19);
	}
}

void changemenu(int lvlnum) {
	int i;
	if (lvlnum != menulvl) {
		putlfig(250,36,68,84,menuText);
		switch (menulvl = lvlnum) {
			case MLV_TEXTURES1:
				 
				for(i=0;i<56;i++) {
					storelfig(250+(i%7)*10,36+(i/7)*10,8,8,TextAr[i]);
					if (curstore.data == i)
						putlfig(250+(i%7)*10,36+(i/7)*10,8,8,selText);
				}
				changestr(3,strmenu01);
				break;
			case MLV_TEXTURES2:
				for(i=0;i<NUM_TEXTURES-56;i++) {
					storelfig(250+(i%7)*10,36+(i/7)*10,8,8,TextAr[i+56]);
					if (curstore.data == i+56)
						putlfig(250+(i%7)*10,36+(i/7)*10,8,8,selText);
				}
				changestr(3,strmenu02);
				break;
			case MLV_OBJECTS:
				for(i=0;i<NUM_OBJECTS;i++) {
					putlfig(250+(i%7)*10,36+(i/7)*10,8,8,TextAr[i+NUM_TEXTURES]);
					if (curstore.data == i+NUM_TEXTURES)
						putlfig(250+(i%7)*10,36+(i/7)*10,8,8,selText);
				}
				changestr(3,strmenu1);
				break;
			case MLV_ACTORS:
				for(i=0;i<NUM_ENEMIES+12;i++) {
					putlfig(250+(i%7)*10,36+(i/7)*10,8,8,TextAr[i+NUM_TEXTURES+NUM_OBJECTS]);
					if (curstore.data == i+NUM_TEXTURES+NUM_OBJECTS)
						putlfig(250+(i%7)*10,36+(i/7)*10,8,8,selText);
				}
				changestr(3,strmenu2);
				break;
			case MLV_TABLES:
				for(i=0;i<16;i++) {
					if (Level[i].active)
						storelfig(250+(i%7)*10,36+(i/7)*10,8,8,TableNum[i]);
					else
						storelfig(250+(i%7)*10,36+(i/7)*10,8,8,TableNum[16]);
					if (levelnum == i)
						putlfig(250+(i%7)*10,36+(i/7)*10,8,8,selText);
				}
				for(i=0;i<3;i++)
				  if (i == (Level[levelnum].backnum & 0x0f))
					storelfig(250+(i*10),96,8,8,TableNum[i+13]);
				  else
					storelfig(250+(i*10),96,8,8,TableNum[i]);
				i++;
				if (Level[levelnum].backnum & 0x80)
					storelfig(250+(i*10),96,8,8,udonText);
				else
					storelfig(250+(i*10),96,8,8,udoffText);
				i++;
				if (Level[levelnum].backnum & 0x40)
					storelfig(250+(i*10),96,8,8,rainonText);
				else
					storelfig(250+(i*10),96,8,8,rainoffText);
				changestr(3,strmenu3);
				break;
			default: error("changemenu","menu unknown");
		}
		oldrect=-1;
		vtransf(250,36,68,84);
	}
}

void menuselect(int x, int y) {
	int sel = y*7+x;
	switch(menulvl) {
		case MLV_TEXTURES1:
		case MLV_TEXTURES2:
			if (menulvl==MLV_TEXTURES2) sel+=56;
			if (sel<NUM_TEXTURES) {
				curstore.data = sel;
				curstore.shape= curshape;
				curstore.type = curtype;
				curstore.stop = 1;
				curstore.none = 0;
			}
			break;
		case MLV_OBJECTS:
			if (sel<NUM_OBJECTS) {
				curstore.data = NUM_TEXTURES+sel;
				curstore.stop = 0;
				curstore.none = 1;
			}
			break;
		case MLV_ACTORS:
			if (sel<NUM_ENEMIES+12) {
				curstore.data = NUM_TEXTURES+NUM_OBJECTS+sel;
				curstore.stop = 0;
				curstore.none = 1;
			}
			break;
		case MLV_TABLES:
			if (sel<16) {
				if (!Level[sel].active) Level[sel].active=1;
				 else if (sel==levelnum) Level[sel].active=0;
				changelevel(sel);
			} else
			if (sel>=42 && sel<45) {
				Level[levelnum].backnum = (Level[levelnum].backnum & 0xf0) | (sel-42);
			} else
			if (sel==46) {
				Level[levelnum].backnum ^= 0x80;
			} else
			if (sel==47) {
				Level[levelnum].backnum ^= 0x40;
			}
			break;
	}
	 




	updatesel();
}

char *input(const char *str, char *first, char maxchar, char filter) {
	char app[80];
	char nome[33] = "";
	byte c, len;
	int anim = 0;

	if (maxchar<1 || maxchar>32) maxchar=32;
	len = strlen(strcpy(nome,first));

	mouse.hide();
	while (1) {
		if (++anim>40) anim=0;
		ffillblock(4,191,208,8,0x72727272l);
		strcpy(app,str);
		strcat(app,nome);
		if (anim>20) strcat(app,"_");
		litfont->write(4,192,127,117,app);
		ftransfblock(4,191,208,8,VMEMPTR);
		waitvsync();
		if (ShutDown) {
			mouse.show();
			return NULL;
		}
		if (kbhit())
		switch (c = getch()) {
			case 27:
				mouse.show();
				return NULL;
			case 13:
				mouse.show();
				return strcpy(first,nome);
			case 8: if (len) nome[--len]=0; break;
			default:
				if (filter) {
					c = toupper(c);
					if (c>='0' && c<='Z') {
						if (len<maxchar) {
							nome[len++]=c;
							nome[len]=0;
						}
					}
				} else {
					if (c>=' ' && c<=(byte)'\x97') {
						if (len<maxchar) {
							nome[len++]=c;
							nome[len]=0;
						}
					}
				  }
				break;
		}
	}
}

void findplayerpos() {
	TMapInfo *mptr = map;
	int x, y, i;

	fwfill(playerpos,0xffff,sizeof(playerpos));
	for (x=0;x<64;x++)
	 for (y=0;y<64;y++,mptr++)
		if ((*mptr).data>=NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES) {
			i = ((int)(*mptr).data-(NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES))>>2;
			playerpos[levelnum][i].x = x;
			playerpos[levelnum][i].y = y;
		}
}

void LoadFile() {
	int handle,
		i = 0,j;
	char id[9] = "........";
	char app[13];
	byte num;

	strcpy(app,filename);
	strcat(app,".DLF");

	if ((handle = CRC_openread(app)) >= 0) {
		CRC_read(handle,id,8);
		if (!fwcomp(id,(void *)&idDLF,8)) {
			CRC_close(handle);
			return;
		}
		for (i=0;i<16;i++) Level[i].active=0;
		CRC_read(handle,&num,sizeof(num));
		for (j=0;j<num;j++) {
			CRC_read(handle,&i,1);
			Level[i].active = 1;
			changelevel(i);
			CRC_read(handle,&Level[levelnum].name,24);
			CRC_read(handle,&Level[levelnum].backnum,1);
			 
			CRC_read(handle,map,8192);
			CRC_read(handle,floormap,4096);
			CRC_read(handle,ceilmap,4096);
			 



			findplayerpos();
		}
		CRC_read(handle,&ddfnum,sizeof(ddfnum));
		alloc_ddf(ddfnum);
		if (ddfnum) CRC_read(handle,ddf,ddfnum<<2);
		CRC_close(handle);
		statuswrite("Ok.");
		levelnum=-1;
		changelevel(0);
		menulvl=-1;
		changemenu(MLV_TABLES);
	} else statuswrite("Failed!");
}

void SaveFile() {
	int handle,i,
		oldlev = levelnum;
	char app[13];

	strcpy(app,filename);
	strcat(app,".DLF");

	 
	 

	if ((handle = CRC_openwrite(app)) >= 0) {
		CRC_write(handle,idDLF,8);
		byte num = 0;
		for (i=0;i<16;i++) if (Level[i].active) num++;
		CRC_write(handle,&num,sizeof(num));
		for (i=0;i<16;i++)
			if (Level[i].active) {
				CRC_write(handle,&i,1);
				changelevel(i);
				CRC_write(handle,&Level[levelnum].name,24);
				CRC_write(handle,&Level[levelnum].backnum,1);
				 
				CRC_write(handle,map,8192);
				CRC_write(handle,floormap,4096);
				CRC_write(handle,ceilmap,4096);
			}
		CRC_write(handle,&ddfnum,sizeof(ddfnum));
		if (ddfnum) CRC_write(handle,ddf,ddfnum<<2);
		CRC_close(handle);
		changelevel(oldlev);
		statuswrite("Ok.");
	} else statuswrite("Failed!");
}

int kaos_editor_main() {
	char ch = 0;
	int i,mx = 0,omx,my = 0,omy;
	byte mb = 0,omb;
	char moved, pushed;
	int x,y;

	initL();
	kaos_editor_win32::set_title("KAOS Level Editor");
	litfont = new (std::nothrow) Font("little");
	if (!litfont) error("main", "out of memory creating editor font");
	fadeout(pal,0,256,1);
	setvbuff(editor_vpage);
	if (!loadLBM("LBM/EDITOR.LBM",pal))
		error("main","cannot load LBM/editor.lbm");
	ftransfscreen(VMEMPTR);
	fwfill(pal,0,3);
	setvbuff(editor_vpage);
	kaos_editor_win32::present(VMEMPTR);
	fadein(pal,0,256,30);
	if (!mouse.install()) error("main","mouse driver needed");
	litfont->setdispl(1,0);

	addrect(0,0,80,32,strKaosfirm);			 
	addrect(80,0,32,16,strLoad);             
	addrect(80,16,32,16,strSave);            
	addrect(250,36,68,84,strmenu01);         
	addrect(4,36,240,152,strclickput);       
	addrect(224,0,16,16,strshape0);			 
	addrect(240,0,16,16,strshape1);			 
	addrect(256,0,16,16,strshape2);			 
	addrect(272,0,16,16,strshape3);			 
	addrect(288,0,16,16,strshape4);			 
	addrect(304,0,16,16,strshape5);			 
	addrect(224,16,16,16,strtype0);			 
	addrect(240,16,16,16,strtype1);			 
	addrect(256,16,16,16,strtype2);			 
	addrect(272,16,16,16,strtype3);			 
	addrect(180,0,32,10,strceiling);		 
	addrect(180,10,32,12,strwalls);			 
	addrect(180,22,32,10,strfloor);			 
	addrect(160,0,20,18,strtable);			 
	addrect(160,18,20,14,strmap);			 
	addrect(168,8,8,8,strlname);			 
	addrect(136,16,24,16,strgetmap);		 
	addrect(136,0,24,16,strnoobj);			 
	addrect(112,0,24,16,strnounder);         
	addrect(112,16,24,16,strfill);           

	getlfig(4,36,8,8,nullText);
	menuText = new char[68*84];
	getlfig(250,36,68,84,menuText);
	setvbuff(VMEMPTR);
	if (!loadLBM("LBM/NICTEST.LBM",NULL))
		error("main","cannot load LBM/NICTEST.LBM");
	setvbuff(VMEMPTR);

	crsr_std.hotx = 0; crsr_std.hoty = 0;
	crsr_std.dx = 16; crsr_std.dy = 16;
	crsr_std.data = new char[16*16];
	getlfig(0,88,16,16,crsr_std.data);
	mouse.setcursormem(&crsr_std,undermem);
	crsr_get.hotx = 0; crsr_get.hoty = 11;
	crsr_get.dx = 16; crsr_get.dy = 16;
	crsr_get.data = new char[16*16];
	getlfig(16,88,16,16,crsr_get.data);

	fwfill(playerpos,0xffff,sizeof(playerpos));

	for(i=0;i<NUM_TEXTURES;i++) {
		TextAr[i] = new char[64];
		getlfig((i%40)<<3,(i/40)<<3,8,8,TextAr[i]);
	}
	for(i=0;i<NUM_OBJECTS;i++) {
		TextAr[i+NUM_TEXTURES] = new char[64];
		getlfig((i%40)<<3,((i/40)<<3)+24,8,8,TextAr[i+NUM_TEXTURES]);
	}
	for(i=0;i<NUM_ENEMIES;i++) {
		TextAr[i+NUM_TEXTURES+NUM_OBJECTS] = new char[64];
		getlfig(i<<3,40,8,8,TextAr[i+NUM_TEXTURES+NUM_OBJECTS]);
	}
	for(i=0;i<12;i++) {
		TextAr[i+NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES] = new char[64];
		getlfig(i<<3,48,8,8,TextAr[i+NUM_TEXTURES+NUM_OBJECTS+NUM_ENEMIES]);
	}

	for (i=0;i<6;i++) {
		MaskAr[i] = new char[64];
		getlfig(i<<3,64,8,8,MaskAr[i]);
	}
	getlfig(6<<3,64,8,8,door1Text);
	getlfig(7<<3,64,8,8,door2Text);
	getlfig(8<<3,64,8,8,gridText);

	for (i=0;i<17;i++) {
		TableNum[i] = new char[64];
		getlfig(i<<3,56,8,8,TableNum[i]);
	}
	getlfig(i++<<3,56,8,8,selText);
	getlfig(i++<<3,56,8,8,udoffText);
	getlfig(i++<<3,56,8,8,udonText);
	getlfig(i++<<3,56,8,8,rainoffText);
	getlfig(i++<<3,56,8,8,rainonText);
	setvbuff(editor_vpage);
	ftransfscreen(VMEMPTR);

	mm_init();
	for (i=0;i<16;i++) {
		Level[i].active = 0;
		fdfill(mm_recall(Level[i].wallidx = mm_alloc(8192)),0l,8192);
		fdfill(mm_recall(Level[i].flooridx = mm_alloc(4096)),0l,4096);
		fdfill(mm_recall(Level[i].ceilidx = mm_alloc(4096)),0l,4096);
		mm_reset();
	}
	getlfig( 0,104,24,16,mm_recall(p_noobj = mm_alloc(24*16)));
	getlfig(24,104,24,16,mm_recall(p_nounder = mm_alloc(24*16)));

	setvbuff(editor_vpage);

	Level[0].active = 1;
	changelevel(0);
	changelvl(LVL_WALL);
	changemenu(MLV_TEXTURES1);
	menuselect(0,0);
	waitvsync();

	mouse.show();
	do {
		command = 0;
		omx=mx; omy=my; omb=mb;
		mouse.get(mx,my,mb);
		moved = (mx!=omx)||(my!=omy);
		pushed= mb && mb!=omb;
		if ((oldrect==-1 || moved) && (i = recthit(mx,my)) != oldrect) {
			oldrect = i;
			ffillblock(4,191,208,8,0x72727272l);
			if (i!=-1)
				litfont->write(4,192,127,117,HotArray[i].descr);
			vtransf(4,191,208,8);
		}
		if (pushed) {
			i=1;
			switch(oldrect) {
				case 4: i=0; break;
				case 3:	 
					if (mb==MB_LEFT) {
						x = (mx-250)/10;
						y = (my-36)/10;
						menuselect(x,y);
					} else
						if (mb==MB_RIGHT)
							if (menulvl<MLV_TABLES)
								changemenu((menulvl+1) % MLV_TABLES);
							else changemenu(MLV_TEXTURES1);
					do {
				mouse.get(mx,my,mb);
				if (mb && !ShutDown) waitvsync();
			} while(mb && !ShutDown);
					break;
				case 5:  
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
					curshape = oldrect-5;
					curstore.shape = curshape;
					updatesel();
					break;
				case 11:	 
					curtype = MTP_WALL;
					curstore.type = curtype;
					updatesel();
					break;
				case 12:	 
					curtype = MTP_DOOR1;
					curstore.type = curtype;
					updatesel();
					break;
				case 13:	 
					curtype = MTP_DOOR2;
					curstore.type = curtype;
					updatesel();
					break;
				case 14:	 
					curtype = MTP_GRID1;
					curstore.type = curtype;
					updatesel();
					break;
				case 15: command = COM_LEVELC; break;
				case 16: command = COM_LEVELW; break;
				case 17: command = COM_LEVELF; break;
				case 18: changemenu(MLV_TABLES); break;
				case 1:	command = COM_LOAD; break;
				case 2: command = COM_SAVE; break;
				case 20:  
					if (input("Name: ",Level[levelnum].name,23,0)) {
						strcpy(strlname,"Change name (");
						strcat(strlname,Level[levelnum].name);
						strcat(strlname,")");
						oldrect=-1;
					}
					break;
				case 21:
					if (!getmode) {
						mouse.setcursormem(&crsr_get,undermem);
						changestr(4,strgetsee);
						getmode++;
					}
					break;
				case 22: command = COM_NOOBJ; break;
				case 23: command = COM_NOUNDER; break;
				case 24:
					for (int fx=0;fx<64;fx++)
					 for (int fy=0;fy<64;fy++)
					   store(fx,fy);
					break;
				default: i=0;
			}
			if (i) {
				while(mb && !ShutDown) {
					mouse.get(mx,my,mb);
					if (mb) waitvsync();
				}
				pushed=0;
			}
		}
		if (pushed || (moved && mb)) {
			i=0;
			if (oldrect==4)	{	 
				x = ((mx-4)>>3)+viewleft;
				y = ((my-36)>>3)+viewtop;
			  if (getmode) {
				i = (x<<6)+y;
				switch (curlvl) {
					case LVL_WALL: curstore = map[i]; break;
					case LVL_FLOOR: curstore.data = floormap[i]; break;
					case LVL_CEILING: curstore.data = ceilmap[i]; break;
				}
				updatesel();
				i = (mb==MB_LEFT);
			  } else {
				if (mb==MB_LEFT) store(x,y); else
					if (mb==MB_RIGHT) {
						TMapInfo app = curstore;
						curstore = nullinf;
						store(x,y);
						curstore = app;
					}
			  }
			}
			if (getmode && i) {
				mouse.setcursormem(&crsr_std,undermem);
				changestr(4,strclickput);
				getmode=0;
				oldrect=-1;
			}
		}
		if (kbhit()) {
			ch=toupper(getch());
			switch(ch) {
				case 0:
					ch=getch();
					switch(ch) {
						case 72: scroll(0,-2); break;
						case 80: scroll(0,2); break;
						case 75: scroll(-4,0); break;
						case 77: scroll(4,0); break;
					}
					break;
				case '1': command = COM_LEVELF; break;
				case '2': command = COM_LEVELW; break;
				case '3': command = COM_LEVELC; break;
				case 'L': command = COM_LOAD; break;
				case 'S': command = COM_SAVE; break;
				case 'O': command = COM_NOOBJ; break;
				case 'U': command = COM_NOUNDER; break;
			}
		}
		switch (command) {
			case 0: break;  
			case COM_LOAD:
				if (input("Load file: ",filename,8,1))
						LoadFile();
					else statuswrite("Abort load.");
				break;
			case COM_SAVE:
				if (input("Save file: ",filename,8,1))
						SaveFile();
					else statuswrite("Abort save.");
				break;
			case COM_LEVELC:
			case COM_LEVELW:
			case COM_LEVELF: changelvl(2+COM_LEVELC-command); break;
			case COM_NOOBJ:
				noobj = !noobj;
				if (noobj) {
					setvbuff(VMEMPTR);
					mouse.hide();
					putlfig(136,0,24,16,mm_recall(p_noobj));
					mouse.show();
					setvbuff(editor_vpage);
				} else vtransf(136,0,24,16);
				paintrect(viewleft,viewtop,30,19);
				break;
			case COM_NOUNDER:
				nounder = !nounder;
				if (nounder) {
					setvbuff(VMEMPTR);
					mouse.hide();
					putlfig(112,0,24,16,mm_recall(p_nounder));
					mouse.show();
					setvbuff(editor_vpage);
				} else vtransf(112,0,24,16);
				if (curlvl!=LVL_WALL) paintrect(viewleft,viewtop,30,19);
				break;
		}
		waitvsync();
	} while (ch!=27 && !ShutDown);
	if (litfont) { delete litfont; litfont = NULL; }
	mm_done();
	fadeout(pal,0,256,30);
	doneL();
	
	return 0;
}