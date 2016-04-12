////////////////////////
//	param.h
////////////////////////

#define		VER			200

//#define		BMP_SAVE			//	これを定義すると面画像をbmpで保存する

#define		Z_RVS					//	Z軸反転(OnInitDialog(), imgOpen())

#define		IMG_OFS		30

#define		PI			3.1415926535
#define		RAMBDA		0.83		//	光源の中心波長

#define		SCALE		1			//	□10mm基準(=1)
//#define	CURVE		(60*SCALE)	//	湾曲補正値(Y幅にﾘﾆｱ)
#define		CURVE		(65*SCALE)	//	湾曲補正値(Y幅にﾘﾆｱ)

#define		PZ			512
#define		PY			1024
#define		PX			1024
#define		PF			1024		//	総ﾌﾚｰﾑ数(=PX)
#define		PR			1000		//	輪郭ﾃﾞｰﾀ数

#define		EXPND		2			//	拡大率

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
#define		PAR			4			//	分布上下限を決める標準偏差への係数
#define		LIMIT		70
#define		CORE_R		12			//	極座標表示でのｺｱ直径(固定)
#define		PK_GAIN		3
#define		FIT_NUM		11//7		//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞ用ﾃﾞｰﾀ数
#define		AVG_FRM		5			//	平均化ﾌﾚｰﾑ数
#define		UM_RATIO	1.5			//	画素⇒um換算係数

#define		UM_RANGE	20//10		//	ﾌﾟﾛﾌｧｲﾙ表示ﾚﾝｼﾞ(±)

//	傾斜補正用の座標3点の初期値
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


//	輪郭抽出
#define		TH_COL					//	これを定義すると解析結果に着色
#define		TH_OTL		50//100		//	輪郭点検出閾値

//	面補正用抽出座標点のNo.
#define		P1			0
#define		P2			(P1+300)	//300
#define		P3			(P2+300)	//600
