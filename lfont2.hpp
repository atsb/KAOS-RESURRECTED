#pragma once
 






#define	FIRSTCHAR	(unsigned char)'!'
#define	LASTCHAR	(unsigned char)'\x97'
#define	NUMCHAR		(LASTCHAR-FIRSTCHAR+1)

typedef struct {
			byte dx, dy;
			char *data;
		} TCharFig;

typedef	TCharFig TAlphabet[NUMCHAR];

class Font
{
		TAlphabet font;
		unsigned char height, spacelen;
		char displx, disply;
		 



		int charidx(unsigned char c);
	public:
		Font(unsigned char h = 1, unsigned char slen = 1,
			 char dx = 0, char dy = 0)
			 : height(h), spacelen(slen), displx(dx), disply(dy)
				{ fdfill(font,0l,sizeof font); };
		Font(const char *filename);
		void setspacelen(unsigned char splen)
			{ spacelen = splen; };
		void setdispl(char dx, char dy)
			{ displx = dx; disply = dy; };
		void getcharfig(char c, int x, int y, int mdx, int mdy);
		void freechar(char idx);
		void write(int x, int y, char col1, char col2, const char *str);
		void writeclip(int x, int y, char col1, char col2, const char *str);
		void colorwrite(int x, int y, byte shade, const char *str);
		int writelen(const char *str);
		 




		void save(const char *filename);
		void packdata();
		void unpackdata();
		~Font();
};