#pragma once
 






typedef char *PPal;

extern void waitvsync();
extern void setpal(PPal pal, word fromcol, word numcol);
extern void palfilter(PPal pal, byte r, byte g, byte b);
extern void fadein(PPal pal, word fromcol, word numcol, word numcicli);
extern void fadeout(PPal pal, word fromcol, word numcol, word numcicli);
extern void palmorph(PPal initpal, PPal endpal,
			  word fromcol, word numcol, word numcicli);
extern void fadeinpart(PPal pal, word fromcol, word numcol,
				word ciclo, word numcicli);
extern void fadeoutpart(PPal pal, word fromcol, word numcol,
				 word ciclo, word numcicli);
extern void palmorphpart(PPal initpal, PPal endpal,
			  word fromcol, word numcol, word ciclo, word numcicli);