////////////////////////
//	param.h
////////////////////////

#define		VER			204

//#define		BMP_SAVE			//	������`����Ɩʉ摜��bmp�ŕۑ�����

#define		Z_RVS					//	Z�����](OnInitDialog(), imgOpen())
//#define		REF_AVG					//	�̧�ݽ�ް��̕���������

#define		IMG_OFS		30

#define		PI			3.1415926535
#define		RAMBDA		0.83		//	�����̒��S�g��

#define		SCALE		1			//	��10mm�(=1)
//#define	CURVE		(60*SCALE)	//	�p�ȕ␳�l(Y�����Ʊ)
#define		CURVE		(65*SCALE)	//	�p�ȕ␳�l(Y�����Ʊ)

#define		PZ			512
#define		PY			1024
#define		PX			1024
#define		PF			1024		//	���ڰѐ�(=PX)
#define		PR			1000		//	�֊s�ް���

#define		EXPND		2			//	�g�嗦

#define		PD			1024

#define		PC			3
#define		PS			PY*PZ*PC
#define		BMPSIZE		PY*PZ*PC

#define		POLSIZE		PD*PD*PC

#define		OCTSIZE		PX*PY*PZ

#define		BLUE		0
#define		GREEN		1
#define		RED			2

#define		NUM			256
#define		PAR			4			//	���z�㉺�������߂�W���΍��ւ̌W��
#define		LIMIT		70
#define		CORE_R		12			//	�ɍ��W�\���ł̺����a(�Œ�)
#define		PK_GAIN		3
#define		FIT_NUM		11//7		//	����̨�èݸޗp�ް���
#define		AVG_FRM		5			//	���ω��ڰѐ�
#define		UM_RATIO	1.5			//	��f��um���Z�W��

#define		UM_RANGE	20//10		//	���̧�ٕ\���ݼ�(�})

//	�X�Ε␳�p�̍��W3�_�̏����l
#define		KN
#ifdef KN
#define		X1			180	//	130
#define		X2			700
#define		X3			512
#define		Y1			512
#define		Y2			780
#define		Y3			180
#else
#define		X1			150	//	130
#define		X2			780
#define		X3			512
#define		Y1			512
#define		Y2			780
#define		Y3			130
#endif // KN


//	�֊s���o
#define		TH_COL					//	������`����Ɖ�͌��ʂɒ��F
#define		TH_OTL		50//100		//	�֊s�_���o臒l

//	�ʕ␳�p���o���W�_��No.
#define		P1			0
#define		P2			(P1+300)	//300
#define		P3			(P2+300)	//600
