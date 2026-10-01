#pragma once
 






extern void fmove(void *dest, void *sorg, word bytes);
extern void fwmove(void *dest, void *sorg, word bytes);
extern void fdmove(void *dest, void *sorg, word bytes);
extern void fowmove(void *dest, void *sorg, word bytes);
extern void fodmove(void *dest, void *sorg, word bytes);
extern void fwfill(void *dest, word val, word bytes);
extern void fdfill(void *dest, long val, word bytes);
extern char fwcomp(const void *s1, const void *s2, word len);
extern char fdcomp(const void *s1, const void *s2, word len);