 






 

#include "std.hpp"
#include "errorstr.hpp"
#include "fastmem.hpp"
#include "mm4.hpp"
#include "vgatool3.hpp"
#include "lgraph.hpp"
#include "lfont2.hpp"
#include "platform_compat.hpp"

int main() {
	byte c;
	PPal pal = new char[768];
	Font *myfont;

	mm_init();
	setvbuff(VMEMPTR);
	initL();

	if (!loadLBM("lfonts.lbm",pal)) error("main",err_filenotfound);
	setpal((PPal)pal,0,256);
	myfont = new Font(6,3,1,0);
	for (c=FIRSTCHAR; c<=LASTCHAR; c++)
		myfont->getcharfig(c,((c-FIRSTCHAR) & 31)*9,((c-FIRSTCHAR) >> 5)*7,8,6);
	ffillblock(16,140,280,32,0x07070707);
	myfont->write(24,144,1,2,"ab cdefghijklmn AB CDEFGHIJKLM");
	if (getch()==13) myfont->save("little");
	delete myfont;

	myfont = new Font(8,5,0,0);
	for (c=FIRSTCHAR; c<=LASTCHAR; c++)
		myfont->getcharfig(c,((c-FIRSTCHAR) & 31)*9,40+((c-FIRSTCHAR) >> 5)*9,8,8);
	ffillblock(16,140,280,32,0x07070707l);
	myfont->write(24,144,1,2,"ab cdefghijklmn AB CDEFGHIJKLM");
	if (getch()==13) myfont->save("std");
	delete myfont;

	 























	 





























	doneL();
	mm_done();
	return 0;
}