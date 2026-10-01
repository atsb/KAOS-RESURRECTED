 






#include "std.hpp"
#include "mp.hpp"
#include "platform_compat.hpp"

void main() {
	clrscr();
	mp_init();
	if (mp_active) {
		mp_StartMusic("c:\\music\\mid\\lastdays.mid");
		cprintf("Tick per quarter %ld\r\n",mp_tickperquarter);
		while (!kbhit() && mp_playing) {
			cprintf("%ld        \r",mp_waitdown);
			mp_idle();
		}
		mp_StopMusic();
		mp_done();
	}
}