#pragma once
 






 
#define	KB_NONE		0
#define	KB_UP		72
#define	KB_DOWN		80
#define	KB_LEFT		75
#define	KB_RIGHT	77
#define	KB_ALT		56
#define	KB_SHIFT	128	 
#define	KB_LSHIFT	42
#define	KB_RSHIFT	54	 
#define	KB_CTRL		29
#define	KB_SPACE	57
#define	KB_ESC		1
#define	KB_ENTER    28
#define	KB_t1		79
#define	KB_t3  		81
#define KB_t5		76
#define	KB_t7		71
#define KB_t0		82
#define KB_CANC		83
#define KB_tENTER   28
#define	KB_tPLUS	78
#define	KB_Z		44
#define KB_X		45

#define	KB_Q		16
#define	KB_W		17
#define	KB_E		18
#define	KB_A		30
#define	KB_S		31
#define	KB_D		32
#define	KB_F		33
#define	KB_TAB		15
#define	KB_BSLASH	41

#define KB_P		25
#define	KB_O_		39
#define KB_L		38
#define	KB_A_		40
#define	KB_M		50
#define	KB_H		35
#define	KB_J		36
#define	KB_K		37
#define	KB_U		22
#define	KB_COMMA	51

#define	KB_F1		59
#define	KB_F2		60
#define	KB_F3		61
#define	KB_F4		62
#define	KB_F5		63
#define	KB_F6		64
#define	KB_F7		65
#define	KB_F8		66
#define	KB_F9		67
#define	KB_F10		68
#define	KB_F11		87
#define	KB_F12		88

#define KB_1		2
#define KB_2		3
#define KB_3		4
#define KB_4		5
#define KB_5		6
#define KB_6		7
#define KB_7		8
#define KB_8		9
#define KB_9		10
#define KB_0		11
#define	KB_12		12
#define	KB_13		13
#define KB_SLASH	53
#define KB_PER		55
#define KB_MINUS	74

extern char Key[129], CodeAr[6], key_pause;
 
extern char kb_pressed(byte c);
extern char kb_onepressed(byte c);
extern void kb_presskey(byte c);
extern void kb_releasekey(byte c);
extern void kb_reset();
extern void kb_initkey();
extern void kb_enablestd();
extern void kb_disablestd();
extern void kb_donekey();
extern int kb_mouse_dx();
extern int kb_mouse_dy();
extern char kb_mouse_left();
extern char kb_mouse_right();
