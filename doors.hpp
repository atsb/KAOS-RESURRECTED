#pragma once
#include <cstdint>
 






#ifndef	__DOORS__
#define __DOORS__

 

typedef struct {
			std::int16_t idx, opennum, animcount, status;
		} TDoor;

class DoorManager
{
		TDoor *fldoorlist;
		std::int16_t doornum, fldoornum;
	public:
		byte *doorlist;
		 
		byte *doortype;
		DoorManager();
		~DoorManager() {
				delete fldoorlist;
				delete doortype;
				delete doorlist;
			}
		void load(int handle);
		void save(int handle);
		int add(byte image);
		void makeslots();
		char open(int idx, byte key);
		void animate();
		int doorpos(int indx);
		void eraseall();
		void handle_event(TEvent &event);
};

extern DoorManager doormanager;

#endif	 