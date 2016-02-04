
// CveDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Cve.h"
#include "CveDlg.h"
#include "param.h"
#include "lz4.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// **********************************************************************
// *  機能：Windows 起動後に経過時間をm秒単位で返す                      *
// *                                                                    * 
// * Input : 無し                                                       *
// *                                                                    *   
// * OutPut: =  経過時間                                                *
// **********************************************************************
double GetPassTimeWindow() 
{
	LARGE_INTEGER pfCounter , pfFrequency ;
	
	QueryPerformanceCounter(&pfCounter) ;
	QueryPerformanceFrequency(&pfFrequency) ;
	return (double)pfCounter.QuadPart * 1000.0 / (double)pfFrequency.QuadPart ; 
}

//	Message pump
void MessagePump()
{
	MSG	msg;
	if(PeekMessage(&msg, (HWND)NULL, 0, 0, PM_NOREMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}



// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCveDlg ダイアログ
CCveDlg::CCveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCveDlg::IDD, pParent)
	, m_nSld(512)
	, m_nEdit(0)
	, m_strPathName(_T(""))
	, m_bCal(FALSE)				//	疑似ｶﾗｰ
	, m_bPeak(TRUE)
	, m_strSlide(_T(""))
	, m_nSldCore(CORE_R)
	, m_strCore(_T(""))
	, m_strXY(_T(""))
	, m_strSts(_T(""))
	, m_bCurv(TRUE)
	, m_strDet(_T(""))
	, m_bIncl(FALSE)			//	傾斜面補正
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);

	pcbRed = new CBrush(COLORREF(RGB(255,0,0)));
	pcbPpl = new CBrush(COLORREF(RGB(255,0,255)));
	pcbGrn = new CBrush(COLORREF(RGB(0,255,0)));
	pcpX = new CPen(PS_SOLID, 1, COLORREF(RGB(0,0,0)));
	pcpG1 = new CPen(PS_SOLID, 1, COLORREF(RGB(150,150,150)));
	pcpG2 = new CPen(PS_SOLID, 1, COLORREF(RGB(200,200,200)));
	pcpB = new CPen(PS_SOLID, 1, COLORREF(RGB(0,0,125)));
	pcpW = new CPen(PS_SOLID, 1, COLORREF(RGB(128,128,255)));
	pcpR = new CPen(PS_DOT, 1, COLORREF(RGB(255,0,0)));
	pcpP1 = new CPen(PS_SOLID, 1, COLORREF(RGB(200,128,128)));
	pcpP2 = new CPen(PS_DOT, 1, COLORREF(RGB(200,128,128)));
	pcpG = new CPen(PS_SOLID, 1, COLORREF(RGB(0,200,0)));
	pcpDG = new CPen(PS_SOLID, 1, COLORREF(RGB(0,100,0)));
	pcpD = new CPen(PS_DOT, 1, COLORREF(RGB(100,255,100)));
	pcpP = new CPen(PS_SOLID, 1, COLORREF(RGB(255,0,255)));
	Font1.CreateFont(	20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, //** フォント変更
						SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, "ＭＳ Ｐゴシック");
	Font2.CreateFont(	12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, //** フォント変更
						SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, "ＭＳ Ｐゴシック");
	pOldPen;

}

//	ｶﾗｰﾏｯﾌﾟ
const COLORREF CCveDlg::ColorTable[256]  = {
	0x000000,0x0A0200,0x140400,0x1E0600,0x280800,0x320A00,0x3C0C00,0x460E00,	// 00-07
	0x501000,0x5A1200,0x641400,0x6E1600,0x781800,0x821A00,0x8C2200,0x962A00,	// 08-0F
	0x9D3200,0xA13A00,0xA54200,0xA94A00,0xAD5200,0xB15A00,0xB56200,0xB96A00,	// 10-17
	0xBD7200,0xC17A00,0xC58200,0xC88A00,0xC99200,0xCA9A00,0xCBA200,0xCCAA00,	// 18-1F
	0xCDB200,0xCEBA00,0xCFC200,0xD0CA00,0xD1D200,0xD2D300,0xCED400,0xCAD500,	// 20-27
	0xC2D600,0xBAD700,0xB2D800,0xAAD900,0xA2DA00,0x9ADB00,0x92DC00,0x8ADD00,	// 28-2F
	0x82DE00,0x7ADF00,0x72E000,0x6AE100,0x62E200,0x5AE400,0x52E500,0x4AE700,	// 30-37	
	0x42E800,0x3AE900,0x32EA00,0x2AEC00,0x22ED00,0x1AEF00,0x12F000,0x0AF200,	// 38-3F
	0x02F300,0x00F500,0x00F601,0x00F807,0x00F90D,0x00FB13,0x00FC19,0x00FF19,	// 40-47
	0x00FF25,0x00FF2B,0x00FF31,0x00FF37,0x00FF3D,0x00FF43,0x00FF49,0x00FF4F,	// 48-4F
	0x00FF55,0x00FF5B,0x00FF61,0x00FF67,0x00FF6D,0x00FF73,0x00FF79,0x00FF7F,	// 50-57
	0x00FF85,0x00FF8B,0x00FF91,0x00FF97,0x00FF9D,0x00FFA3,0x00FFA9,0x00FFAF,	// 58-5F
	0x00FFB5,0x00FFBB,0x00FFC1,0x00FFC7,0x00FFCD,0x00FFD0,0x00FFD3,0x00FFD6,	// 60-67
	0x00FFD9,0x00FFDC,0x00FFDF,0x00FFE2,0x00FFE5,0x00FFE8,0x00FFEB,0x00FFEE,	// 68-6F
	0x00FFF1,0x00FFF4,0x00FFF7,0x00FFFA,0x00FFFD,0x00FCFF,0x00F9FF,0x00F6FF,	// 70-77
	0x00F3FF,0x00F0FF,0x00EDFF,0x00EAFF,0x00E7FF,0x00E4FF,0x00E1FF,0x00DEFF,	// 78-7F
	0x00DBFF,0x00D8FF,0x00D5FF,0x00D2FF,0x00CFFF,0x00CCFF,0x00C9FF,0x00C6FF,	// 80-87
	0x00C3FF,0x00C0FF,0x00BDFF,0x00B6FF,0x00AFFF,0x00A8FF,0x00A1FF,0x009AFF,	// 88-8F
	0x0093FF,0x008CFF,0x0085FF,0x007EFF,0x0077FF,0x0074FF,0x0071FF,0x006EFF,	// 90-97
	0x006BFF,0x0068FF,0x0065FF,0x0062FF,0x005FFF,0x005CFF,0x0059FF,0x0056FF,	// 98-9F
	0x0053FF,0x0050FF,0x004DFF,0x004AFF,0x0047FF,0x0045FF,0x0043FF,0x0041FF,	// A0-A7
	0x003FFF,0x003DFF,0x003BFF,0x0039FF,0x0037FF,0x0035FF,0x0033FF,0x0031FF,	// A8-AF
	0x002FFF,0x002DFF,0x002BFF,0x0029FF,0x0027FF,0x0025FF,0x0023FF,0x0022FF,	// B0-B7
	0x0021FF,0x0020FF,0x001FFF,0x001EFF,0x001DFF,0x001CFF,0x001BFF,0x001AFF,	// B8-BF
	0x0019FF,0x0018FF,0x0017FF,0x0016FF,0x0015FF,0x0014FF,0x0014FF,0x0214FF,	// C0-27
	0x0414FF,0x0614FF,0x0814FF,0x0A14FF,0x0C14FF,0x0E14FF,0x1012FF,0x1210FF,	// C8-CF
	0x1412FF,0x1818FF,0x1C1CFF,0x2020FF,0x2424FF,0x2828FF,0x2C2CFF,0x3030FF,	// D0-D7
	0x3434FF,0x3838FF,0x3C3CFF,0x4444FF,0x4C4CFF,0x5454FF,0x5C5CFF,0x6464FF,	// D8-DF
	0x6C6CFF,0x7474FF,0x7C7CFF,0x8484FF,0x8C8CFF,0x9494FF,0x9C9CFF,0xA4A4FF,	// E0-E7
	0xACACFF,0xB4B4FF,0xBCBCFF,0xC4C4FF,0xC8C8FF,0xCCCCFF,0xD0D0FF,0xD4D4FF,	// E8-EF
	0xD8D8FF,0xDCDCFF,0xE0E0FF,0xE4E4FF,0xE9E9FF,0xEBEBFF,0xEDEDFF,0xEFEFFF,	// F0-F7
	0xF1F1FF,0xF3F3FF,0xF5F5FF,0xF7F7FF,0xF9F9FF,0xFBFBFF,0xFDFDFF,0xFFFFFF } ;	// F8-FF

void CCveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDER1, m_nSld);
	DDX_Control(pDX, IDC_SLIDER1, m_sldLmt);
	DDX_Control(pDX, IDC_EDIT1, m_editLmt);
	DDV_MinMaxInt(pDX, m_nSld, 0, 1023);
	DDX_Text(pDX, IDC_EDIT1, m_nEdit);
	DDV_MinMaxInt(pDX, m_nEdit, 0, 1023);
	DDX_Text(pDX, IDC_PATH, m_strPathName);
	DDX_Check(pDX, IDC_CAL, m_bCal);
	DDX_Check(pDX, IDC_PEAK, m_bPeak);
	DDX_Text(pDX, IDC_SLIDE_TEXT, m_strSlide);
	DDX_Slider(pDX, IDC_SLIDER2, m_nSldCore);
	DDX_Text(pDX, IDC_CORE, m_strCore);
	DDX_Text(pDX, IDC_XY, m_strXY);
	DDX_Text(pDX, IDC_STS, m_strSts);
	DDX_Check(pDX, IDC_CURV, m_bCurv);
	DDX_Text(pDX, IDC_DET, m_strDet);
	DDX_Control(pDX, IDC_RANGE, m_xcRange);
	DDX_Check(pDX, IDC_INCL, m_bIncl);
}

BEGIN_MESSAGE_MAP(CCveDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &CCveDlg::OnBnClickedOpen)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CALC, &CCveDlg::OnBnClickedCalc)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CCveDlg::OnNMCustomdrawSlider1)
	ON_EN_CHANGE(IDC_EDIT1, &CCveDlg::OnEnChangeEdit1)
	ON_EN_UPDATE(IDC_EDIT1, &CCveDlg::OnEnUpdateEdit1)
	ON_BN_CLICKED(IDOK, &CCveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CAL, &CCveDlg::OnBnClickedCal)
	//ON_BN_CLICKED(IDC_LUMP, &CCveDlg::OnBnClickedLump)
	ON_BN_CLICKED(IDC_Make, &CCveDlg::OnBnClickedMake)
	ON_BN_CLICKED(IDC_PEAK, &CCveDlg::OnBnClickedPeak)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CCveDlg::OnNMCustomdrawSlider2)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_LUMP, &CCveDlg::OnBnClickedLump)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_REF, &CCveDlg::OnBnClickedRef)
	ON_BN_CLICKED(IDC_CURV, &CCveDlg::OnBnClickedCurv)
	ON_BN_CLICKED(IDC_SAVE, &CCveDlg::OnBnClickedSave)
	ON_CBN_SELCHANGE(IDC_RANGE, &CCveDlg::OnCbnSelchangeRange)
	ON_BN_CLICKED(IDC_LZ4, &CCveDlg::OnBnClickedLz4)
	ON_BN_CLICKED(IDC_OCT, &CCveDlg::OnBnClickedOct)
	ON_BN_CLICKED(IDC_INCL, &CCveDlg::OnBnClickedIncl)
	ON_BN_CLICKED(IDC_ANLYZ, &CCveDlg::OnBnClickedAnlyz)
END_MESSAGE_MAP()


// CCveDlg メッセージ ハンドラー
BOOL CCveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	int		n, nOS;
	CString	str;
	str.Format("Cvx ver.%03d", VER);
	this->SetWindowTextA(str);

	//	ﾌﾗｸﾞ初期化
	bImg = false;
	bRef = false;
	bRcv = false;	//	湾曲補正完了ﾌﾗｸﾞを無効化
	bOrg = false;	//	原寸表示ﾌﾗｸﾞ
	nSlcPos = IMG_OFS;

	//	傾斜補正座標初期化
	ptD[0] = CPoint(X1, Y1);
	ptD[1] = CPoint(X2, Y2);
	ptD[2] = CPoint(X3, Y3);

	//	ComboBox初期化
	nViewRange = UM_RANGE;	//	20um
	m_xcRange.SetCurSel(2);	//

	//	ｱｸｾﾗﾚｰﾀ追加
	hAccl = ::LoadAccelerators(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	//	画像用ﾒﾓﾘ確保
	//uTmp = new uchar[PZ][PY][PC];	memset(uTmp, 0, PS);	//	1.5M	
	uDsp = new uchar[PZ][PY][PC];	memset(uDsp, 0, PS);	//	1.5M	断層画像(上)表示用ﾒﾓﾘ	
	uSfc = new uchar[PX][PY][PC];	memset(uSfc, 0, 2*PS);	//	3M		ｽﾗｲｽ画像(下)表示用ﾒﾓﾘ
	uBar = new uchar[2][PZ][PY][PC];memset(uBar, 0, 2*PS);	//	3M		ｶﾗｰﾊﾞｰ表示用ﾒﾓﾘ
	uBin = new uchar[PX][PY];		memset(uBin, 0, PX*PY);	//	1M		ﾜｰｸﾏｯﾌﾟ	

	//	画像表示ﾚﾃﾞｨﾌﾗｸﾞ
	bImg = true;		//	uDspとuSfcの用意ができたことを示すﾌﾗｸﾞ

	//	OCTﾃﾞｰﾀ用ﾒﾓﾘ
	uFrm = new uchar[PX][PZ][PY];		//	512M	湾曲変換前の全ﾃﾞｰﾀ
	uFro = new uchar[PX][PZ][PY];		//	512M	湾曲変換後の全ﾃﾞｰﾀ

	nPkPos = new int[PY];				memset(nPkPos, 0, PY*sizeof(int));
	pkDepth = new CPeak[PY][PX];		memset(pkDepth, 0, PX*PY*sizeof(CPeak));
	pkRef = new CPeak[PY][PX];			memset(pkRef, 0, PX*PY*sizeof(CPeak));

	int		ret;
	CFile	f;
	if(f.Open("c:\\temp\\cvx\\reference.dat", CFile::modeRead)){
		f.Read(pkDepth, f.GetLength());
		ret = LZ4_decompress_safe((char*)pkDepth, (char*)pkRef, f.GetLength(), sizeof(CPeak)*PX*PY);	//	pkDepth -> pkRef
		if(ret<=0){
			m_strDet.Format("error...");
			MessageBox(m_strDet);
		}
		f.Close();
		int	x, y;
		for(y=0; y<PY; y++){
			for(x=0; x<PX; x++){
				pkDepth[y][x].nOrg = 0.0;	//	初期画像ｸﾘｱ
			}
		}

		m_strSts.Format("補正値 Ready");
		UpdateData(FALSE);
	}

	//	BITMAPINFO構造体1(PX*PY*PC)
	bi.bmiHeader.biSize			= sizeof(BITMAPINFO);	//	sizeof(BITMAPINFO);
	bi.bmiHeader.biHeight		= PZ;		
	bi.bmiHeader.biWidth		= PY;		
	bi.bmiHeader.biSizeImage	= BMPSIZE;	
	bi.bmiHeader.biBitCount		= 24;					//	24bitColor
	bi.bmiHeader.biPlanes		= 1;
	bi.bmiHeader.biCompression	= 0;
	bi.bmiHeader.biClrUsed		= 0;
	bi.bmiHeader.biClrImportant	= 0;

	//	BITMAPINFO構造体2(PY*PY*PC)
	bp.bmiHeader.biSize			= sizeof(BITMAPINFO);	//	sizeof(BITMAPINFO);
	bp.bmiHeader.biHeight		= PY;		
	bp.bmiHeader.biWidth		= PX;		
	bp.bmiHeader.biSizeImage	= POLSIZE;	
	bp.bmiHeader.biBitCount		= 24;					//	24bitColor
	bp.bmiHeader.biPlanes		= 1;
	bp.bmiHeader.biCompression	= 0;
	bp.bmiHeader.biClrUsed		= 0;
	bp.bmiHeader.biClrImportant	= 0;

	//	LUT取得
	for(int i=0; i<256; i++){
		uClr[i][0] = (uchar)((0xff0000&ColorTable[i])>>16);
		uClr[i][1] = (uchar)((0x00ff00&ColorTable[i])>>8);
		uClr[i][2] = (uchar)(0x0000ff&ColorTable[i]);
	}
	makeBar();

	//	円筒描画ｸﾗｽをｻﾌﾞｸﾗｽ化
	CWnd* pw = GetDlgItem(IDC_CIRC);
	pw->SetWindowPos(&CWnd::wndTop,	0, 0, PZ, PZ, SWP_NOMOVE);	//	正方形に修正
	pw->GetClientRect(&rcCirc);
	m_Circ.pv = (void*)this;
	m_Circ.SubclassWindow(pw->GetSafeHwnd());

	//	初期処理
	m_strSlide.Format("<< 画像No. 0 - %d >>", PF-1);
	m_sldLmt.SetRange(0, PF-1);
	m_nEdit = m_nSld = 512;	//	画像No.
	UpdateData(FALSE);
	
	//	ｺｱ径
	CSliderCtrl*	ps;
	ps = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	ps->SetRange(0, 511, TRUE);
	
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CCveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

//	ｳｨﾝﾄﾞｳ破棄
void CCveDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	delete	pcbRed;
	delete	pcbPpl;
	delete	pcbGrn;
	delete	pcpX;
	delete	pcpG1;
	delete	pcpG2;
	delete	pcpR;
	delete	pcpP1;
	delete	pcpP2;
	delete	pcpG;
	delete	pcpB;
	delete	pcpW;
	delete	pcpP;
	delete	pcpDG;
	delete	pcpD;

	delete[]	uBar;
	delete[]	uDsp;
	delete[]	uSfc;
	delete[]	uBin;

	//delete[]	uTmp;
	delete[]	uFrm;
	delete[]	uFro;
	delete[]	nPkPos;
	delete[]	pkDepth;
	delete[]	pkRef;
}

