#pragma once
 






#define	MM_MAXALLOC		4096
#define	MAXMEMBLOCKS 	581
#ifdef __DATAFILE__
#define	MAXXMSBLOCKS    480
#else
#define	MAXXMSBLOCKS    768
#endif

#define	DATAFILENAME	"KAOS.BDF"

#define	BLF_USED	0x01
#define	BLF_XMS		0x02
#define	BLF_DISK	0x04
#define BLF_LOCKED	0x08

#define	BLF_UNREADY	(BLF_XMS|BLF_DISK)
#define BLF_UNUSED	0xfe

#ifdef __MAINCHECK__
extern long mm_mainmem;
#endif

extern char *mm_reserve(word size);
 
extern int mm_alloc(word size);
extern word mm_getblocksize(int id);
extern void *mm_recall(int id);
extern void mm_unload(int id);
extern byte mm_lock(int id);
extern byte mm_unlock(int id);
extern unsigned long mm_memavail();
extern unsigned long mm_totalloc();
extern unsigned long mm_freealloc();
extern void mm_reset();
extern void mm_resetall();
extern void mm_flushall();
extern void mm_init();
extern void mm_done();
#ifdef __VISUALIZE__
extern void mm_visualize();  
#endif
 


 
#ifdef __MAKEDATA__
extern void mm_makedatafile();
#endif