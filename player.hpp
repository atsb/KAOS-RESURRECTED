#include <cstdint>
#include <cstddef>
#pragma once
class InfoBar;
class LittleInfoBar;
 






#define	COM_DIRECT		0
#define	COM_SAVE		1
#define	COM_LOAD		2
#define	COM_SEND		3
#define	COM_RECEIVE		4

 
#define PFL_GOD			0x01
#define	PFL_MAPVIEW     0x02
 
#define	GODMODE			(pflags & PFL_GOD)
#define	MAPVIEW			(pflags & PFL_MAPVIEW)

typedef struct {
			 
			std::int16_t health;
			byte curarm, keys, armflag;
			std::int16_t armbull[6];	 
		} TPlayerData;

static_assert(sizeof(TPlayerData) == 18,
			  "DOS player transfer record must remain 18 bytes");
static_assert(offsetof(TPlayerData, armbull) == 6,
			  "DOS player ammo array must retain its original offset");

class Player : public CanFireActor
{
		friend InfoBar;
		friend LittleInfoBar;

		TControl control;
		TControl command;
		char myseenmap[MAPDIM];
		std::int16_t animtrig;
		virtual char fire(int angle);
		std::int16_t speed,angspeed,latspeed,angwalk;
		std::int16_t hitspeed, hitangle, vangle, vangang;
		byte keys, oldkeys, controlnum;
		word oldarms;
		char pflags, upwalk, comtype;
		word oldpack;
		 
		 
		std::int16_t handang;  
		std::int16_t cecita, hardcount;  
		fixed hwalk;
		void makeboom(fixed x,fixed y, fixed z, int angle, int id, byte type);
		void updatecommands();
		word setpackbit(byte bit, byte val);
		char getpackbit(word pack, byte bit);
		word packcom();
		void unpackcom(word pack);
		void savepack(word pack);
		word loadpack();
		 



		 
		std::int32_t fires, hits, blood, tecno;
		std::int16_t kills, plkilled;
	public:
		Player(fixed px, fixed py, int ang, byte cntrlnum);
		Player(int handle);
		Player(fixed px, fixed py, int ang, TPlayerData &data,
			   byte cntrlnum);
		void setcomtype(char type);
		virtual void save(int handle);
		~Player();
		void moveto(fixed x, fixed y, int ang);
		virtual void animate();
		virtual void draw(int vidx, int vidy, char &cd, char &fd);
		virtual void transf(int vidx, int vidy);
		virtual void handle_event(TEvent &event);
		void getdata(TPlayerData &data);
		void updatestat(int type, int num);
		void getstats(std::int32_t &fires,std::int32_t &hits,std::int32_t &blood,
					  std::int32_t &tecno,int &kills);
};
