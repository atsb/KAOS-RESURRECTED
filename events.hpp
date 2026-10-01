#pragma once
#include "fixed.h"
#include <cstddef>
 






#ifndef	__EVENTS__
#define __EVENTS__

#define MAXEVENTS       64

 
 





 
#define EV_ACKTAKE		0x01 	 
#define	EV_ERASEACT		0x02   	 
								 
								 
#define EV_UPDATEBAR	0x03	 
#define	EV_DEAD			0x04	 
#define	EV_RESURRECT	0x05	 
#define	EV_ENDPLAYBACK	0x06	 
#define	EV_CHGTEXTURE	0x07
#define	EV_ANTIKAOS		0x08	 
#define EV_MESSAGE		0x09	 
 
#define	EV_HIT			0x21   	 
								 
								 
#define	EV_GET			0x22
#define	EV_GOT			0x23
#define	EV_ADDSTAT		0x24	 
#define	EV_ENDSOUND		0x25	 
#define	EV_POISON		0x26	 
#define	EV_ALERT		0x27	 
								 
 
#define	EV_KILL			0x81
#define	EV_ACTIVATE		0x82
#define EV_PLACTIVATE	0x83
#define EV_SCODE		0x84
#define EV_BOOM			0x85
#define	EV_PLKILLED		0x86	 
#define EV_PLNOISE		0x87	 
 
#define EV_REQTAKE		0xC1

 





typedef struct {                                     
			std::uint16_t what;							 
			std::int16_t source;                              
			union {                                  
				struct {
					std::int16_t dest, x, y, z, d1, d2;
				} msg;
				struct {
					fixed x, y, z;
				} pos;
			} data;
		} TEvent;

 
static_assert(sizeof(TEvent) == 16, "TEvent must retain the 16-byte DOS layout");
static_assert(offsetof(TEvent, data) == 4, "TEvent payload must start after two DOS words");

class EventManager
{
		TEvent eventlist[MAXEVENTS];
		unsigned char head, num;
	public:
		EventManager() { flush(); };
		char ready() { return num; };
		char get(TEvent &event);
		char put(TEvent &event);
		char send(std::uint16_t what, std::int16_t sorg,
				  std::int16_t dest, std::int16_t x, std::int16_t y,
				  std::int16_t z, std::int16_t a, std::int16_t b);
		char sendshort(std::uint16_t what, std::int16_t sorg,
					   std::int16_t dest, std::int16_t x, std::int16_t y);
		char senddirect(std::uint16_t what, std::int16_t sorg,
						std::int16_t dest, std::int16_t x, std::int16_t y);
		char sendcom(std::uint16_t what, std::int16_t sorg);
		char sendpos(std::uint16_t what, std::int16_t sorg,
					 fixed x, fixed y, fixed z);
		void flush() { head = 0; num = 0; };
};

extern EventManager eventmanager;

#endif	 