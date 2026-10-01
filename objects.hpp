#pragma once
 






#ifndef __OBJECTS__
#define __OBJECTS__

#include <cstdint>

class Object;
class ObjectsList;
class ActorsList;
class InfoBar;
class LittleInfoBar;

#define OFL_SEEN		0x01
#define	OFL_ONESEEN		0x02
#define	OFL_ISACTOR		0x04
#define	OFL_TAKEABLE  	0x08
#define	OFL_REVERSE  	0x10
#define	OFL_ISLIGHT		0x20
#define	OFL_HASLIGHT	0x40

#define MVT_0	0	 
#define MVT_1 	1	 
#define MVT_2   2    
 

#define	MAXARMS			6
#define	ARMCHGSPEED		36
 
#define AFL_ASCIA		0x01
#define AFL_PISTOLA		0x02
#define AFL_FUCILE		0x04
#define AFL_MITRA		0x08
#define AFL_BAZOOKA		0x10
#define AFL_BOMB		0x20
 

 
#define	ARM_ADDPISTOLA	20
#define ARM_ADDFUCILE	4
#define	ARM_ADDMITRA	24
#define ARM_ADDBAZOOKA	2
#define	ARM_ADDBOMB		1

typedef	struct {
			byte flag, power, chargetime;
			std::int16_t pre_ammo, max_ammo;
		} TArm;
extern TArm ArmList[MAXARMS];

 
struct kaos_from_file { explicit kaos_from_file(int h) : handle(h) {} int handle; };

 

class Mover
{
	protected:
		friend Object;
		byte mover_type;
		void enqueue(int idx, std::int16_t &undr);
		void dequeue(int idx, std::int16_t undr);
	public:
		std::int16_t id;
		std::int16_t flags;
		std::int16_t index;
		std::int16_t under;
		char shdim;
		fixed x, y, z;
		std::int16_t angle;
		Mover(fixed _x=0, fixed _y=0, fixed _z=0, int _shdim=5,
			  word _flags=OMF_STANDARD) :
			  x(_x), y(_y), z(_z), id(0), shdim(_shdim), flags(_flags),
			  mover_type(MVT_0) {};
		explicit Mover(kaos_from_file src);
		virtual void save(int handle);
		void setid(int the_id) {
			if (!id) id = the_id;
			else error("Object::setid","multiple IDs");
		}
		virtual char canmove(fixed nx, fixed ny);
		virtual void place();
		virtual void remove();
		virtual int underq(int idx) {return under;};
		virtual void correctq(int idx, int undid);
		virtual char move(int ang, int lshift);
		virtual char fluidmove(int ang, fixed mult);
};

class Mover1 : public Mover
{
	public:
		Mover1(fixed _x=0, fixed _y=0, fixed _z=0, int _shdim=5,
			  word _flags=OMF_STANDARD) :
			  Mover(_x,_y,_z,_shdim,_flags) {mover_type=MVT_1;};
		explicit Mover1(kaos_from_file src):Mover(src) {};
		virtual char move(int ang, int lshift);
		virtual char fluidmove(int ang, fixed mult);
};

class Mover2 : public Mover1	 
{
	protected:
		std::int16_t index1;
		std::int16_t under1;
	public:
		Mover2(fixed _x=0, fixed _y=0, fixed _z=0, int _shdim=5,
			  word _flags=OMF_STANDARD)
			  :Mover1(_x,_y,_z,_shdim,_flags) {mover_type=MVT_2;};
		explicit Mover2(kaos_from_file src);
		virtual void save(int handle);
		virtual void place();
		virtual void remove();
		virtual int underq(int idx);
		virtual void correctq(int idx, int undid);
		 
		 
};

 










class Object
{
		char doblood(int angle);
	protected:
		byte object_type;
		friend ObjectsList;	 
		virtual void setid(int _id) {mover->setid(_id);};
	public:
		Mover *mover;
		char oflags, selflight;
		std::int16_t figstart;
		char fignum;
		Object(fixed px, fixed py, int startfig = 0,
			   char numfig = 1, byte movertype = MVT_0);
		Object(int handle);  
		virtual void save(int handle);
		virtual ~Object();
		int getid()	{return mover->id;};
		 
