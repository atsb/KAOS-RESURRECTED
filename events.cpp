 






 

#include "std.hpp"
#include "events.hpp"
#include "fixed.h"

 
#include "kaos.hpp"
#include "objects.hpp"	 

char EventManager::get(TEvent &event) {
	if (num) {	 
		event = eventlist[head];
		if (++head >= MAXEVENTS) head = 0;
		num--;
		return 1;
	}
	return 0;
};

char EventManager::put(TEvent &event) {
	if (num < MAXEVENTS) {
		register int idx;
		if ((idx = head+(num++)) >= MAXEVENTS) idx -= MAXEVENTS;
		eventlist[idx] = event;
		return 1;
	}  
	return 0;
}

char EventManager::send(std::uint16_t what, std::int16_t sorg,
						std::int16_t dest, std::int16_t x, std::int16_t y,
						std::int16_t z, std::int16_t a, std::int16_t b) {
	TEvent evnt;
	evnt.what = what;
	evnt.source = sorg;
	evnt.data.msg.dest = dest;
	evnt.data.msg.x = x;
	evnt.data.msg.y = y;
	evnt.data.msg.z = z;
	evnt.data.msg.d1 = a;
	evnt.data.msg.d2 = b;
	return put(evnt);
}

char EventManager::sendshort(std::uint16_t what, std::int16_t sorg,
							 std::int16_t dest, std::int16_t x, std::int16_t y) {
	TEvent evnt;
	evnt.what = what;
	evnt.source = sorg;
	evnt.data.msg.dest = dest;
	evnt.data.msg.x = x;
	evnt.data.msg.y = y;
	return put(evnt);
}

char EventManager::senddirect(std::uint16_t what, std::int16_t sorg,
							  std::int16_t dest, std::int16_t x, std::int16_t y) {
	Object *obj = objectslist.get(dest);
	if (obj) {
		TEvent evnt;
		evnt.what = what;
		evnt.source = sorg;
		evnt.data.msg.dest = dest;
		evnt.data.msg.x = x;
		evnt.data.msg.y = y;
		 
		obj->handle_event(evnt);
		return 1;
	} else return 0;
}

char EventManager::sendcom(std::uint16_t what, std::int16_t sorg) {
	TEvent evnt;
	evnt.what = what;
	evnt.source = sorg;
	return put(evnt);
}

char EventManager::sendpos(std::uint16_t what, std::int16_t sorg,
						   fixed x, fixed y, fixed z) {
	TEvent evnt;
	evnt.what = what;
	evnt.source = sorg;
	evnt.data.pos.x = x;
	evnt.data.pos.y = y;
	evnt.data.pos.z = z;
	return put(evnt);
}

EventManager eventmanager;