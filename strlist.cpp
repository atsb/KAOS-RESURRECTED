 






 

#include "std.hpp"
#include "fastmem.hpp"
#include "mm4.hpp"
#include "strlist.hpp"
#include "platform_compat.hpp"
#include <fcntl.h>

#define	ENDCHAR		''

 
#ifndef __DATAFILE__
int sl_convert(const char *slfile) {
	int handle;
	word size = 0;
	char c;
	char far *p, far *app;
	if ((handle = _open(slfile,O_RDONLY|O_BINARY))>=0) {
		app = p = (char *)mm_reserve(filelength(handle));
		do {
			_read(handle,&c,1);
			if (c==10) continue; else	 
			if (c==13) c=0;				 
			*(app++) = c;
			size++;
			if (c==ENDCHAR) break;			 
		} while (!eof(handle));
		if (c!=ENDCHAR) {	 
			*(app++) = ENDCHAR;
			size++;
		}
		_close(handle);

		handle = mm_alloc(size);
		fdmove(mm_recall(handle),p,size);
		delete p;
		return handle;
	}
	error("sl_convert","can't convert");
	return -1;
}
#endif

#pragma warn -rvl
void far *sl_find(void far *list, int n) {
    if (!list || n < 0) return nullptr;

    byte *p = static_cast<byte *>(list);
    for (int i = 0; i < n; ++i) {
        while (*p != ENDCHAR && *p != 0)
            ++p;
        if (*p == ENDCHAR)
            return nullptr;
        ++p;
    }

    if (*p == ENDCHAR)
        return nullptr;
    return p;
}
#pragma warn +rvl
 