//
BOOL CCveDlg::PreTranslateMessage(MSG* pMsg)
{
	if(hAccl)
		if(::TranslateAccelerator(m_hWnd, hAccl, pMsg))
			return	TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CCveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(!bImg)
			return;		//	画像ﾃﾞｰﾀの準備ができていないので即抜ける

		CPaintDC dc(this); // 描画用のデバイス コンテキスト
		dc.SetBkMode(TRANSPARENT);
		//dc.SetStretchBltMode(COLORONCOLOR);
		dc.SetStretchBltMode(HALFTONE);

		CString	str[4];
		//str[0].Format("元画像");			dc.TextOutA(10, 10, str[0]);			//	左上
		//str[1].Format("極座標表示");		dc.TextOutA(10, PZ/2+40, str[1]);		//	左下

		//////////////////////////////////////////////////////////
		//	①断層画像(上)
		StretchDIBits(	dc.m_hDC,
						10, IMG_OFS,
						PY/EXPND, PZ/EXPND,		//	出力座標
#ifdef	Z_RVS
						0, PZ, PY, -PZ,			//	元画像の座標
#else
						0, 0, PY, PZ,			//	元画像の座標
#endif
						uDsp, &bi,				//	画像データ
						DIB_RGB_COLORS,	SRCCOPY);
		//	②ｶﾗｰﾊﾞｰ(左)
		if(m_bCal)	StretchDIBits(	dc.m_hDC,
						0, 40+253,
						8, 512,					//	出力座標
						0, 0, 1, PZ,			//	元画像の座標
						uBar, &bi,				//	ｶﾗｰﾊﾞｰ
						DIB_RGB_COLORS,	SRCCOPY);
		else		StretchDIBits(	dc.m_hDC,
						0, 40+253,
						8, 512,					//	出力座標
						0, 0, 1, PZ,			//	元画像の座標
						uBar+1, &bi,			//	ﾓﾉｸﾛﾊﾞｰ
						DIB_RGB_COLORS,	SRCCOPY);

		//	③ﾌﾟﾛﾌｧｲﾙ描画(nOrg)
		if(m_bPeak){
			bool bZero = false;
			int	x, y, nd, nx;
			CString	s;
			//	③-1	目盛描画
			pOldPen = dc.SelectObject(pcpP2);	//	pink_dot
			for(nd=1; nd<10; nd++){
				dc.MoveTo(10, nd*PZ/EXPND/10+IMG_OFS);
				dc.LineTo(10+PY/EXPND, nd*PZ/EXPND/10+IMG_OFS);
			}
			dc.SelectObject(pcpP1);				//	pink_solid
			dc.MoveTo(10, PZ/EXPND/2+IMG_OFS);
			dc.LineTo(10+PY/EXPND, PZ/EXPND/2+IMG_OFS);

			//	③-2	断層ﾌﾟﾛﾌｧｲﾙ
			dc.SelectObject(pcpW);				//	purple
			dc.MoveTo(10, PZ/EXPND+IMG_OFS);
			for(x=10; x<10+PY/EXPND; x++){
				nx = (x-10)*EXPND;
#ifdef	Z_RVS
				y = IMG_OFS+pkDepth[nx][PY-m_nSld-1].nOrg;	//	XY軸交換,Y軸反転
#else
				y = IMG_OFS+PZ/EXPND-pkDepth[nx][PY-m_nSld-1].nOrg;	//	XY軸交換,Y軸反転
#endif
				if(bZero||(y>280))	dc.MoveTo(x, y);
				else				dc.LineTo(x, y);
				if(y>280)	bZero = true;
				else		bZero = false;
			}

			//	③-3	文字列表示
			dc.SelectObject(pOldPen);
			dc.SetTextColor(RGB(255, 128, 128)); //	pink
			pOldFont = (CFont*)dc.SelectObject(&Font2);
			s.Format("+%dum", nViewRange);
			dc.TextOut(15, 35, s);
			s.Format("  0um");
			dc.TextOut(15, 146, s);
			s.Format("-%dum", nViewRange);
			dc.TextOut(15, 270, s);
			dc.SelectObject(pOldFont);
		}else{
			//	③-4	ｽﾗｲｽﾗｲﾝ描画
			dc.SelectObject(pcpR);
			dc.MoveTo(10, m_nSldCore/EXPND+IMG_OFS);
			dc.LineTo(10+PY/EXPND, m_nSldCore/EXPND+IMG_OFS);
		}
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CCveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	RAWﾌｧｲﾙｵｰﾌﾟﾝ
void CCveDlg::OnBnClickedOpen()
{
	fileOpen();
}

//	OCTﾃﾞｰﾀを開く
void CCveDlg::OnBnClickedOct()
{
//	fileOpen(false);
}

//	ﾃﾞｰﾀﾌｧｲﾙｵｰﾌﾟﾝと補正一括処理
void CCveDlg::fileOpen()
{
	int			ret;
	double		dTm;
	CString		str;
	CWaitCursor	cur;
	CString		strFileName, strFilePath;
	CFile		f;
	dTm = GetPassTimeWindow();
	UpdateData();				//	値取得
	bRcv = false;				//	無補正ﾌﾗｸﾞ
	//m_bCurv = m_bIncl = true;	//	ﾃﾞｰﾀﾌｧｲﾙは湾曲補正・傾斜補正ありの状態で開く
	m_bCurv = true;				//	ﾃﾞｰﾀﾌｧｲﾙは湾曲補正ありの状態で開く
	UpdateData(FALSE);			//	値設定

	CFileDialog	dlg(TRUE, "*.oct", NULL, OFN_ALLOWMULTISELECT|OFN_EXPLORER, "octﾃﾞｰﾀﾌｧｲﾙ (*.oct)|*.oct|rawﾃﾞｰﾀﾌｧｲﾙ (*.raw)|*.raw||");

	if(dlg.DoModal() == IDOK){
		strFolderPath = dlg.GetPathName();
		//	ﾌｧｲﾙ拡張子で処理を分岐
		if(dlg.GetFileExt().MakeLower()=="oct"){
			imgOpen(strFolderPath, false);		//	圧縮ﾌｧｲﾙ(.oct)を開く
		}else{
			imgOpen(strFolderPath, true);		//	生ﾃﾞｰﾀ(.raw)を開く
		}

		//////////////////////////////////////////////////////////////////////////////
		//	面補正処理	//////////////////////////////////////////////////////////////
		memcpy(pkDepth, pkRef, sizeof(CPeak)*PX*PY);		//	基準値をｺﾋﾟｰして復元
		imgCalc();		//	湾曲補正処理	uFro <- uFrm
		imgCvFit();		//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞ	dCrv <- ﾋﾟｰｸ, uFrm

		if(bRef){
			//	基準面用の処理
			imgNrAvg();	//	近傍平均
			imgLwSub();	//	下限値からの差分値
			//	無用のﾃﾞｰﾀを削除
			for(int x=0; x<PX; x++){
				for(int y=0; y<PY; y++){
					pkDepth[y][x].dAvg = pkDepth[y][x].dCal = pkDepth[y][x].dSmp = pkDepth[y][x].dInc = 0.0;
				}
			}
			f.Open("c:\\temp\\cvx\\reference.dat", CFile::modeCreate|CFile::modeWrite);
			//	圧縮
			ret = LZ4_compress((char*)pkDepth, (char*)pkRef, sizeof(CPeak)*PX*PY);	//	pkDepth -> pkRef(圧縮)	
			if(ret<=0){
				m_strDet.Format("dat error.");
				MessageBox(m_strDet);
			}
			UpdateData(FALSE);
			f.Write(pkRef, ret);
			f.Close();
			memcpy(pkRef, pkDepth, sizeof(CPeak)*PX*PY);							//	pkDepth -> pkRef	
			m_strSts.Format("補正値 Ready");
		}else{
			//	評価用の処理
			imgMeas();		//	試料計測				dSmp <- dCal, dSub	<< dCal <- dCrv, dAvg
			imgIncl();		//	傾斜面補正値(dInc)算出	dInc <- dSmp
		}
		imgSurface();	//	面画像生成					nOrg <- dSmp, dInc

		//////////////////////////////////////////////////////////////////////////////
		m_strPathName.Format("%s", strFolderPath);
		UpdateData(FALSE);
	}
	imgChg();		//	Y-Z断面生成		uDsp <- uFro or uFrm
	imgSlc();		//	X-Y断面生成		uSfc <- nOrg or uFro or uFrm
	Invalidate(FALSE);

	m_strDet.Format("%.3f[ms]", GetPassTimeWindow()-dTm);
	UpdateData(FALSE);
	//MessageBox(str);}
}

//	基準面画像ﾌｧｲﾙｵｰﾌﾟﾝ
void CCveDlg::OnBnClickedRef()
{
	bRef = true;	//	唯一ここでだけtrue
	OnBnClickedOpen();
	bRef = false;
}

//	ﾌｧｲﾙｵｰﾌﾟﾝ処理
void CCveDlg::imgOpen(CString s, bool bRaw)
{
	CWaitCursor	cur;
	int			n;
	int			c, x, y, z;
	int			ret;
	CString		strName;
	CFile		f;
	int			(*nSum)[PX];
	int			tmp;
	uchar		uMax;

	//nSum = new int[PY][PX];
	//memset(nSum, 0, PX*PY*sizeof(int));

	//	ﾌｧｲﾙﾛｰﾄﾞ
	m_strPathName = s;	//	ﾌｫﾙﾀﾞｰﾊﾟｽを表示
	if(f.Open(s, CFile::modeRead)){
		if(!bRaw){
			//	解凍処理して読込み
			f.Read(uFrm, f.GetLength());
			ret = LZ4_decompress_safe((char*)uFrm, (char*)uFro, f.GetLength(), OCTSIZE);
			if(ret<=0){
				m_strDet.Format("error...");
				UpdateData(FALSE);
				f.Close();
				return;
			}
		}else{
			//	生ﾃﾞｰﾀ読込み
			f.Read(uFro, OCTSIZE);
		}
		f.Close();
	}else{
		MessageBox("File Open Error.");
	}

	//	表示用ﾒﾓﾘへｺﾋﾟｰ
#pragma omp parallel for
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			for(z=0; z<PZ; z++){
				uFrm[y][z][x] = ((uchar(*)[PX][PZ])((void*)uFro))[y][x][z];		//	XY回転(湾曲を見えるようにするため)
			}
		}
	}
	memcpy(uFro, uFrm, OCTSIZE);
}

