 






 

#include "std.hpp"
#include "errorstr.hpp"
#include "fastmem.hpp"
#include "mm4.hpp"
#include "platform_compat.hpp"
#include <fcntl.h>
#include <string.h>
#include "std.hpp"
#include "lfont2.hpp"
#include "lgraph.hpp"
#include "crcio.hpp"

const char idLFN[9] = "NIClfn\x7\x1a";

int Font::charidx(unsigned char c) {
	if (c >= FIRSTCHAR && c <= LASTCHAR) return c-FIRSTCHAR;
		else return -1;
}

Font::Font(const char *filename)
	 :displx(0), disply(0) {
	char nome[13];
	char id[9] = "........";
	int i,handle,size;

	fdfill(font,0l,sizeof font);
	strcpy(nome,filename);
	strcat(nome,".LFN");
	if ((handle = CRC_openread(nome)) >= 0) {
		CRC_read(handle,id,8);
		if (fwcomp(&id,(void *)&idLFN,8)) {
			CRC_read(handle,&height,sizeof height);
			CRC_read(handle,&spacelen,sizeof spacelen);
			for (i=0;i<NUMCHAR;i++) {
				TCharFig &fig = font[i];
				CRC_read(handle,&(fig.dx),1);
				if (fig.dx) {
					CRC_read(handle,&(fig.dy),1);
					fig.data = (char *)mm_reserve(size = fig.dx*fig.dy);
					CRC_read(handle,fig.data,size);
				}
				 





			}
		} else error("Font",err_invalidformat);
		CRC_close(handle);
	} else error("Font",err_filenotfound);
}

void Font::getcharfig(char c, int x, int y, int mdx, int mdy) {
	int idx;
	if ((idx = charidx(c)) >= 0) {
		freechar(idx);

		int ix,iy,maxx=1,maxy=1;
		for (ix=0;ix<mdx;ix++)
		  for (iy=0;iy<mdy;iy++)
			if (getpix(x+ix,y+iy)) {
				if (iy>=maxy) maxy=iy+1;
				maxx=ix+1;
			}

		TCharFig &fig = font[idx];
		fig.dx = maxx;
		fig.dy = maxy;
		fig.data = (char *)mm_reserve(maxx*maxy);
		getlfig(x,y,maxx,maxy,fig.data);
	}
}

void Font::freechar(char idx) {
	if (idx >= 0) {
		TCharFig &fig = font[idx];
		if (fig.data != NULL) {
			delete[] fig.data;  	 
			fig.data = NULL;
		}
	}
}

void Font::write(int x, int y, char col1, char col2, const char *str) {
	int idx;
	while (*str) {
		if ((idx = charidx(*str)) >= 0) {
			TCharFig &fig = font[idx];
			if (fig.dx) putlchar(x,y,fig.dx,fig.dy,col1,col2,fig.data);
			x+=fig.dx+displx;
		} else
			if (*str == ' ') x+=spacelen+displx;
		str++;
	}
}

void Font::writeclip(int x, int y, char col1, char col2, const char *str) {
	int idx;
	while (*str) {
		if ((idx = charidx(*str)) >= 0) {
			TCharFig &fig = font[idx];
			if (fig.dx) putlcchar(x,y,fig.dx,fig.dy,col1,col2,fig.data);
			x+=fig.dx+displx;
		} else
			if (*str == ' ') x+=spacelen+displx;
		str++;
	}
}

void Font::colorwrite(int x, int y, byte shade, const char *str) {
	int idx;
	while (*str) {
		if ((idx = charidx(*str)) >= 0) {
			TCharFig &fig = font[idx];
			if (fig.dx) putlschar(x,y,fig.dx,fig.dy,shade,fig.data);
			x+=fig.dx+displx;
		} else
			if (*str == ' ') x+=spacelen+displx;
		str++;
	}
}

int Font::writelen(const char *str) {
	int idx, len = 0;
	while (*str) {
		if ((idx = charidx(*str)) >= 0)	len+=font[idx].dx+displx;
		 else if (*str == ' ') len+=spacelen+displx;
		str++;
	}
	return len;
}

 





void Font::save(const char *filename) {
	char nome[13];
	int i,handle;

	strcpy(nome,filename);
	strcat(nome,".LFN");
	if ((handle = CRC_openwrite(nome)) >= 0) {
		CRC_write(handle,(void *)&idLFN,8);
		CRC_write(handle,&height,sizeof height);
		CRC_write(handle,&spacelen,sizeof spacelen);
		for (i=0;i<NUMCHAR;i++) {
			TCharFig &fig = font[i];
			CRC_write(handle,&(fig.dx),1);
			if (fig.dx) {
				CRC_write(handle,&(fig.dy),1);
				CRC_write(handle,fig.data,fig.dx*fig.dy);
			}
		}
		CRC_close(handle);
	} else error("Font::save","can't create");
}

void Font::packdata() {
	word totsize;
	int i, memidx;
	char *ptr;

	totsize = sizeof(height)+sizeof(spacelen)+
			  sizeof(byte)*NUMCHAR;	 
	for (i=0;i<NUMCHAR;i++) {
		TCharFig &fig = font[i];
		if (fig.dx)
			totsize += sizeof(fig.dy) + fig.dx*fig.dy;
	}

	ptr = (char *)mm_recall(memidx = mm_alloc(totsize));
	fmove(ptr,&height,sizeof(height));
	ptr+=sizeof(height);
	fmove(ptr,&spacelen,sizeof(spacelen));
	ptr+=sizeof(spacelen);
	for (i=0;i<NUMCHAR;i++) {
		TCharFig &fig = font[i];
		fmove(ptr,&fig.dx,sizeof(fig.dx));
		ptr+=sizeof(fig.dx);
		if (fig.dx) {
			fmove(ptr,&fig.dy,sizeof(fig.dy));
			ptr+=sizeof(fig.dy);
			fmove(ptr,fig.data,fig.dx*fig.dy);
			ptr+=fig.dx*fig.dy;
		}
	}
	mm_unload(memidx);
	 
}

void Font::unpackdata() {
	int i, memidx;
	char *ptr;
	word size;

	ptr = (char *)mm_recall(memidx = mm_alloc(0));	 
	mm_lock(memidx);
	fmove(&height,ptr,sizeof(height));
	ptr+=sizeof(height);
	fmove(&spacelen,ptr,sizeof(spacelen));
	ptr+=sizeof(spacelen);
	for (i=0;i<NUMCHAR;i++) {
		TCharFig &fig = font[i];
		fmove(&fig.dx,ptr,sizeof(fig.dx));
		ptr+=sizeof(fig.dx);
		if (fig.dx) {
			fmove(&fig.dy,ptr,sizeof(fig.dy));
			ptr+=sizeof(fig.dy);
			fig.data = (char *)mm_reserve(size = fig.dx*fig.dy);
			fmove(fig.data,ptr,size);
			ptr+=size;
		}
	}
	mm_unlock(memidx);
	mm_unload(memidx);
	 
}

Font::~Font() {
	for (char i=0; i<NUMCHAR; i++) freechar(i);
}