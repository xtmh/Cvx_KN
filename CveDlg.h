
// CveDlg.h : �w�b�_�[ �t�@�C��
//

#include	"param.h"
#include	"Circ.h"

#pragma once

typedef	unsigned char	uchar;

//	�ʕ␳���Ұ��׽
class CPeak
{
public:
	int		nOrg;	//	����Z���W
	double	dCrv;	//	����̨�èݸނ������W�l	dCrv <- nOrg
	double	dAvg;	//	�ߖT���ϒl				dAvg = avg(dCrv)		���ύ����̐�Βl
	double	dSub;	//	�����l����̍����l		dSub = dAvg - (MIN)		���ύ����̑��Βl
	double	dCal;	//	�ʕ␳�l				dCal = dCrv - dSub		���ύ����ɑ΂��鍂��
	double	dSmp;	//	����ϯ��				dSmp = dCal() - AVG
	double	dInc;	//	�X�Ζʕ␳�l			dSmp -= dInc			�X�Ζʕ␳
	uchar	uSmp;	//	�^���װ�\���p
	//	temp
	double	a, b, c;
	int		nFit[FIT_NUM];
};

class CFlt{
public:
	double	dMax;			//	�ő�l
	double	dMin;			//	�ŏ��l
	double	dAvg;			//	���ϒl
	double	dDist;			//	���U�l
	double	dRy;			//	Ry
	double	dRa;			//	Ra
	void reset()
	{
		dMax = dMin = dAvg = dDist = dRy = dRa = 0.0;
	}
};

//	�~������ް��׽
class COtl
{
public:
	CPoint	ptCg;			//	�d�S
	CPoint	ptCirc[PR];		//	�~�����W�l
	
	//	calc()�ŎZ�o
	double	dMax;			//	�ő�l
	double	dMin;			//	�ŏ��l
	double	dR;				//	���ϔ��a
	double	dDist;			//	���U�l
	double	dRy;			//	Ry:�ő卂��(PV�l)
	double	dRa;			//	Ra:�Z�p���ύ���
	
	void reset()
	{
		ptCg = CPoint(0, 0);
		memset(ptCirc, 0, sizeof(CPoint)*PR);
		dR = dDist = dRy = dRa = 0.0;
	}

	//	�e�����Ұ��Z�o(dDist, dRy, dRa)	
	void	calc()
	{
		int		x, y, n;
		double	dTemp[PR];
		double	dSum;

		//	���ϔ��a�Z�o
		dR = 0;
		for(n=0; n<PR; n++){
			dTemp[n] = sqrt((double)((ptCirc[n].x-ptCg.x)*(ptCirc[n].x-ptCg.x)
								+(ptCirc[n].y-ptCg.y)*(ptCirc[n].y-ptCg.y)));
			dR += dTemp[n];
		}
		dR /= PR;		//	���ϔ��a
	
		//	���ϕ��U�l, Ra, Ry�Z�o
		dMax = dMin = 0;
		dRa = dDist = 0;
		for(n=0; n<PR; n++){
			dSum = (dTemp[n]-dR);	//	�΍�
			dRa += fabs(dSum);
			dDist += (dSum*dSum);	//	�e�_�̕��U�l
			//	PV
			if(dSum>dMax)		dMax = dSum;
			else if(dSum<dMin)	dMin = dSum;
		}
		dRa /= PR;		//	Ra
		dRy = dMax-dMin;	//	Ry
		dDist /= PR;	//	���ϕ΍�
	}
};