//	全ﾌﾚｰﾑの湾曲補正処理(未使用)
void CCveDlg::OnBnClickedCalc()
{
	/*	
	imgCalc();
	imgCurv();

	imgChg();
	imgSlc();
	Invalidate(FALSE);
	*/
}

//	Z軸補正画像生成
void CCveDlg::imgCalc()
{
	int			x, y, z, c;
	int			tz[PY];

	for(x=0; x<PX; x++){
		for(z=0; z<PZ; z++){
#pragma omp parallel for
			for(y=0; y<PY; y++){
				//	ここでZ軸補正画像を作成したい
				tz[y] =  z - (int)pkDepth[y][x].dSub;	// + 512;	
				if(tz[y]<0)		tz[y] = 0;
				if(tz[y]>PZ)	tz[y] = PZ-1;
				uFro[x][tz[y]][y]  = uFrm[x][z][y];
			}
		}
	}
}

//	湾曲補正処理(未使用)
void CCveDlg::imgRcv(uchar dst[][PY][PC], uchar src[][PY][PC], int x)
{
/*	int		y, z, c;
	int		tz, vz;

	memset(dst, 0, PS);
#pragma omp parallel for
	for(z=0; z<PZ; z++){
		for(y=0; y<PY; y++){
			//	ここでZ軸補正画像を作成したい
			tz =  z - (int)pkDepth[y][x].dSub;	// + 512;	
			if(tz<0)	tz = 0;
			if(tz>PZ)	tz = PZ-1;
			memcpy(&dst[tz][y][0], &src[z][y][0], 3);	//	3s短縮
		}
	}
	*/
}

//	湾曲補正画像切り替え(未使用)
void CCveDlg::imgCurv()	
{
	/*
	int		x, y, z, c;
	UpdateData();
#pragma omp parallel for
	for(z=0; z<PZ; z++)
		for(y=0; y<PY; y++)
			memset(&uDsp[z][y][0], uFro[m_nSld][z][y], 3);
	bRcv = true;					//	湾曲補正完了ﾌﾗｸﾞ	
	*/
}



//	変更ｲﾍﾞﾝﾄ(未使用)
void CCveDlg::OnEnChangeEdit1()
{
	/*
	int		n;
	UpdateData();
	//m_sldLmt.SetPos(m_nSld);
	m_nSld = m_nEdit;
	UpdateData(FALSE);
	//memcpy(uSrc, uFrm+m_nSld, PS);
	OnBnClickedCalc();
	*/
}

//	Updateｲﾍﾞﾝﾄ
void CCveDlg::OnEnUpdateEdit1()
{
}

//	OK
void CCveDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

//	疑似ｶﾗｰﾁｪｯｸﾎﾞｯｸｽ
void CCveDlg::OnBnClickedCal()
{
	UpdateData();		//	取得
	imgChg();
	imgSlc();
}

//	仮画像生成処理
void CCveDlg::OnBnClickedMake()
{
	CWaitCursor	cur;
	imgMake();
}

//	仮画像生成
void CCveDlg::imgMake()
{
	/*
	//	cvx用にﾊﾟｽ
	CString	strPath, strName;
	int		n, nOS;

	strPath.Format("c:\\temp\\sdz\\cvcimage\\img_make\\");	//	ﾌｧｲﾙ生成するﾌｫﾙﾀﾞ

	m_strPathName.Format("c:\\temp\\sdz\\cvcimage\\src.jpg");
	img.Load(m_strPathName);
	CBitmap::FromHandle(img)->GetBitmap(&bmp);	//	BITMAPと連結
	pb = (uchar*)bmp.bmBits;					//	画像開始ｱﾄﾞﾚｽ取得
	memcpy(uSrc, pb, BMPSIZE);					//	表示用配列にｺﾋﾟｰ
	for(n=0; n<PF; n++){
		//	座標ｼﾌﾄ画像生成
		nOS = 10*n;
		memcpy(pb, uSrc+nOS, BMPSIZE-nOS*PX*PC);
		memcpy(pb+BMPSIZE-nOS*PX*PC, uSrc, nOS*PX*PC);
		//	画像保存
		strName.Format("%05d.jpg", n);
		img.Save(strPath+strName);
	}
	//pBmp->Detach();
	img.Destroy();
	*/
}

