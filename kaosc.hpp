#pragma once
 






#define	T_WALL1	 	 0
#define	T_WALL1M	 1
#define	T_WALL1S     2
#define	T_WALL1MS	 3
#define	T_WALL1SS	 4
#define	T_WALL1MSS	 5
#define T_TAP		 6
#define T_TAPM		 7
#define T_TAPS		 8
#define T_TAPMS		 9
#define T_TAPSS		10
#define T_TAPMSS	11
#define T_SCUDO		12
#define T_SCUDOM	13
#define T_SCUDOS	14
#define T_SCUDOMS	15
#define T_SCUDOSS	16
#define T_SCUDOMSS	17
#define T_ARC		18
#define T_ARCM		19
#define T_PIETR1	20
#define T_PIETR1M 	21
#define T_PIETR1S	22
#define T_PIETR1MS 	23
#define T_PIETR1SS	24
#define T_PIETR1MSS	25
#define T_ASCE		26
#define T_ASCEM		27
#define T_ASCES		28
#define T_ASCEMS	29
#define T_ASCESS	30
#define T_ASCEMSS	31
#define T_ROCK		32
#define T_ROCKS		33
#define T_ROCKSS	34
#define T_WOOD1		35
#define T_WOOD1S	36
#define T_WOOD1SS	37
#define T_WBOX		38
#define T_WBOXS		39
#define T_WBOXSS	40
#define T_DOOR1		41
#define T_DOOR1S	42
#define T_DOOR2		43
#define T_DOOR2S	44
#define T_DOORY		45
#define T_DOORYS	46
#define T_DOORR		47
#define T_DOORRS	48
#define T_WOOD2		49
#define T_WOOD2S	50
#define T_WOOD2SS	51
#define T_SBARRE	52
#define T_SBARRES	53
#define T_SWITCHU	54

#define T_WALL1Z	55
#define T_WALL1MZ	56
#define T_PIETR1Z	57
#define T_PIETR1MZ	58
#define T_ROCKZ		59
#define T_WOOD1Z	60
#define T_WBOXZ  	61
#define	T_WOOD2Z	62
#define	T_WALL2 	63
#define T_WALL2M	64
#define T_WALL2Z	65
#define T_WALL2MZ	66
#define T_PIETR2	67
#define T_PIETR2M	68
#define T_PIETR2Z	69
#define T_PIETR2MZ	70
#define T_EARTH		71
#define T_EARTHZ	72
#define T_SOFT		73
#define T_SOFTM		74
#define T_SOFTZ		75
#define T_SOFTMZ	76

#define T_DRAGON	77	 

 
#define T_GRATAW	86
#define T_GRATAWZ	87
#define T_GRATAB	88
#define T_GRATABZ	89
#define T_FLROCK	90
#define T_FLROCKZ	91
 
#define T_OVERDOOR	92
#define T_DOORSLOT	93
#define T_SWITCHD	94

#define T_TRANSP	T_GRATAW
#define TEXTNUM		(T_SWITCHD+1)

#define	T_TEXTURES	(TEXTNUM)
#define	T_OBJECTS	(T_TEXTURES+36+18)	 
#define	T_ENEMIES	(T_OBJECTS+12)
#define	T_PLAYERS	(T_ENEMIES+12)

