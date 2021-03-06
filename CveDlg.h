
// CveDlg.h : ヘッダー ファイル
//

#include	"param.h"
#include	"Circ.h"

#pragma once

typedef	unsigned char	uchar;

//	面補正ﾊﾟﾗﾒｰﾀｸﾗｽ
class CPeak
{
public:
	int		nOrg;	//	生のZ座標
	double	dCrv;	//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞした座標値	dCrv <- nOrg
	double	dAvg;	//	近傍平均値				dAvg = avg(dCrv)		平均高さの絶対値
	double	dSub;	//	下限値からの差分値		dSub = dAvg - (MIN)		平均高さの相対値
	double	dCal;	//	面補正値				dCal = dCrv - dSub		平均高さに対する高さ
	double	dSmp;	//	差分ﾏｯﾌﾟ				dSmp = dCal() - AVG
	double	dInc;	//	傾斜面補正値			dSmp -= dInc			傾斜面補正
	uchar	uSmp;	//	疑似ｶﾗｰ表示用
	//	temp
	double	a, b, c;
	int		nFit[FIT_NUM];
};

class CFlt{
public:
	double	dMax;			//	最大値
	double	dMin;			//	最小値
	double	dAvg;			//	平均値
	double	dDist;			//	分散値
	double	dRy;			//	Ry
	double	dRa;			//	Ra
	void reset()
	{
		dMax = dMin = dAvg = dDist = dRy = dRa = 0.0;
	}
};

//	円周解析ﾃﾞｰﾀｸﾗｽ
class COtl
{
public:
	CPoint	ptCg;			//	重心
	CPoint	ptCirc[PR];		//	円周座標値
	
	//	calc()で算出
	double	dMax;			//	最大値
	double	dMin;			//	最小値
	double	dR;				//	平均半径
	double	dDist;			//	分散値
	double	dRy;			//	Ry:最大高さ(PV値)
	double	dRa;			//	Ra:算術平均高さ
	
	void reset()
	{
		ptCg = CPoint(0, 0);
		memset(ptCirc, 0, sizeof(CPoint)*PR);
		dR = dDist = dRy = dRa = 0.0;
	}

	//	各種ﾊﾟﾗﾒｰﾀ算出(dDist, dRy, dRa)	
	void	calc()
	{
		int		x, y, n;
		double	dTemp[PR];
		double	dSum;

		//	平均半径算出
		dR = 0;
		for(n=0; n<PR; n++){
			dTemp[n] = sqrt((double)((ptCirc[n].x-ptCg.x)*(ptCirc[n].x-ptCg.x)
								+(ptCirc[n].y-ptCg.y)*(ptCirc[n].y-ptCg.y)));
			dR += dTemp[n];
		}
		dR /= PR;		//	平均半径
	
		//	平均分散値, Ra, Ry算出
		dMax = dMin = 0;
		dRa = dDist = 0;
		for(n=0; n<PR; n++){
			dSum = (dTemp[n]-dR);	//	偏差
			dRa += fabs(dSum);
			dDist += (dSum*dSum);	//	各点の分散値
			//	PV
			if(dSum>dMax)		dMax = dSum;
			else if(dSum<dMin)	dMin = dSum;
		}
		dRa /= PR;		//	Ra
		dRy = dMax-dMin;	//	Ry
		dDist /= PR;	//	平均偏差
	}
};


// CCveDlg ダイアログ
class CCveDlg : public CDialogEx
{
// コンストラクション
public:
	CCveDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_CVE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

public:

	static const	COLORREF		ColorTable[256];

	uchar	uClr[256][PC];

	CBrush*	pcbRed;
	CBrush*	pcbPpl;
	CBrush*	pcbGrn;
	CPen*	pcpX;
	CPen*	pcpG1;
	CPen*	pcpG2;
	CPen*	pcpB;
	CPen*	pcpW;
	CPen*	pcpR;
	CPen*	pcpG;
	CPen*	pcpP1;	//	Pink実線
	CPen*	pcpP2;	//	Pink点線
	CPen*	pcpP;	
	CPen*	pcpDG;
	CPen*	pcpD;
	CPen*	pOldPen;
	CFont	Font1, Font2;
	CFont*	pOldFont;

	BITMAPINFO	bi;
	BITMAPINFO	bp;
	uchar*		pb;
	CImage		img;
	CBitmap*	pBmp;
	BITMAP		bmp;

	//uchar		(*uLz4);

	uchar		(*uBar)[PZ][PY][PC];		//	ｶﾗｰﾊﾞｰ
	uchar		(*uDsp)[PY][PC];		//	ｶﾗｰ画像ﾒﾓﾘ
	//uchar		(*uTmp)[PY][PC];
	uchar		(*uSfc)[PY][PC];		//	上面視画像用ﾒﾓﾘ
	uchar		(*uBin)[PY];			//	ﾜｰｸﾏｯﾌﾟ(内外径規定)

	//uchar		(*uOct)[PY][PZ];		//	ﾌｧｲﾙからの読み出し用ﾒﾓﾘ
	uchar		(*uFrm)[PZ][PY];		//	全ﾌﾚｰﾑ保存用ﾒﾓﾘ
	uchar		(*uFro)[PZ][PY];		//	補正出力用ﾒﾓﾘ

