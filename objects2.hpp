#pragma once
#include <cstdint>
 






 
#define BOOM_PALLOT		0	 
#define	BOOM_MISSIL		1    
#define	BOOM_BLOOD		2    
#define	BOOM_SLIMER		3    
#define	BOOM_SPIDER		4	 
#define	BOOM_ASCIA		5	 
#define	BOOM_SMOG		6	 

class Ossa : public Actor
{
		std::int16_t animtrig;
		void launch(int angle, int strong);
	protected:
		virtual void setid(int the_id)
			{Object::setid(the_id);};
	public:
		Ossa(fixed px, fixed py, int type);
		Ossa(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class FlOssa : public Actor
{
		std::int16_t speed, bang;
	public:
		FlOssa(fixed px, fixed py, int angle, int _speed);
		FlOssa(int handle);
		virtual void save(int handle);
		virtual void animate();
};

class ObjMover : public Actor
{
		std::int16_t speed, bounceang;
		std::int16_t objlinkID;
	public:
		ObjMover(int _objlinkID, int angle, int _speed);
		ObjMover(int handle);
		 
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class Bonus : public LightActor
{
		std::int16_t animtrig, animang;
	public:
		Bonus(fixed px, fixed py);
		Bonus(int handle);
		virtual void animate();
		virtual int getlightfig();
		 
};

class AntiKaos : public LightActor
{
		std::int16_t animtrig, animang;
	public:
		AntiKaos(fixed px, fixed py);
		AntiKaos(int handle);
		virtual void animate();
		virtual int getlightfig();
		virtual void handle_event(TEvent &event);
};

class Enemy : public Actor
{
	protected:
		std::int16_t posneg;
		std::int16_t animtrig, animstate;
		std::int16_t firstfig, walkspeed, speed, hitangle;
	public:
		Enemy(fixed px, fixed py, byte type);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class Slimer : public Actor
{
	protected:
		std::int16_t animtrig, animstate;
		std::int16_t speed, hitangle;
		std::int16_t objlink;
		std::int16_t cannoncharge, scanangle, mysound, floatangle;
		char objseen,  posneg, sndcount;
		char turnnum, turning, turnable;
	public:
		Slimer(fixed px, fixed py);
		Slimer(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class Spider : public Actor
{
	protected:
		std::int16_t animtrig, animstate;
		std::int16_t speed, hitangle;
		std::int16_t scanangle, objlink;
		std::int16_t mysound, cannoncharge;
	public:
		Spider(fixed px, fixed py);
		Spider(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class Soldier : public Actor
{
	protected:
		std::int16_t animstate, animtrig;
		std::int16_t speed, hitangle;
		std::int16_t scanangle, zigzagang, objlink;
		char objseen, recover;
		std::int16_t turncount,havetoturn;
		fixed oldobjx,oldobjy;
	public:
		Soldier(fixed px, fixed py);
		Soldier(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class Mouse : public Actor
{
	protected:
		std::int16_t animstate, animtrig;
	public:
		Mouse(fixed px, fixed py);
		Mouse(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class FireBoom : public LightActor
{
		std::int16_t animtrig;
		byte type;
		fixed decay, decayspd;
	public:
		FireBoom(fixed px, fixed py, fixed h, int angle, byte boomtype);
		FireBoom(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual int getlightfig();
};

class Fireball : public LightActor
{
		std::int16_t ownerid;
		fixed deh;
		char sendsound, animtrig;
		std::int16_t   shspeed, boomcount;
	public:
		Fireball(fixed px, fixed py, int fireangle,   int type,
				 fixed h1, fixed h2, fixed dist, int _ownerid);
		Fireball(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &evnt);
		virtual int getlightfig();
};

class Bomb : public Actor
{
		std::int16_t ownerid, timer, animtrig, strenght;
		fixed decay;
	public:
		Bomb(fixed px, fixed py, fixed h, int angle, int strng,
			int _ownerid);
		Bomb(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &evnt);
		int getownerid() { return ownerid; }
};

class Torcia : public LightActor
{
		char animtrig;
		std::int16_t fireid;
	public:
		Torcia(fixed px, fixed py, int fig);
		Torcia(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
		virtual int getlightfig();
};

class Lampadario : public LightActor
{
		char animtrig;
		fixed decay;
	public:
		Lampadario(fixed px, fixed py, int fig);
		Lampadario(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
		virtual int getlightfig();
};

class Barile : public LightActor
{
		std::int16_t animtrig, speed;
		std::int16_t ownerid;
	protected:
		virtual void setid(int the_id)
			{Object::setid(the_id);};
	public:
		Barile(fixed px, fixed py);
		Barile(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};

class Vaso : public Actor
{
		std::int16_t animtrig, speed;
		byte whatin;
	protected:
		virtual void setid(int the_id)
			{Object::setid(the_id);};
	public:
		Vaso(fixed px, fixed py, int fig, byte objin);
		Vaso(int handle);
		virtual void save(int handle);
		virtual void animate();
		virtual void handle_event(TEvent &event);
};