		void changefig(int startfig, char numfig);
		virtual int getviewfig(int ang);	 
		virtual void place() {mover->place();};
		virtual void remove() {mover->remove();};
		virtual void handle_event(TEvent &event);

		char have_seen()	{return (oflags & OFL_SEEN);};
		char had_seen()		{return (oflags & OFL_ONESEEN);};
		void seen() 		{oflags |= OFL_SEEN | OFL_ONESEEN;};
		void reset()		{oflags &= ~OFL_SEEN;};

		byte get_type() 	{return object_type;};
};

class Actor : public Object
{
	protected:
		std::int16_t animcount;
		virtual void setid(int the_id);
		virtual char makeActor();
		virtual char makeObject();
		char nearPlayer(fixed maxdist);
	public:
		std::int16_t health;
		Actor(fixed px, fixed py, int startfig = 0, char numfig = 1,
			  byte movertype = MVT_0)
			:Object(px,py,startfig,numfig,movertype),
			animcount(0), health(1) {object_type=OBJT_ACTOR;};
		Actor(int handle);  
		virtual void save(int handle);
		virtual ~Actor();
		virtual int getviewfig(int ang);
		virtual void animate() {};
		virtual void draw(int vidx, int vidy, char &cd, char &fd);
		virtual void virtualdraw();
		virtual void transf(int vidx, int vidy);
		 
};

class LightActor : public Actor
{
	protected:
		char lightrange;
		void lights();
		void unlights();
	public:
		fixed lightz;
		char lightpow;
		LightActor(fixed px, fixed py, int startfig = 0, char numfig = 1,
				   char lr = 0, char self = 0, byte movertype = MVT_0) :
				Actor(px,py,startfig,numfig,movertype),
				lightrange(lr) {
					selflight=self;
					object_type=OBJT_LIGHTACTOR;
					lightz = 0;
					lightpow = 0;
					SETFLAG(oflags,OFL_HASLIGHT);
				};
		LightActor(int handle);  
		virtual void save(int handle);
		~LightActor();
		virtual void place();
		virtual void remove();
		virtual int getlightfig() { return -1; };
};

class CanFireActor : public LightActor
{
	friend InfoBar;
	friend LittleInfoBar;
	protected:
		word armflag;
		byte oldarm, curarm, armcharge, armchanging;
		std::int16_t Armbull[MAXARMS-1];
		virtual char setarm(byte armtype);
		virtual char fire(int angle);
	public:
		CanFireActor(fixed px, fixed py, int startfig = 0,
					 char numfig = 1, byte movertype = MVT_0);
			 




		CanFireActor(int handle);  
		virtual void save(int handle);
		virtual void animate();
		virtual char takearm(byte armtype);
		virtual char takeammo(byte armtype, int ammo);
};

class ObjectsList
{
		friend ActorsList;
		Object *objlist[MAXOBJECTS];
		int objnum;
	public:
		ObjectsList();
		~ObjectsList();
		void loadobj(Object *object, byte objtype);  
		void save(int handle);
		void put(Object *object);
		Object *get(int objidx);
		void erase(int objidx);
		void eraseall();
		void handle_event(TEvent &event);
		void preloadall();
};

class ActorsList
{
		int actorslist[MAXACTORS];
		int actorsnum;
	public:
		ActorsList() : actorsnum(0) {};
		void add(int objidx);
		void erase(int objidx);
		void eraseall();
		void animate();
		void handle_event(TEvent &event);
		void reset();
};

class DrawList
{
		int drawlist[MAXDRAWING];
		fixed distlist[MAXDRAWING];
		byte drawnum;
	public:
		DrawList() : drawnum(0) {};
		char not_empty() {return drawnum;};
		void flush() {drawnum=0;};
		void add(int objidx, fixed objdist);
		int get(fixed &objdist);
};

extern ObjectsList objectslist;
extern ActorsList actorslist;

#endif 	 