// CCveDlg �_�C�A���O
class CCveDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CCveDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_CVE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g

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
	CPen*	pcpP1;	//	Pink����
	CPen*	pcpP2;	//	Pink�_��
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

	uchar		(*uBar)[PZ][PY][PC];		//	�װ�ް
	uchar		(*uDsp)[PY][PC];		//	�װ�摜���
	//uchar		(*uTmp)[PY][PC];
	uchar		(*uSfc)[PY][PC];		//	��ʎ��摜�p���
	uchar		(*uBin)[PY];			//	ܰ�ϯ��(���O�a�K��)

	//uchar		(*uOct)[PY][PZ];		//	̧�ق���̓ǂݏo���p���
	uchar		(*uFrm)[PZ][PY];		//	�S�ڰѕۑ��p���
	uchar		(*uFro)[PZ][PY];		//	�␳�o�͗p���

	//uchar		(*uSrc)[PY][PC];
	//uchar		(*uBak)[PX][PZ];
	//uchar		(*uMn0)[PX][PZ];
	//uchar		(*uMn1)[PX][PZ];
	//uchar		(*uPol)[PD][PZ];		//	�ɍ��W�摜�p���
	//uchar		(*uDd3)[PD][PD][PZ];	//	�S�ڰѕ��̋ɍ��W�摜�p���

	int			(*nPkPos);
	int			nSlcPos;
	int			nViewRange;

	CPeak		(*pkDepth)[PX];			//	Z��͏��
	CPeak		(*pkRef)[PX];			//	��ʂ�Z��͏��

	double		dPkAvg;
	double		dTrg[PY][2];

	CString		strFolderPath;			//	̧��̫��ނ����߽
	CString		strSfcImg;				//	�ʉ摜̧�ٖ�

	CRect		rcCirc;
	CCirc		m_Circ;

	int			nHist0[NUM];
	int			nHist1[NUM];
	double		dStd0;
	double		dStdL;			//	�����W���΍�
	double		dStdH;			//	�㑤�W���΍�
	double		dAvg0;

	HACCEL		hAccl;

	bool		bRef;			//	��ʉ摜۰�ސ�p
	bool		bImg;
	bool		bRcv;			//	�␳�����׸�
	bool		bOrg;			//	�����\��
	bool		bAnlyz;			//	�������͊����׸�

	CPoint		ptPos;			//	�د����W
	CPoint		ptMov;			//	���ݍ��W
	CPoint		ptD[3];			//	�X�Ε␳�̌��s��̂��߂̍��W3�_

	COtl		crOut, crIn;	//	�~������
	CFlt		ftSfc;			//	�ʑ���


	void		fileOpen();
	void		imgRcv(uchar dst[][PY][PC], uchar src[][PY][PC], int x);
	void		imgChg();
	void		imgSlc();
	void		imgCurv();
	void		imgCvFit();		//	����̨�èݸޏ���			(�@nOrg, �AdCrv)
	void		imgNrAvg();		//	�ߖT���ώZ�o����			(�BdAvg)
	void		imgLwSub();		//	�����l����̍����l�Z�o����	(�CMIN, �DdSub)
	void		imgMeas();		//	�v���]������				(�EAVG, �FdSmp)
	void		imgIncl();		//	�X�Ζʕ␳�l�Z�o
	void		imgSurface();	//	�ʉ摜����
	void		imgUpdate();	//	�`��X�V

	void		imgBin(uchar img1[][PX][PC], uchar img2[][PX][PC], uchar img3[][PX][PC]);
	void		imgCal(uchar img[][PX][PC]);
	void		imgLmt(uchar ref[][PX][PC], uchar dst[][PX][PC]);
	void		imgOpen(CString	s, bool sw=true);
	void		imgSfcOpen(CString s);
	void		imgCalc();
	void		imgMake();		//	���摜����
	//void		imgPeak(uchar dst[][PX][PC], uchar src[][PX][PC]);
	void		imgPol(uchar dst[][PD][PC], uchar src[][PX][PC]);
	double		imgFitting(double dDst[], int nSrc[], int nPk);	//	����̨�èݸޏ���
	void		imgTrim();		//	�~�������]��(���O�`)
	void		imgFlat();		//	�ʂ̕��R�x�]��

	void		csvOut();		//	���Ұ���CSV�o��

	void		makeTrig();
	void		makeBar();

	void		ptAvg( double *dDst, CPoint ptSrc);



// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	CSliderCtrl  m_sldLmt; // �X���C�_(Control)
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
};