//	極座標変換
void CCveDlg::imgPol(uchar dst[][PD][PC], uchar src[][PX][PC])
{
/*	int		x, y, hx, hy, c, rad, rx, ry, core, px, py;
	double	dR, dR2, dRLmt, dCLmt, sita;

	core = m_nSldCore;			//	ｺｱ半径
	memset(dst, 255, POLSIZE);	//	背景を白化
	dRLmt = PD*PD/4;			//	最大外径
	dCLmt = core*core;			//	最小内径

	for(y=0; y<PD; y++){
		hy = y - PD/2+1;	//	中心座標からのY位置
		for(x=0; x<PD; x++){
			hx = x - PD/2+1;	//	中心座標からのX位置
			dR2 = hx*hx + hy*hy;
			if((dR2>dRLmt)|(dR2<dCLmt))
				continue;

			dR = sqrt(dR2)-core;		//	ｺｱを引いた半径
			sita = dSita[y][x]+PI;		//atan2((double)hy, (double)hx)+PI;

			rx = dR/(double)(PD/2-core)*(double)PX;		//	元画像のX座標
			if(rx<0)		rx = 0;
			else if(rx>=PX)	rx = PX-1;
			ry = sita/2.0/(double)PI*(double)PY;			//	元画像のY座標
			if(ry<0)		ry = 0;
			else if(ry>=PY)	ry = PY-1;

			for(c=0; c<PC; c++){
				dst[y][x][c] = src[ry][rx][c];
			}
		}
	}
	//	ﾋﾟｰｸを描画
	if(m_bPeak){
		for(y=0; y<PY; y++){
			sita = (double)y/PY*2*PI;
			px = PD/2+(PD/4+PK_GAIN*((double)nPkPos[y]-dPkAvg))*dTrg[y][1];	//	cos(sita);
			py = PD/2+(PD/4+PK_GAIN*((double)nPkPos[y]-dPkAvg))*dTrg[y][0];	//	sin(sita);
			//dst[py][px][0] = 255;
			//dst[py][px][1] = 100;
			//dst[py][px][2] = 100;
		}

	}
*/
}



//	ﾋﾟｰｸ検出処理
//void CCveDlg::imgPeak(uchar dst[][PX][PC], uchar src[][PX][PC])
void CCveDlg::imgCvFit()
{
	int		x, y, z, c, col;
	int		zs, ze;
	int		nFit[PX][FIT_NUM];
	int		nPeak[PX];
	int		nMax[PX];
	uchar	uDt[PX];
	double	dDst[PX][4];

	col = 0;
	//	ﾋﾟｰｸ座標検出(①nOrg)
	for(y=0; y<PY; y++){
#pragma omp parallel for
		for(x=0; x<PX; x++){
			nMax[x] = 0;
			pkDepth[y][x].nOrg = 100;	//	100で開始する
			for(z=0; z<PZ; z++){
				//uDt[x] = uFro[x][z][y];	//	uFro:無補正画像->補正画像
				uDt[x] = uFrm[x][z][y];	//	uFrm:無補正画像->補正画像
				if((nMax[x]<uDt[x])&&(50<uDt[x])&&(uDt[x]!=255)){	//	
					nMax[x] = uDt[x];				//	ﾋﾟｰｸ値保持
					pkDepth[y][x].nOrg = z;	//	ﾋﾟｰｸ座標保持
				}//	end if
			}//	end for z
		}//	end for x
	}//	end for y

	//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞ処理(②dCrv)
//#pragma omp parallel for
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			nPeak[y] = pkDepth[y][x].nOrg;
			if(	(nPeak[y]>FIT_NUM/2)&&			//	開始点条件
				(nPeak[y]<(PZ-FIT_NUM/2-1))){	//	停止点条件
					zs = nPeak[y] - FIT_NUM/2;	//	近傍座標開始点
					ze = nPeak[y] + FIT_NUM/2;	//	近傍座標停止点
					//	近傍点収集
					int n = 0;
					for(z=zs; z<=ze; z++){
						//nFit[y][n] = uFro[x][z][y];
						nFit[y][n] = uFrm[x][z][y];
						n++;
					}
					pkDepth[y][x].dCrv = imgFitting(dDst[y], nFit[y], nPeak[y]);
					
					pkDepth[y][x].a = dDst[y][0];
					pkDepth[y][x].b = dDst[y][1];
					pkDepth[y][x].c = dDst[y][2];
					memcpy(pkDepth[y][x].nFit, nFit[y], sizeof(int)*FIT_NUM);
					
			}else{
				//	ﾋﾟｰｸなし
				pkDepth[y][x].dCrv = 0.0;
			}//	end if
		}//	end for x
	}//	end for y
}

//	近傍平均値算出
//	基準面を想定しているので異常値排除は考慮していない
void CCveDlg::imgNrAvg()
{
	int		x, y, c;
	int		tx, ty;
	double	dSum;

	//	近傍平均値算出(③dAvg)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if((x<=AVG_FRM)||(x>(PX-AVG_FRM-1))||(y<=AVG_FRM)||(y>(PY-AVG_FRM-1))){
				//	ﾌﾚｰﾑｴﾘｱは実値
				pkDepth[y][x].dAvg = pkDepth[y][x].dCrv;
			}else{
				//	近傍平均値
				dSum = 0.0;
				for(ty=y-AVG_FRM; ty<=y+AVG_FRM; ty++){
					for(tx=x-AVG_FRM; tx<=x+AVG_FRM; tx++){
						dSum += pkDepth[y][x].dCrv;
					}
				}
				pkDepth[y][x].dAvg = dSum/(AVG_FRM*2+1)/(AVG_FRM*2+1);	//	近傍平均値
			}
		}
	}
}

//	下限値からの差分値
void CCveDlg::imgLwSub()
{
	int		x, y, c;
	int		tx, ty;
	double	dMin;
	CString	s;

	//	MIN探索(④MIN)
	//dMin = pkDepth[0][0].dCrv;
	dMin = pkDepth[0][0].dAvg;
	s.Format("dMin = %.3f", dMin);
	//MessageBox(s);
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if((dMin > pkDepth[y][x].dAvg)&&(pkDepth[y][x].dAvg!=0)){
				dMin = pkDepth[y][x].dAvg;
			}
		}
	}
	s.Format("dMin = %.3f", dMin);
	//MessageBox(s);
	//	最小値からの差分値算出(⑤dAvg, ⑥dCal)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			pkDepth[y][x].dSub = pkDepth[y][x].dAvg - dMin;			//	下限値からの差分値
			pkDepth[y][x].dCal = pkDepth[y][x].dSmp = 
						pkDepth[y][x].dCrv - pkDepth[y][x].dSub;	//	Test:差分値処理(実値-近傍平均値)
		}
	}
	///////////////////////////////////////////////////////////////////////////////
	//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞした座標出力(x=PX/2のﾗｲﾝ)
	
	CFile	f;
	//CString	s;
	f.Open("c:\\temp\\cvx\\peak.csv", CFile::modeCreate|CFile::modeWrite);
	s.Format("y, dCrv, dAvg, dCal");
	f.Write(s, s.GetLength());
	for(y=0; y<PY; y++){
		s.Format("%d,%.3f,%.3f,%.3f", y, pkDepth[y][PX/2].dCrv, pkDepth[y][PX/2].dAvg, pkDepth[y][PX/2].dCal);
		//s.Format("%d,%.3f,%.3f", y, pkDepth[y][PX/2].dSub, pkDepth[y][PX/2].dCal);
		/*
		for(int m=0; m<FIT_NUM; m++){
			s.Format("%s,%d", s, pkDepth[y][PX/2].nFit[m]);	//	周辺座標ﾃﾞｰﾀ
		}*/
		s.Format("%s\n", s);
		f.Write(s, s.GetLength());
	}
	f.Close();
	/**/
	///////////////////////////////////////////////////////////////////////////////
}