#define F_PLAYER		0				 
#define	F_SOLDIER		(F_PLAYER+39)	 
#define	F_SLIMER		(F_SOLDIER+35)	 
#define	F_BUB	 	 	(F_SLIMER+22)  	 
#define	F_FACE			(F_BUB+20)		 
#define	F_SPIDER	    (F_FACE+17)  	 
#define	F_MOUSE			(F_SPIDER+19)	 
#define	F_MISSILE		(F_MOUSE+10)	 
#define	F_FIREBALL		(F_MISSILE+5)
#define	F_SLIMFIRE		(F_FIREBALL+1)   
#define	F_SPIDFIRE		(F_SLIMFIRE+2)   
#define	F_TORCIA  		(F_SPIDFIRE+4)	 
#define	F_TORCIAM  		(F_TORCIA+4)	 
#define	F_COLUMN		(F_TORCIAM+4)
#define	F_COLUMNM		(F_COLUMN+1)
#define	F_BARILE		(F_COLUMNM+1)	 
#define	F_BARILX		(F_BARILE+5)
#define	F_FIREBOOM		(F_BARILX+1)     
#define	F_KEYRED      	(F_FIREBOOM+4)
#define	F_KEYELLOW   	(F_KEYRED+1)
#define	F_OSSALIT     	(F_KEYELLOW+1)
#define	F_OSSABIG     	(F_OSSALIT+1)
#define	F_VASO        	(F_OSSABIG+1)    
#define	F_PIANTA  		(F_VASO+6) 		 
#define	F_ALBERO		(F_PIANTA+5)	 
#define	F_LAMP			(F_ALBERO+2)	 
#define	F_LAMPX			(F_LAMP+4)
#define	F_LAMPOFF		(F_LAMPX+1)
#define	F_TRESPOLO     	(F_LAMPOFF+1)	 
#define	F_TRESPOLOX    	(F_TRESPOLO+2)	 
#define	F_TRESPOLOFF	(F_TRESPOLOX+1)
#define	F_BRAZIER		(F_TRESPOLOFF+1) 	 
#define	F_FUCILE       	(F_BRAZIER+4)
#define	F_MITRA       	(F_FUCILE+1)
#define	F_BAZOOKA       (F_MITRA+1)
#define	F_FARMALIT		(F_BAZOOKA+1)
#define	F_FARMABIG    	(F_FARMALIT+1)
#define	F_BONUS       	(F_FARMABIG+1)		 
#define	F_SOLDOLIT    	(F_BONUS+2)
#define	F_SOLDOBIG    	(F_SOLDOLIT+1)
#define F_CARTUCCIA1	(F_SOLDOBIG+1)
#define F_CARTUCCIA2	(F_CARTUCCIA1+1)
#define F_CARTUCCIA3	(F_CARTUCCIA2+1)
#define F_CARTUCCIA4	(F_CARTUCCIA3+1)
#define	F_BOOM			(F_CARTUCCIA4+1) 	 
#define	F_BLOOD         (F_BOOM+12)			 
#define F_SMOG			(F_BLOOD+5)		  	 
#define F_TESCHIO		(F_SMOG+3)		  	 
#define F_BOMB			(F_TESCHIO+2)		 
#define F_CIRCLIGHT		(F_BOMB+15)
#define F_SPIKELIGHT	(F_CIRCLIGHT+1)
#define F_TORCLIGHT     (F_SPIKELIGHT+1)
#define F_LAMPLIGHT		(F_TORCLIGHT+1)
#define	F_ANTIKAOS		(F_LAMPLIGHT+1)		 
#define	FIGNUM			(F_ANTIKAOS+9)		 
 
 
#define MSH_SQUARE		0
#define	MSH_VERT		1
#define	MSH_HORIZ		2
#define	MSH_DIAG1		3
#define	MSH_DIAG2		4
#define	MSH_DOOR		5
#define	MAPSHAPE(X)		(X<<8)
 
#define MTP_WALL		0
#define	MTP_DOOR1		1	 
#define	MTP_DOOR2		2    
#define MTP_GRID1 		3	 
#define	MTP_SLOT		4
#define MAPTYPE(X)		(X<<11)
 
#define	MFL_NONE	0x4000
#define	MFL_STOP 	0x8000
typedef struct {		   
			byte data  		: 8;
			byte shape 		: 3;
			byte type  		: 3;
			byte none		: 1;	 
			byte stop		: 1; 	 
		} TMapInfo;
 
#define	OMF_UNWALKABLE	0x1000
#define	OMF_VISIBLE		0x0100
#define	OMF_HITABLE		0x0010
#define	OMF_SOUNDABLE	0x0001
#define OMF_STANDARD	0x1111
 
#define	OMM_UNWALKABLE	0xF000
#define	OMM_VISIBLE		0x0F00
#define	OMM_HITABLE		0x00F0
#define	OMM_SOUNDABLE	0x000F
typedef struct {		   
			byte soundable	: 4;
			byte hitable	: 4;
			byte visible	: 4;
			byte unwalkable	: 4;
		} TObjInfo;