	//uchar		(*uSrc)[PY][PC];
	//uchar		(*uBak)[PX][PZ];
	//uchar		(*uMn0)[PX][PZ];
	//uchar		(*uMn1)[PX][PZ];
	//uchar		(*uPol)[PD][PZ];		//	極座標画像用ﾒﾓﾘ
	//uchar		(*uDd3)[PD][PD][PZ];	//	全ﾌﾚｰﾑ分の極座標画像用ﾒﾓﾘ

	int			(*nPkPos);
	int			nSlcPos;
	int			nViewRange;

	CPeak		(*pkDepth)[PX];			//	Z解析情報
	CPeak		(*pkRef)[PX];			//	基準面のZ解析情報
	CPeak		(*pkTmp)[PX];			//	保存用ﾃﾝﾎﾟﾗﾘ

	double		dPkAvg;
	double		dTrg[PY][2];

	CString		strFolderPath;			//	ﾌｧｲﾙﾌｫﾙﾀﾞのﾌﾙﾊﾟｽ
	CString		strSfcImg;				//	面画像ﾌｧｲﾙ名

	CRect		rcCirc;
	CCirc		m_Circ;

	int			nHist0[NUM];
	int			nHist1[NUM];
	double		dStd0;
	double		dStdL;			//	下側標準偏差
	double		dStdH;			//	上側標準偏差
	double		dAvg0;

	HACCEL		hAccl;

	bool		bRef;			//	基準面画像ﾛｰﾄﾞ専用
	bool		bImg;
	bool		bRcv;			//	補正完了ﾌﾗｸﾞ
	bool		bOrg;			//	原寸表示
	bool		bAnlyz;			//	特徴分析完了ﾌﾗｸﾞ

	CPoint		ptPos;			//	ｸﾘｯｸ座標
	CPoint		ptMov;			//	現在座標
	CPoint		ptD[3];			//	傾斜補正の元行列のための座標3点

	COtl		crOut, crIn;	//	円周属性
	CFlt		ftSfc;			//	面属性


	void		fileOpen();
	void		imgRcv(uchar dst[][PY][PC], uchar src[][PY][PC], int x);
	void		imgChg();
	void		imgSlc();
	void		imgCurv();
	void		imgCvFit();		//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞ処理			(�@nOrg, �AdCrv)
	void		imgNrAvg();		//	近傍平均算出処理			(�BdAvg)
	void		imgLwSub();		//	下限値からの差分値算出処理	(�CMIN, �DdSub)
	void		imgMeas();		//	計測評価処理				(�EAVG, �FdSmp)
	void		imgMeas2();		//	計測評価処理4分割			(�EAVG, �FdSmp)
	void		imgIncl();		//	傾斜面補正値算出(3点による面補正)
	void		imgIncl2();		//	傾斜面補正値算出(4点による面補正)
	void		imgSurface();	//	面画像生成
	void		imgUpdate();	//	描画更新

	void		imgBin(uchar img1[][PX][PC], uchar img2[][PX][PC], uchar img3[][PX][PC]);
	void		imgCal(uchar img[][PX][PC]);
	void		imgLmt(uchar ref[][PX][PC], uchar dst[][PX][PC]);
	void		imgOpen(CString	s, bool sw=true);
	void		imgSfcOpen(CString s);
	void		imgCalc();
	void		imgMake();		//	仮画像生成
	//void		imgPeak(uchar dst[][PX][PC], uchar src[][PX][PC]);
	void		imgPol(uchar dst[][PD][PC], uchar src[][PX][PC]);
	double		imgFitting(double dDst[], int nSrc[], int nPk);	//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞ処理
	void		imgTrim();		//	円周属性評価(内外形)
	void		imgFlat();		//	面の平坦度評価

	void		csvOut();		//	ﾊﾟﾗﾒｰﾀのCSV出力

	void		makeTrig();
	void		makeBar();

	void		ptAvg( double *dDst, CPoint ptSrc);



// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpen();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCalc();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	int m_nSld;
	int m_nEdit;
	CSliderCtrl  m_sldLmt; // スライダ(Control)
	CEdit		 m_editLmt;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnUpdateEdit1();
	CString m_strPathName;
	afx_msg void OnBnClickedOk();
	BOOL m_bCal;
	afx_msg void OnBnClickedCal();
	//afx_msg void OnBnClickedLump();
	afx_msg void OnBnClickedMake();
	BOOL m_bPeak;
	afx_msg void OnBnClickedPeak();
	CString m_strSlide;
	int m_nSldCore;
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_strCore;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CString m_strXY;
	afx_msg void OnBnClickedLump();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedRef();
	CString m_strSts;
	BOOL m_bCurv;
	afx_msg void OnBnClickedCurv();
	afx_msg void OnBnClickedSave();
	CString m_strDet;
	afx_msg void OnCbnSelchangeRange();
	CComboBox m_xcRange;
	afx_msg void OnBnClickedLz4();
	afx_msg void OnBnClickedOct();
	BOOL m_bIncl;
	afx_msg void OnBnClickedIncl();
	afx_msg void OnBnClickedAnlyz();
	afx_msg void OnBnClickedImgOpen();
	afx_msg void OnBnClickedInv();
	BOOL m_bInv;
};