//	試料計測処理
//	dCrv, dSub -> dCal
//	dCal, dAvg -> dSmp
void CCveDlg::imgMeas()
{
	int		x, y, c;
	int		num;
	double	dAvg;

	//	面補正値再計算(⑥dCal)
	dAvg = 0.0;
	num = 0;
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(pkDepth[y][x].dCrv != 0){
				if(m_bCurv)		pkDepth[y][x].dCal = pkDepth[y][x].dCrv - pkDepth[y][x].dSub;	//	湾曲補正あり
				else			pkDepth[y][x].dCal = pkDepth[y][x].dCrv;						//	湾曲補正なし
				dAvg += pkDepth[y][x].dCal;
				num++;
			}else{
				pkDepth[y][x].dCal = 1000.0;		//	排除すべき特別な値
			}//	end if
		}//	end for x
	}//	end for y
	dAvg /= (double)num;	//	平均高さ算出(⑦AVG)
	//	平均0の差分ﾏｯﾌﾟ(⑧dSmp)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			pkDepth[y][x].dSmp = pkDepth[y][x].dCal - dAvg;
		}//	end for x
	}//	end for y
	Invalidate(FALSE);

	///////////////////////////////////////////////////////////////////////////////
	//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞした座標出力(x=PX/2のﾗｲﾝ)
	/*
	CFile	f;
	CString	s;
	f.Open("c:\\temp\\cvx\\peak.csv", CFile::modeCreate|CFile::modeWrite);
	for(y=0; y<PY; y++){
		//s.Format("%d,%.3f,%.3f", y, pkDepth[y][PX/2].dCrv, pkDepth[y][PX/2].dAvg);
		s.Format("%d,%.3f,%.3f,%.3f,%.3f", y, pkDepth[y][PX/2].dSmp, pkDepth[y][PX/2].dCal, pkDepth[y][PX/2].dSub, pkDepth[y][PX/2].dAvg);
		//for(int m=0; m<FIT_NUM; m++){
		//	s.Format("%s,%d", s, pkDepth[y][PX/2].nFit[m]);	//	周辺座標ﾃﾞｰﾀ
		//}
		s.Format("%s\n", s);
		f.Write(s, s.GetLength());
	}
	f.Close();
	/**/
	///////////////////////////////////////////////////////////////////////////////
}

//	面画像生成
//	pixel->umへ変換してnOrgへ再登録
//	dSmp -> nOrg
void CCveDlg::imgSurface()
{
	int		x, y, c;
	double	dRange = nViewRange;	//	±5um or ±10um or ±20um	
	int		nDt;

	//	面画像
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(m_bIncl){
				//	傾斜面補正あり
				nDt = (int)(128.0*UM_RATIO*(pkDepth[y][x].dSmp-pkDepth[y][x].dInc)/dRange+128.0);
			}else{
				//	傾斜面補正なし
				nDt = (int)(128.0*UM_RATIO*pkDepth[y][x].dSmp/dRange+128.0);	//	umに変換して[±dRange]で8bitに正規化
			}
			//nDt = 128;//(int)(128.0*(x/10)/dRange+128.0);
#ifdef	Z_RVS
			if(nDt>255)	nDt = 255;	//	255;
#else
			if(nDt>255)	nDt = 0;	//	255;
#endif
			if(nDt<0)	nDt = 255;	//	背景の白点除去
			pkDepth[y][x].nOrg = nDt;
		}
	}
}

//	湾曲補正ﾁｪｯｸﾎﾞｯｸｽ
void CCveDlg::OnBnClickedCurv()
{
	UpdateData();
	imgMeas();		//	湾曲補正
	imgUpdate();
}

//	ﾋﾟｰｸ検出ﾁｪｯｸﾎﾞｯｸｽ
void CCveDlg::OnBnClickedPeak()
{
	UpdateData();
	imgSlc();		//	X-Y断面生成
	CWnd*	pw;
	pw = GetDlgItem(IDC_RANGE);
	pw->EnableWindow(m_bPeak);
}


//	三角関数ﾃｰﾌﾞﾙ作成
void CCveDlg::makeTrig()
{
	/*
	int		x, y;
	double	sita;
	for(y=0; y<PY; y++){
		sita = 2*PI*(double)y/PY;
		dTrg[y][0] = sin(sita);
		dTrg[y][1] = cos(sita);
	}
	for(y=0; y<PD; y++){
		for(x=0; x<PD; x++){
			dSita[y][x] = atan2((double)(y-PD/2), (double)(x-PD/2));
		}
	}
	*/
}

//	任意X位置のY-Z断面画像生成
//	uDspの生成
void CCveDlg::imgChg()
{
	int		x, y, z, c;
	UpdateData();
	//m_strPathName.Format("Frame:04d/%04d", m_nEdit, PF-1);
	UpdateData(FALSE);

	//	表示色調選択
	//	疑似ｶﾗｰ表示
#pragma omp parallel for
	for(y=0; y<PY; y++){
		for(z=0; z<PZ; z++){
			if(m_bCal){
				//	疑似ｶﾗｰ表示
				if(m_bCurv)	memcpy(&uDsp[z][y][0], &uClr[uFro[PF-m_nSld-1][z][y]][0], 3);	//	湾曲補正画像
				else		memcpy(&uDsp[z][y][0], &uClr[uFrm[PF-m_nSld-1][z][y]][0], 3);	//	無補正画像
			}else{
				//	ｸﾞﾚｰｽｹｰﾙ表示
				if(m_bCurv)	memset(&uDsp[z][y][0], uFro[PF-m_nSld-1][z][y], 3);	//	湾曲補正画像
				else		memset(&uDsp[z][y][0], uFrm[PF-m_nSld-1][z][y], 3);	//	無補正画像
			}
		}
	}
	Invalidate(FALSE);
}

//	任意ZによるX-Y断面画像生成
//	uSfcの生成
void CCveDlg::imgSlc()
{
	int	x, y, c;
	UpdateData();

#pragma omp parallel for
	for(x=0; x<PX; x++){
		for(y=0; y<PY ;y++){
#ifdef	Z_RVS
			//	Peak表面表示
			if(m_bPeak){
				//	表面画像は色調反転
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[255-(uchar)pkDepth[y][x].nOrg][0], 3);	//	疑似ｶﾗｰ
				else		memset(&uSfc[x][y][0], (255-(uchar)pkDepth[y][x].nOrg), 3);			//	ﾓﾉｸﾛ
			}else if(m_bCurv){
				//	湾曲補正後ｽﾗｲｽ画像
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFro[x][m_nSldCore][y]][0], 3);		//	疑似ｶﾗｰ
				else		memset(&uSfc[x][y][0], uFro[x][m_nSldCore][y], 3);					//	ﾓﾉｸﾛ
			}else{
				//	湾曲補正前ｽﾗｲｽ画像
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFrm[x][m_nSldCore][y]][0], 3);		//	疑似ｶﾗｰ
				else		memset(&uSfc[x][y][0], uFrm[x][m_nSldCore][y], 3);					//	ﾓﾉｸﾛ
			}
#else
			//	Peak表面表示
			if(m_bPeak){
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[(uchar)pkDepth[y][x].nOrg][0], 3);		//	疑似ｶﾗｰ
				else		memset(&uSfc[x][y][0], (uchar)(pkDepth[y][x].nOrg), 3);				//	ﾓﾉｸﾛ
			}else if(m_bCurv){
				//	湾曲補正後ｽﾗｲｽ画像
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFro[x][PZ-m_nSldCore-1][y]][0], 3);	//	疑似ｶﾗｰ
				else		memset(&uSfc[x][y][0], uFro[x][PZ-m_nSldCore-1][y], 3);				//	ﾓﾉｸﾛ
			}else{
				//	湾曲補正前ｽﾗｲｽ画像
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFrm[x][PZ-m_nSldCore-1][y]][0], 3);	//	疑似ｶﾗｰ
				else		memset(&uSfc[x][y][c], uFrm[x][PZ-m_nSldCore-1][y], 3);				//	ﾓﾉｸﾛ
			}
