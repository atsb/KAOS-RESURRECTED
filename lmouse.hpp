#pragma once
 






#define	MB_LEFT		0x01
#define	MB_RIGHT	0x02
#define	MB_MIDDLE	0x04

#define	MA_MOVES 	0x01
#define	MA_PLEFT 	0x02
#define	MA_RLEFT 	0x04
#define	MA_PRIGHT 	0x08
#define	MA_RRIGHT 	0x10
#define	MA_PMIDDLE 	0x20
#define	MA_RMIDDLE 	0x40
#define	MA_ALWAYS 	0x7f

typedef struct {
			int hotx, hoty;
			int dx, dy;
			void *data;
		} MouseCursor;

extern void mousehandler();

class Mouse
{
	private:
		friend void mousehandler();
		MouseCursor cursor, under;
		char installed;
		int curx, cury;
		byte butnum, buttons, invisible;
		void getmouserect(int &x1, int &y1, int &x2, int &y2);
		void refresh(int x, int y, byte butt);
		void draw();
		void undraw();
	public:
		Mouse();
		char install();
		char reset();
		void show();
		void hide();
		void get(int &x, int &y, byte &butt);
		void getpos(int &x, int &y);
		byte getbuttons();
		void go(int x, int y);
		void setxrange(int x1, int x2);
		void setyrange(int y1, int y2);
		void setrange(int x1, int y1, int x2, int y2);
		void setcursor(MouseCursor *newcurs);
		void setcursormem(MouseCursor *newcurs, void *undr);
		void update(int x1, int y1, int dx, int dy);
		char visible() { return !invisible; }
		void uninstall();
		~Mouse();
};

extern Mouse mouse;
