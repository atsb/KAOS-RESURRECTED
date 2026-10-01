 






#include "std.hpp"
#include "errorstr.hpp"
#include "xms.hpp"
#include "fastmem.hpp"
#include "mm4.hpp"
#include "crcio.hpp"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

std::int32_t far *ddf = NULL;
std::int16_t ddfnum = 0;

#define LEVSIZE	16410
void far *ptr = new char[LEVSIZE];

void showhelp() {
	printf("formato: SETFIRM <option> <dlf_file>\n");
	printf(" <option>   = \"a\" add firms\n");
	printf("              \"r\" remove firms\n");
	printf("              \"v\" view all firms\n");
	printf(" <dlf_file> = name of .DLF file without extension\n");
}

int main(int argc, char *argv[]) {
	int action, handle, wrhandle, i,j;
	char dlfname[260];
	char *mark = new char[8];
	byte num;

	mm_init();

	printf("\n\nSETFIRM - Installatore di firme per file .DLF\n");
	if (argc == 3) {
		switch(toupper(argv[1][0])) {
		case 'V': action=0; break;
		case 'A': action=1; break;
		case 'R': action=2; break;
		default:
			printf("invalid option: %s\n",argv[1]);
			showhelp();
			return 1;
		}
		if (strlen(argv[2]) > sizeof(dlfname) - 5) {
			printf("filename too long\n");
			return 1;
		}
		strcpy(dlfname,argv[2]);
		strcat(dlfname,".DLF");
		if ((handle = CRC_openread(dlfname)) >= 0) {
			CRC_read(handle,mark,8);
			CRC_read(handle,&num,1);
			if (action>0) {
				if ((wrhandle = CRC_openwrite("$$$.dlf")) >= 0) {
					CRC_write(wrhandle,mark,8);
					CRC_write(wrhandle,&num,1);
				} else {
					CRC_close(handle);
					error("main","can't create new file");
				  }
			}
			for (i=0;i<num;i++) {
				CRC_read(handle,ptr,LEVSIZE);
				if (action>0)
					CRC_write(wrhandle,ptr,LEVSIZE);
			}
			CRC_read(handle,&ddfnum,sizeof(ddfnum));
			if (action==1) {
				printf("(Š possibile aggiungere solo 100 firme a sessione)\n");
				i=100;
			} else i=0;
			if ((ddf = (std::int32_t far *)mm_reserve((ddfnum+i)<<2)) == NULL)
				error("main",err_notenoughmemory);
			CRC_read(handle,ddf,ddfnum<<2);
			std::int32_t *app = ddf;
			do {
				printf("%d firme nel file:\n",ddfnum);
				for (i=0;i<ddfnum;i++)
					printf("%08lX  ",static_cast<unsigned long>(*(app++)));
				printf("\n");
				if (action) {
				  if (action==1) printf("Inserisci la firma da aggiungere (\"-1\" per finire): ");
							else printf("Inserisci la firma da rimuovere (\"-1\" per finire): ");
				  unsigned long parsed;
				  scanf("%08lX",&parsed);
				  if (parsed <= 0x7ffffffful) {
					std::int32_t firm = static_cast<std::int32_t>(parsed);
					for (i=0,app=ddf;i<ddfnum;i++,app++)
						if (*app==firm) {
							if (action==2) {
								for (;i<ddfnum-1;i++,app++)
									*app = *(app+1);
								ddfnum--;
							}
							i = -1;
							break;
						}
					if (action==1 && i!=-1) {
						ddf[ddfnum++] = firm;
					}
				  } else break;
				}
			} while (action  );
			CRC_close(handle);
			if (action) {
				CRC_write(wrhandle,&ddfnum,sizeof(ddfnum));
				CRC_write(wrhandle,ddf,ddfnum<<2);
				CRC_close(wrhandle);
				printf("Rewriting %s\n",dlfname);
				if (!unlink(dlfname)) {
					if (rename("$$$.dlf",dlfname))
						printf("Can't rename\n");
				} else
					printf("Can't delete\n");
				if (ddf) { delete[] reinterpret_cast<char *>(ddf); ddf = NULL; }
			}
		} else error("main",err_filenotfound);
		delete[] static_cast<char *>(ptr);
	} else {
		showhelp();
		return 1;
	  }
	mm_done();
	return 0;
}