#endif
		}
	}
	Invalidate(FALSE);
}

//	ﾏｳｽ移動
void CCveDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// 左ﾎﾞﾀﾝが押されている状態
	switch(nFlags){
	case	MK_RBUTTON:
		if(m_bPeak)
			break;
		if((point.x>20)&&point.x<(20+PY/2)&&(point.y>IMG_OFS)&&(point.y<30+PZ/2)){
			nSlcPos = (point.y-IMG_OFS)*2;
			if(nSlcPos<0)		nSlcPos = 0;
			if(nSlcPos>(PZ-1))	nSlcPos = PZ-1;
			m_nSldCore = nSlcPos;
			UpdateData(FALSE);
			m_strCore.Format("%d", nSlcPos);
			imgSlc();
			UpdateData(FALSE);
		}
		break;
	case	MK_LBUTTON:
		break;
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

//	左ﾎﾞﾀﾝｸﾘｯｸ
void CCveDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
}

//	ｽﾗｲﾀﾞｰ1の処理
void CCveDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData();
	if(m_nEdit != m_nSld){
		m_nEdit = m_nSld;
		UpdateData(FALSE);
		imgChg();			//	X-Z断面生成
	}
	*pResult = 0;
}

//	ｽﾗｲﾀﾞｰ2の処理
void CCveDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	static	int	n;
	UpdateData();
	if(m_nSldCore != n){
		n = m_nSldCore;
		m_strCore.Format("%d", m_nSldCore);
		UpdateData(FALSE);
		imgSlc();			//	X-Y断面生成
	}
	*pResult = 0;
}

//	Test
void CCveDlg::OnBnClickedLump()
{
	imgIncl();
	//OnCbnSelchangeRange();
	UpdateData();
	imgUpdate();
}

//	平均座標取得
void CCveDlg::ptAvg(double dst[], CPoint p)
{
	int		x, y, z, sx, sy, sn;
	int		i, j, k;
	double	dSum, dAvg, dStd;
	double	dP[3];

	sn = 0;
	dSum = dAvg = 0.0;
	//	通常の平均値算出
	for(y=p.y-5; y<p.y+5; y++){
		for(x=p.x-5; x<p.x+5; x++){
			//	異常値除去処理を追加

			//	累計
			dSum += pkDepth[y][x].dSmp;
			sn++;
		}
	}
	dAvg = dSum/(double)sn;	//	平均
	dSum = dStd = 0.0;
	//	標準偏差算出
	for(y=p.y-5; y<p.y+5; y++){
		for(x=p.x-5; x<p.x+5; x++){
			dSum += ((dAvg - pkDepth[y][x].dSmp)*(dAvg - pkDepth[y][x].dSmp));
		}
	}
	dStd = sqrt(dSum/(double)sn);	//	標準偏差
	//	標準偏差内による平均座標高さ算出
	sn = sx = sy = 0;
	dSum = 0.0;
	for(y=p.y-5; y<p.y+5; y++){
		for(x=p.x-5; x<p.x+5; x++){
			if(dStd < fabs(dAvg-pkDepth[y][x].dSmp))
				continue;
			dSum += pkDepth[y][x].dSmp;
			sx += x;
			sy += y;
			sn++;
		}
	}
	dst[0] = (double)sx/(double)sn;	//	X座標平均値
	dst[1] = (double)sy/(double)sn;	//	Y座標平均値
	dst[2] = dSum/(double)sn;		//	Z座標平均値
	dst = dP;
}


//	傾斜面補正値算出
//	dInc <- dSmp
void CCveDlg::imgIncl()
{
	CWaitCursor	cur;
	int		i, j, k, x, y;
	int		n = 3;		//	行列の次数
	double	dZ[3] = {0};//	dZ座標
	double	dP[3] = {0};//	ﾊﾟﾗﾒｰﾀ{a, b, c}
	double	dQ[3] = {0};
	double	buf;

	//	元ﾃﾞｰﾀの代入
	double	dT[3][3] = {0};
	double	dA[3][3] = {{ptD[0].x, ptD[0].y, 1},
						{ptD[1].x, ptD[1].y, 1},
						{ptD[2].x, ptD[2].y, 1}};

	for(i=0; i<n; i++){
		dZ[i] = pkDepth[ptD[i].y][ptD[i].x].dSmp;
	}
	/**/
	

	
	for(n=0; n<3; n++){
		ptAvg(dQ, ptD[n]);
		//	XY
		dA[n][0] = dQ[0];	//	X
		dA[n][1] = dQ[1];	//	Y
		dA[n][2] = 1;
		dZ[n] = dQ[2];		//	Z
	}
	/**/
	//単位行列を作る
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			dT[i][j] = (i==j) ? 1.0 : 0.0;
		}
	}
	
	//	逆行列演算(掃き出し法)
	for(i=0; i<n; i++){
		buf = 1.0 / dA[i][i];
		for(j=0;j<n;j++){
			dA[i][j] *= buf;
			dT[i][j] *= buf;
		}
		for(j=0; j<n; j++){
			if(i!=j){
				buf = dA[j][i];
				for(k=0; k<n; k++){
					dA[j][k] -= dA[i][k] * buf;
					dT[j][k] -= dT[i][k] * buf;
				}
			}
		}
	}

	//	ﾊﾟﾗﾒｰﾀ逆算
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			dP[i] += dT[i][j] * dZ[j];
		}
	}
	
	//	傾斜面補正値算出
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			pkDepth[y][x].dInc = (dP[0]*x + dP[1]*y + dP[2]);
			//pkDepth[y][x].dSmp -= (dP[0]*x + dP[1]*y + dP[2]);
		}
	}
}

//	ｶｰﾌﾞﾌｨｯﾃｨﾝｸﾞ処理
//	ex.	y = ax^2 + bx + c
//	usage	o	dDst	計算結果(a,b,c,d)	
//			i	nSrc	ﾋﾟｰｸを中心に7ﾃﾞｰﾀ
//			i	nPk		ﾋﾟｰｸ座標
//			ret	dPk		ﾋﾟｰｸ位置計算値
double CCveDlg::imgFitting(double dDst[], int nSrc[], int nPk)
{
	int			n, z, nm;
	double		dp, dt, dPk;
	long double	a, b, c, d, sm;
	long double	sx1, sx2, sx3, sx4;
	long double sx2y, sxy, sy1;

	nm = FIT_NUM;
	sx1 = sx2 = sx3 = sx4 = 0.0;
	sx2y = sxy = sy1 = 0.0;

	for(z=0; z<nm; z++){
		dp = z - (int)(FIT_NUM/2);	//	position
		dt = nSrc[z];				//	data
		sx1 += dp;
		sx2 += dp*dp;
		sx3 += dp*dp*dp;
		sx4 += dp*dp*dp*dp;
		sy1 += dt;
		sxy += dp*dt;
		sx2y += dp*dp*dt;
	}
	sm = sx4*sx2*nm + 2*sx3*sx2*sx1 -sx2*sx2*sx2 - sx3*sx3*nm - sx1*sx1*sx4;
	if(sm != 0){
		a = (sx2y*sx2*nm + sx3*sx1*sy1 + sxy*sx1*sx2 - sx2*sx2*sy1 - sx3*sxy*nm - sx1*sx1*sx2y) / sm;
		b = (sx4*sxy*nm + sx2y*sx1*sx2 + sx3*sy1*sx2 - sx2*sxy*sx2 - sx2y*sx3*nm - sx1*sy1*sx4) / sm;
		c = (sx4*sx2*sy1 + sx3*sxy*sx2 + sx3*sx1*sx2y - sx2y*sx2*sx2 - sx3*sx3*sy1 - sxy*sx1*sx4) / sm;
		//	ﾋﾟｰｸ点計算
		if(a!=0)	dPk = -((double)b/2.0/(double)a) + (double)nPk;	
		else		dPk = 0.0;
	}else{
		a = b = c = 0.0;
		d = 0.0;
	}
	dDst[0] = (double)a;
	dDst[1] = (double)b;
	dDst[2] = (double)c;
	dDst[3] = dPk;

	return	dPk;
}

