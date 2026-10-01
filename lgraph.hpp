#pragma once
#ifndef KAOS_LGRAPH_HPP
#define KAOS_LGRAPH_HPP

#include "std.hpp"
#include <cstdint>

#define MakeID(d,c,b,a) ((std::uint32_t)(a)<<24 | (std::uint32_t)(b)<<16 | (std::uint32_t)(c)<<8 | (std::uint32_t)(d))
#define CK_FORM MakeID('F','O','R','M')
#define ID_PBM  MakeID('P','B','M',' ')
#define ID_ILBM MakeID('I','L','B','M')
#define ID_BMHD MakeID('B','M','H','D')
#define ID_CMAP MakeID('C','M','A','P')
#define ID_BODY MakeID('B','O','D','Y')

struct form_chunk {
    std::uint32_t ID_type;
    std::uint32_t cksize;
    std::uint32_t ID_subtype;
};

struct chunk_header {
    std::uint32_t ID_chunk;
    std::uint32_t cksize;
};

struct bitmap_header {
    unsigned short width, height;
    unsigned short x, y;
    byte nPlanes, masking, compression, no;
    unsigned short transparent;
    byte x_aspect, y_aspect;
    unsigned short p_width, p_height;
};

extern byte *vbuff;
extern byte *vpage;  
extern byte *kaos_screen_buffer();

#define VMEMPTR (kaos_screen_buffer())

extern word xytodisp(word x, word y);
extern void setvrect(int x, int y, int dx, int dy);
extern void initL();
extern void doneL();
extern void setvbuff(void *buff);
extern void putpix(word x, word y, byte col);
extern byte getpix(word x, word y);
extern void fillpage(byte col);
extern void ffillscreen(long col);
extern void ffillarea(word y, word dey, long col);
extern void ftransfscreen(void *dest);
extern void ftransfarea(word y, word dey, void *dest);
extern void fcopyarea(word y, word y1, word dey, void *dest);
extern void ffillblock(word x, word y, word dex, word dey, long col);
extern void ftransfblock(word x, word y, word dex, word dey, void *dest);
extern void fcopyblock(word x, word y, word x1, word y1,
                       word dex, word dey, void *dest);
extern void getlfig(word x, word y, word dex, word dey, void *data);
extern void getrfig(word x, word y, word dex, word dey, void *data);
extern void storelfig(word x, word y, word dex, word dey, void *data);
extern void storerfig(word x, word y, word dex, word dey, void *data);
extern void putlfig(word x, word y, word dex, word dey, void *data);
extern void putrfig(word x, word y, word dex, word dey, void *data);
extern void putlchar(word x, word y, word dex, word dey,
                     byte col1, byte col2, void *data);
extern void putlschar(word x, word y, word dex, word dey,
                      byte addcol, void *data);
extern void getlvfig(int x, int y, word dex, word dey, void *data);
extern void storelcfig(int x, int y, word dex, word dey, void *data);
extern void putlcfig(int x, int y, word dex, word dey, void *data);
extern void putlcchar(int x, int y, word dex, word dey,
                      byte col1, byte col2, void *data);
extern void scalerscfig(int x, int y, word dex, word dey,
                        int vdx, int vdy, char shade, void *rfig);
extern void storelmfig(word x, word y, word dex, word dey,
                       void *mask, void *data);
extern char loadLBM(const char *filename, void *pal);

#endif
