#pragma once
 






#define	IBR_HEALTH		0x01
#define	IBR_ARMS		0x02
#define	IBR_KEYS		0x04
 
#define	IBR_FACEHIT		0x10
#define	IBR_SMILE		0x20
#define	IBR_PLKILLED	0x40

#define IBR_REPAINT 	0x1f
#define	IBR_REPNOTRANSF	0x9f

class InfoBar
{
	protected:
		int x, y, dx, dy;
		word memidx;
		int owner;
		byte animstate, timeslice;
		int oldface;
	public:
		InfoBar(int);
		virtual void drawface(int,char);
		virtual void repaint(byte);
		virtual void animate();
		void handle_event(TEvent &);
};

class LittleInfoBar : public InfoBar
{
	public:
		LittleInfoBar(int,int,int);
		virtual void repaint(byte);
		virtual void animate();
};

extern TFig BigBar, LitBar;