//	ﾏｳｽ左ﾎﾞﾀﾝｱｯﾌﾟ
void CCveDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_Circ.SendMessage(WM_LBUTTONUP);
	CDialogEx::OnLButtonUp(nFlags, point);
}

//	画像保存
void CCveDlg::OnBnClickedSave()
{	
	CWaitCursor	cus;
	CDC* dc(m_Circ.GetDC());
	CBitmap bmpForSave;
	CDC cdcForSave;
	CRect r;

	m_Circ.GetClientRect(&r);
	//bmpForSave.CreateCompatibleBitmap(dc, r.Width(), r.Height());
	bmpForSave.CreateCompatibleBitmap(dc, PX, PY);
	cdcForSave.CreateCompatibleDC(dc);
	cdcForSave.SelectObject(bmpForSave);

	StretchDIBits(	cdcForSave.m_hDC,
					//0, 0, PZ, PZ,				//	出力座標
					0, 0, PX, PY,				//	出力座標
					0, 0, PX, PY,				//	元画像の座標
					uSfc, &bp,			//	画像データ
					DIB_RGB_COLORS,	SRCCOPY);

	img.Attach((HBITMAP)bmpForSave);

	// [名前を付けて保存]ダイアログ
	CFileDialog	dlg(FALSE, ".jpg", "View", OFN_OVERWRITEPROMPT,
							"画像ファイル (*.jpg)|*.jpg|全てのファイル(*.*)|*.*||");

	if(dlg.DoModal() == IDOK){
		CString strMsg;
		strMsg.Format(("ファイル名：%s\nパス：%s"),  dlg.GetFileName(), dlg.GetPathName());
		//MessageBox(strMsg);
		img.Save(dlg.GetPathName());
	}
}

//	ﾚﾝｼﾞ選択
void CCveDlg::OnCbnSelchangeRange()
{
	switch(m_xcRange.GetCurSel()){
	case	0:
		nViewRange = 5;	//	5um
		break;
	case	1:
		nViewRange = 10;	//	10um
		break;
	case	2:
		nViewRange = 20;	//	20um
		break;
	default:
		nViewRange = 50;	//	50um
	}
	imgUpdate();
}

//	ｶﾗｰﾊﾞｰ作成
void CCveDlg::makeBar()
{
	int		z, c;
	uchar	uCol;

	for(z=0; z<PZ; z++){
		memcpy(&uBar[0][z][0][0], &uClr[(z/2)][0], 3);	//	ｶﾗｰ
		memset(&uBar[1][z][0][0], (uchar)(z/2), 3);		//	ﾓﾉｸﾛ
	}
}

//	圧縮変換処理
void CCveDlg::OnBnClickedLz4()
{
	CWaitCursor	cur;
	int			n;
	int			ret;
	int			size;
	CString		s;
	double		dTm;
	CFile		f;

	CFileDialog	dlg(TRUE, "*.raw", NULL, OFN_ALLOWMULTISELECT|OFN_EXPLORER, "rawﾃﾞｰﾀﾌｧｲﾙ (*.raw)|*.raw");
	if(dlg.DoModal() == IDOK){
		bImg = false;
		//	ﾌｧｲﾙﾘｰﾄﾞ
		strFolderPath = dlg.GetPathName();
		f.Open(strFolderPath, CFile::modeRead);
		f.Read(uFro, OCTSIZE);
		f.Close();
		//	LZ4圧縮
		dTm = GetPassTimeWindow();
		ret = LZ4_compress((char*)uFro, (char*)uFrm, OCTSIZE);
		if(ret<=0){
			m_strDet.Format("error, %.1f[ms]", GetPassTimeWindow() - dTm);
		}else{
			m_strDet.Format("%d[Mbyte], %.1f[ms]", ret/1024/1024, GetPassTimeWindow() - dTm);	//	21MB
		}
		UpdateData(FALSE);
		//	ﾌｧｲﾙ保存
		n = dlg.GetFolderPath().ReverseFind('\\');
		s = dlg.GetFolderPath().Right(dlg.GetFolderPath().GetLength()-n);
		s = dlg.GetFolderPath() + s + ".oct";
		f.Open(s, CFile::modeCreate|CFile::modeWrite);
		f.Write(uFrm, ret);
		f.Close();
		memset(uFrm, 0, OCTSIZE);
		memset(uFro, 0, OCTSIZE);
		bImg = true;
	}
}

//	傾斜面補正ﾁｪｯｸﾎﾞｯｸｽ
void CCveDlg::OnBnClickedIncl()
{
	UpdateData();
	imgUpdate();
}

//	描画更新
void CCveDlg::imgUpdate()
{
	imgSurface();
	imgSlc();
	imgChg();
}

//	解析処理(特徴量抽出)
void CCveDlg::OnBnClickedAnlyz()
{
	imgTrim();
	imgFlat();
	Invalidate();
}

//	面評価
void CCveDlg::imgFlat()
{
	int		x, y, c, n;
	double	dRn, dRi, dRo;
	CPoint	ptCg;
	
	dRi = crIn.dR;		//	内径
	dRo = crOut.dR;		//	外径
	ptCg = crIn.ptCg;	//	重心

	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			dRn = sqrt((double)((x-ptCg.x)*(x-ptCg.x)+(y-ptCg.y)*(y-ptCg.y)));
			if((dRn>dRi)&&(dRn<dRo)){
				//uSfc[x][y][0] = 255;		//	test
				//	ﾜｰｸ表面

			}
		}

	}
}

//	円周評価
void CCveDlg::imgTrim()
{
	int		x, y, c, n;
	int		col = RED;
	int		nSumN, nSumX, nSumY;
	int		nCnt;
	double	dRn, dX, dY, dRad;
	double	dLen;
	double	dSita;

	dSita = 2*PI/PR;
	nSumN = nSumX = nSumY = 0;
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(uSfc[x][y][col] != 0){
				nSumN++;	//	有効画素数
				nSumX += x;	//	X座標累計
				nSumY += y;	//	Y座標累計
			}
		}
	}

	//	重心算出
	crIn.ptCg.x  = (int)((double)nSumX/(double)nSumN+0.5);
	crIn.ptCg.y  = (int)((double)nSumY/(double)nSumN+0.5);
	crOut.ptCg = crIn.ptCg;

	//	Inline, Outline
	nCnt = 0;
	for(dRad=-PI; dRad<PI; dRad+=dSita){
		//	Inline
		for(dLen=0; dLen<PX/2; dLen+=0.1){
			dX = dLen * cos(dRad) + crIn.ptCg.x;
			dY = dLen * sin(dRad) + crIn.ptCg.y;
			if((dX>0)&&(dX<PX)&&(dY>0)&&(dY<PY)){
				if(uSfc[(int)dX][(int)dY][col] > TH_OTL){
					//uSfc[(int)dX][(int)dY][col] = 255;
					break;
				}
			}
		}
		crIn.ptCirc[nCnt] = CPoint((int)dX, (int)dY);	//	輪郭点座標群

		//	Outline
		for(dLen=PX/2; dLen>0; dLen-=0.1){
			dX = dLen * cos(dRad) + crOut.ptCg.x;
			dY = dLen * sin(dRad) + crOut.ptCg.y;
			if((dX>0)&&(dX<PX)&&(dY>0)&&(dY<PY)){
				if(uSfc[(int)dX][(int)dY][col] > TH_OTL){
					//uSfc[(int)dX][(int)dY][col] = 255;
					break;
				}
			}
		}
		crOut.ptCirc[nCnt] = CPoint((int)dX, (int)dY);	//	輪郭点座標群
		nCnt++;		//	ｶｳﾝﾀｰ
	}
	

	//	輪郭ﾃﾞｰﾀ分析(dDist, dRy, dRa)
	crIn.calc();
	crOut.calc();

	//	ﾜｰｸ検出
	memset(uBin, 0, PX*PY);	
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			dRn = sqrt((double)((x-crIn.ptCg.x)*(x-crIn.ptCg.x)+(y-crIn.ptCg.y)*(y-crIn.ptCg.y)));
			if((dRn>crIn.dR)&&(dRn<crOut.dR)){
				uSfc[x][y][0] = 255;		//	test
				//	ﾜｰｸ表面
				uBin[x][y] = 255;

			}
		}

	}

}


