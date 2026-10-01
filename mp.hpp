#pragma once
 






extern char mp_active, mp_playing;

extern void mp_StartMusic(const char *filename);
extern void mp_StopMusic();
extern void mp_init();
extern void mp_done();
extern void mp_idle();

extern long mp_totalwait,
	mp_waitdown,
	mp_timeslice,
	mp_tickperquarter,
	mp_microperquarter,
	mp_micropertick;
