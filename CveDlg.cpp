
// CveDlg.cpp : �����t�@�C��
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
// *  �@�\�FWindows �N����Ɍo�ߎ��Ԃ�m�b�P�ʂŕԂ�                      *
// *                                                                    * 
// * Input : ����                                                       *
// *                                                                    *   
// * OutPut: =  �o�ߎ���                                                *
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



// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
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


// CCveDlg �_�C�A���O
CCveDlg::CCveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCveDlg::IDD, pParent)
	, m_nSld(512)
	, m_nEdit(0)
	, m_strPathName(_T(""))
	, m_bCal(FALSE)				//	�^���װ
	, m_bPeak(TRUE)
	, m_strSlide(_T(""))
	, m_nSldCore(CORE_R)
	, m_strCore(_T(""))
	, m_strXY(_T(""))
	, m_strSts(_T(""))
	, m_bCurv(TRUE)
	, m_strDet(_T(""))
	, m_bIncl(FALSE)			//	�X�Ζʕ␳
	, m_bInv(FALSE)
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
	Font1.CreateFont(	20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, //** �t�H���g�ύX
						SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, "�l�r �o�S�V�b�N");
	Font2.CreateFont(	12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, //** �t�H���g�ύX
						SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						DEFAULT_PITCH, "�l�r �o�S�V�b�N");
	pOldPen;

}

//	�װϯ��
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
	DDX_Check(pDX, IDC_INV, m_bInv);
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
	ON_BN_CLICKED(IDC_IMG_OPEN, &CCveDlg::OnBnClickedImgOpen)
	ON_BN_CLICKED(IDC_INV, &CCveDlg::OnBnClickedInv)
END_MESSAGE_MAP()


// CCveDlg ���b�Z�[�W �n���h���[
BOOL CCveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
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

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	int		n, nOS;
	CString	str;
	str.Format("Cvx ver.%03d", VER);
	this->SetWindowTextA(str);

	//	�׸ޏ�����
	bImg = false;
	bRef = false;
	bRcv = false;	//	�p�ȕ␳�����׸ނ𖳌���
	bOrg = false;	//	�����\���׸�
	bAnlyz = false;	//	���͊����׸�
	nSlcPos = IMG_OFS;

	//	ComboBox������
	nViewRange = UM_RANGE;	//	20um
	m_xcRange.SetCurSel(2);	//

	//	����ڰ��ǉ�
	hAccl = ::LoadAccelerators(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	//	�摜�p��؊m��
	//uTmp = new uchar[PZ][PY][PC];	memset(uTmp, 0, PS);	//	1.5M	
	uDsp = new uchar[PZ][PY][PC];	memset(uDsp, 0, PS);	//	1.5M	�f�w�摜(��)�\���p���	
	uSfc = new uchar[PX][PY][PC];	memset(uSfc, 0, 2*PS);	//	3M		�ײ��摜(��)�\���p���
	uBar = new uchar[2][PZ][PY][PC];memset(uBar, 0, 2*PS);	//	3M		�װ�ް�\���p���
	uBin = new uchar[PX][PY];		memset(uBin, 0, PX*PY);	//	1M		ܰ�ϯ��	

	//	�摜�\����ި�׸�
	bImg = true;		//	uDsp��uSfc�̗p�ӂ��ł������Ƃ������׸�

	//	OCT�ް��p���
	uFrm = new uchar[PX][PZ][PY];		//	512M	�p�ȕϊ��O�̑S�ް�
	uFro = new uchar[PX][PZ][PY];		//	512M	�p�ȕϊ���̑S�ް�

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
				pkDepth[y][x].nOrg = 0.0;	//	�����摜�ر
#ifdef Z_RVS
				//	�␳�ް������]
				pkDepth[y][x].dSub *= -1;	//	�␳�l�𔽓](�����Ɏg�p)
				pkRef[y][x].dSub *= -1;		//	�␳�l�𔽓]
#endif // Z_RVS

			}
		}

		m_strSts.Format("�␳�l Ready");
		UpdateData(FALSE);
	}

	//	BITMAPINFO�\����1(PX*PY*PC)
	bi.bmiHeader.biSize			= sizeof(BITMAPINFO);	//	sizeof(BITMAPINFO);
	bi.bmiHeader.biHeight		= PZ;		
	bi.bmiHeader.biWidth		= PY;		
	bi.bmiHeader.biSizeImage	= BMPSIZE;	
	bi.bmiHeader.biBitCount		= 24;					//	24bitColor
	bi.bmiHeader.biPlanes		= 1;
	bi.bmiHeader.biCompression	= 0;
	bi.bmiHeader.biClrUsed		= 0;
	bi.bmiHeader.biClrImportant	= 0;

	//	BITMAPINFO�\����2(PY*PY*PC)
	bp.bmiHeader.biSize			= sizeof(BITMAPINFO);	//	sizeof(BITMAPINFO);
	bp.bmiHeader.biHeight		= PY;		
	bp.bmiHeader.biWidth		= PX;		
	bp.bmiHeader.biSizeImage	= POLSIZE;	
	bp.bmiHeader.biBitCount		= 24;					//	24bitColor
	bp.bmiHeader.biPlanes		= 1;
	bp.bmiHeader.biCompression	= 0;
	bp.bmiHeader.biClrUsed		= 0;
	bp.bmiHeader.biClrImportant	= 0;

	//	LUT�擾
	for(int i=0; i<256; i++){
		uClr[i][0] = (uchar)((0xff0000&ColorTable[i])>>16);
		uClr[i][1] = (uchar)((0x00ff00&ColorTable[i])>>8);
		uClr[i][2] = (uchar)(0x0000ff&ColorTable[i]);
	}
	makeBar();

	//	�~���`��׽���޸׽��
	CWnd* pw = GetDlgItem(IDC_CIRC);
	pw->SetWindowPos(&CWnd::wndTop,	0, 0, PZ, PZ, SWP_NOMOVE);	//	�����`�ɏC��
	pw->GetClientRect(&rcCirc);
	m_Circ.pv = (void*)this;
	m_Circ.SubclassWindow(pw->GetSafeHwnd());

	//	��������
	m_strSlide.Format("<< �摜No. 0 - %d >>", PF-1);
	m_sldLmt.SetRange(0, PF-1);
	m_nEdit = m_nSld = 512;	//	�摜No.
	UpdateData(FALSE);
	
	//	���a
	CSliderCtrl*	ps;
	ps = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	ps->SetRange(0, 511, TRUE);
	
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

//	����޳�j��
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


// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CCveDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(!bImg)
			return;		//	�摜�ް��̏������ł��Ă��Ȃ��̂ő�������

		CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g
		dc.SetBkMode(TRANSPARENT);
		//dc.SetStretchBltMode(COLORONCOLOR);
		dc.SetStretchBltMode(HALFTONE);

		CString	str[4];
		//str[0].Format("���摜");			dc.TextOutA(10, 10, str[0]);			//	����
		//str[1].Format("�ɍ��W�\��");		dc.TextOutA(10, PZ/2+40, str[1]);		//	����

		//////////////////////////////////////////////////////////
		//	�@�f�w�摜(��)
		StretchDIBits(	dc.m_hDC,
						10, IMG_OFS,
						PY/EXPND, PZ/EXPND,		//	�o�͍��W
						0, PZ, PY, -PZ,			//	���摜�̍��W
						uDsp, &bi,				//	�摜�f�[�^
						DIB_RGB_COLORS,	SRCCOPY);
		//	�A�װ�ް(��)
		if(m_bCal)	StretchDIBits(	dc.m_hDC,
						0, 40+253,
						8, 512,					//	�o�͍��W
						0, 0, 1, PZ,			//	���摜�̍��W
						uBar, &bi,				//	�װ�ް
						DIB_RGB_COLORS,	SRCCOPY);
		else		StretchDIBits(	dc.m_hDC,
						0, 40+253,
						8, 512,					//	�o�͍��W
						0, 0, 1, PZ,			//	���摜�̍��W
						uBar+1, &bi,			//	�ɸ��ް
						DIB_RGB_COLORS,	SRCCOPY);

		//	�B���̧�ٕ`��(nOrg)
		if(m_bPeak){
			bool bZero = false;
			int	x, y, nd, nx;
			CString	s;
			//	�B-1	�ڐ��`��
			pOldPen = dc.SelectObject(pcpP2);	//	pink_dot
			for(nd=1; nd<10; nd++){
				dc.MoveTo(10, nd*PZ/EXPND/10+IMG_OFS);
				dc.LineTo(10+PY/EXPND, nd*PZ/EXPND/10+IMG_OFS);
			}
			dc.SelectObject(pcpP1);				//	pink_solid
			dc.MoveTo(10, PZ/EXPND/2+IMG_OFS);
			dc.LineTo(10+PY/EXPND, PZ/EXPND/2+IMG_OFS);

			//	�B-2	�f�w���̧��
			dc.SelectObject(pcpW);				//	purple
			dc.MoveTo(10, PZ/EXPND+IMG_OFS);
			for(x=10; x<10+PY/EXPND; x++){
				nx = (x-10)*EXPND;
				y = IMG_OFS+pkDepth[nx][m_nSld].nOrg;	//	XY������,Y�����]
				if(bZero||(y>280))	dc.MoveTo(x, y);
				else				dc.LineTo(x, y);
				if(y>280)	bZero = true;
				else		bZero = false;
			}

			//	�B-3	������\��
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
			//	�B-4	�ײ�ײݕ`��
			dc.SelectObject(pcpR);
			dc.MoveTo(10, m_nSldCore/EXPND+IMG_OFS);
			dc.LineTo(10+PY/EXPND, m_nSldCore/EXPND+IMG_OFS);
		}
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CCveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//	OCT�ް����J��
void CCveDlg::OnBnClickedOct()
{
//	fileOpen(false);
}

//	RAW̧�ٵ����
void CCveDlg::OnBnClickedOpen()
{
	fileOpen();
}

//	�ް�̧�ٵ���݂ƕ␳�ꊇ����
void CCveDlg::fileOpen()
{
	int			ret;
	double		dTm;
	CString		str;
	CWaitCursor	cur;
	CString		strFileName, strFilePath;
	CFile		f;
	dTm = GetPassTimeWindow();
	UpdateData();				//	�l�擾
	bRcv = false;				//	���␳�׸�
	m_bCurv = true;				//	�ް�̧�ق͘p�ȕ␳����̏�ԂŊJ��
	m_bIncl = false;			//	�X�Ζʕ␳�͏����ɂ͕s���{
	UpdateData(FALSE);			//	�l�ݒ�

	CFileDialog	dlg(TRUE, "*.oct", NULL, OFN_ALLOWMULTISELECT|OFN_EXPLORER, "oct�ް�̧�� (*.oct)|*.oct|raw�ް�̧�� (*.raw)|*.raw||");

	if(dlg.DoModal() == IDOK){
		bAnlyz = false;
		strSfcImg = dlg.GetFileName();
		strFolderPath = dlg.GetPathName();
		//	̧�يg���q�ŏ����𕪊�
		if(dlg.GetFileExt().MakeLower()=="oct"){
			imgOpen(strFolderPath, false);		//	���ķ��(.oct)���J��
		}else{
			imgOpen(strFolderPath, true);		//	���ް�(.raw)���J��
		}

		//////////////////////////////////////////////////////////////////////////////
		//	�ʕ␳����	//////////////////////////////////////////////////////////////
		memcpy(pkDepth, pkRef, sizeof(CPeak)*PX*PY);		//	��l���߰���ĕ���
		imgCvFit();		//	����̨�èݸ�	�AdCrv <- �߰�, uFrm

		if(bRef){
			//	��ʗp�̏���
			imgNrAvg();	//	�ߖT����(�BdAvg, �CMIN)
			imgLwSub();	//	�����l����̍����l(�DdSub, �EdCal)
			//	��������؊m��
			pkTmp = new CPeak[PY][PX];
			memcpy(pkTmp, pkDepth, sizeof(CPeak)*PY*PX);	//	pkDepth -> pkTmp	
			//	���p���ް����폜
			for(int x=0; x<PX; x++){
				for(int y=0; y<PY; y++){
					pkTmp[y][x].dSub*=-1;	//	�̧�ݽ�ް��͋ɐ����]
				}
			}
			//	���k
			ret = LZ4_compress((char*)pkTmp, (char*)pkRef, sizeof(CPeak)*PX*PY);	//	pkDepth -> pkRef(���k)	
			if(ret<=0){
				m_strDet.Format("dat error.");
				MessageBox(m_strDet);
			}
			UpdateData(FALSE);
			//	̧�ٕۑ�
			f.Open("c:\\temp\\cvx\\reference.dat", CFile::modeCreate|CFile::modeWrite);
			f.Write(pkRef, ret);
			f.Close();
			//	�㏈��
			memcpy(pkRef, pkDepth, sizeof(CPeak)*PX*PY);	//	pkDepth -> pkRef	
			delete[]	pkTmp;
			m_strSts.Format("�␳�l Ready");
		}else{
			//	�]���p�̏���
			imgMeas();		//	�����v��				�GdSmp <- �EdCal, �DdSub	<< �EdCal <- �AdCrv, �BdAvg
			//imgIncl();	//	�X�Ζʕ␳�l(dInc)�Z�o	dInc <- dSmp	��.�ʉ摜������Ɉړ�
		}
		imgCalc();			//	�p�ȕ␳����	uFro <- uFrm(Calibration���ɂ͖��␳�̂܂�)
		imgSurface();		//	�ʉ摜����					nOrg <- dSmp, dInc

		//////////////////////////////////////////////////////////////////////////////
		m_strPathName.Format("%s", strFolderPath);
		UpdateData(FALSE);
	}
	imgChg();			//	Y-Z�f�ʐ���		uDsp <- uFro or uFrm
	imgSlc();			//	X-Y�f�ʐ���		uSfc <- nOrg or uFro or uFrm
	OnBnClickedAnlyz();	//	��������				use uSfc
	Invalidate(FALSE);

	m_strDet.Format("%.3f[ms]", GetPassTimeWindow()-dTm);
	UpdateData(FALSE);
	//MessageBox(str);}
}

//	��ʉ摜̧�ٵ����
void CCveDlg::OnBnClickedRef()
{
	bRef = true;	//	�B�ꂱ���ł���true
	OnBnClickedOpen();
	bRef = false;
}

//	�ʉ摜�ް�����ݏ���
void CCveDlg::imgSfcOpen(CString s)
{
	//	�\���ɔ��f
	m_strPathName = s;	//	̫��ް�߽��\��
	//	�摜۰��
	img.Load(s);
	pBmp = CBitmap::FromHandle(img);
	pBmp->GetBitmap(&bmp);
	pb = (uchar*)bmp.bmBits;
	memcpy(uSfc, pb, PX*PY*PC);
}

//	OCT�ް�̧�ٵ���ݏ���
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

	//	̧��۰��
	m_strPathName = s;	//	̫��ް�߽��\��
	if(f.Open(s, CFile::modeRead)){
		if(!bRaw){
			//	�𓀏������ēǍ���
			f.Read(uFrm, f.GetLength());
			ret = LZ4_decompress_safe((char*)uFrm, (char*)uFro, f.GetLength(), OCTSIZE);
			if(ret<=0){
				m_strDet.Format("error...");
				UpdateData(FALSE);
				f.Close();
				return;
			}
		}else{
			//	���ް��Ǎ���
			f.Read(uFro, OCTSIZE);
		}
		f.Close();
	}else{
		MessageBox("File Open Error.");
	}

	//	�\���p��؂ֺ�߰
#pragma omp parallel for
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			for(z=0; z<PZ; z++){
#ifdef Z_RVS	//	Z�����]����(��������)
				uFrm[y][PZ-z-1][x] = ((uchar(*)[PX][PZ])((void*)uFro))[y][x][z];	//	XY��](�p�Ȃ�������悤�ɂ��邽��)
#else
				uFrm[y][z][x] = ((uchar(*)[PX][PZ])((void*)uFro))[y][x][z];		//	XY��](�p�Ȃ�������悤�ɂ��邽��)
#endif // Z_RVS
			}
		}
	}
	memcpy(uFro, uFrm, OCTSIZE);
}

//	�S�ڰт̘p�ȕ␳����(���g�p)
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

//	Z���␳�摜����
void CCveDlg::imgCalc()
{
	int			x, y, z, c;
	int			tz[PY];

	memset(uFro, 0, PX*PY*PZ);		//	������
	for(x=0; x<PX; x++){			//	1024
		for(z=0; z<PZ; z++){		//	512
#pragma omp parallel for
			for(y=0; y<PY; y++){	//	1024
				//	������Z���␳�摜���쐬������
				tz[y] =  z - (int)pkDepth[y][x].dSub;	// + 512;	
				if(tz[y]<0)		tz[y] = 0;
				if(tz[y]>PZ)	tz[y] = PZ-1;
				uFro[x][tz[y]][y]  = uFrm[x][z][y];
			}
		}
	}
}

//	�p�ȕ␳����(���g�p)
void CCveDlg::imgRcv(uchar dst[][PY][PC], uchar src[][PY][PC], int x)
{
/*	int		y, z, c;
	int		tz, vz;

	memset(dst, 0, PS);
#pragma omp parallel for
	for(z=0; z<PZ; z++){
		for(y=0; y<PY; y++){
			//	������Z���␳�摜���쐬������
			tz =  z - (int)pkDepth[y][x].dSub;	// + 512;	
			if(tz<0)	tz = 0;
			if(tz>PZ)	tz = PZ-1;
			memcpy(&dst[tz][y][0], &src[z][y][0], 3);	//	3s�Z�k
		}
	}
	*/
}

//	�p�ȕ␳�摜�؂�ւ�(���g�p)
void CCveDlg::imgCurv()	
{
	/*
	int		x, y, z, c;
	UpdateData();
#pragma omp parallel for
	for(z=0; z<PZ; z++)
		for(y=0; y<PY; y++)
			memset(&uDsp[z][y][0], uFro[m_nSld][z][y], 3);
	bRcv = true;					//	�p�ȕ␳�����׸�	
	*/
}



//	�ύX�����(���g�p)
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

//	Update�����
void CCveDlg::OnEnUpdateEdit1()
{
}

//	OK
void CCveDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

//	�^���װ�����ޯ��
void CCveDlg::OnBnClickedCal()
{
	UpdateData();		//	�擾
	imgChg();
	imgSlc();
}

//	���摜��������
void CCveDlg::OnBnClickedMake()
{
	CWaitCursor	cur;
	imgMake();
}

//	���摜����
void CCveDlg::imgMake()
{
	/*
	//	cvx�p���߽
	CString	strPath, strName;
	int		n, nOS;

	strPath.Format("c:\\temp\\sdz\\cvcimage\\img_make\\");	//	̧�ِ�������̫���

	m_strPathName.Format("c:\\temp\\sdz\\cvcimage\\src.jpg");
	img.Load(m_strPathName);
	CBitmap::FromHandle(img)->GetBitmap(&bmp);	//	BITMAP�ƘA��
	pb = (uchar*)bmp.bmBits;					//	�摜�J�n���ڽ�擾
	memcpy(uSrc, pb, BMPSIZE);					//	�\���p�z��ɺ�߰
	for(n=0; n<PF; n++){
		//	���W��ĉ摜����
		nOS = 10*n;
		memcpy(pb, uSrc+nOS, BMPSIZE-nOS*PX*PC);
		memcpy(pb+BMPSIZE-nOS*PX*PC, uSrc, nOS*PX*PC);
		//	�摜�ۑ�
		strName.Format("%05d.jpg", n);
		img.Save(strPath+strName);
	}
	//pBmp->Detach();
	img.Destroy();
	*/
}

//	�ɍ��W�ϊ�
void CCveDlg::imgPol(uchar dst[][PD][PC], uchar src[][PX][PC])
{
/*	int		x, y, hx, hy, c, rad, rx, ry, core, px, py;
	double	dR, dR2, dRLmt, dCLmt, sita;

	core = m_nSldCore;			//	�����a
	memset(dst, 255, POLSIZE);	//	�w�i�𔒉�
	dRLmt = PD*PD/4;			//	�ő�O�a
	dCLmt = core*core;			//	�ŏ����a

	for(y=0; y<PD; y++){
		hy = y - PD/2+1;	//	���S���W�����Y�ʒu
		for(x=0; x<PD; x++){
			hx = x - PD/2+1;	//	���S���W�����X�ʒu
			dR2 = hx*hx + hy*hy;
			if((dR2>dRLmt)|(dR2<dCLmt))
				continue;

			dR = sqrt(dR2)-core;		//	�������������a
			sita = dSita[y][x]+PI;		//atan2((double)hy, (double)hx)+PI;

			rx = dR/(double)(PD/2-core)*(double)PX;		//	���摜��X���W
			if(rx<0)		rx = 0;
			else if(rx>=PX)	rx = PX-1;
			ry = sita/2.0/(double)PI*(double)PY;			//	���摜��Y���W
			if(ry<0)		ry = 0;
			else if(ry>=PY)	ry = PY-1;

			for(c=0; c<PC; c++){
				dst[y][x][c] = src[ry][rx][c];
			}
		}
	}
	//	�߰���`��
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

//	�߰����o����
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
	//	�߰����W���o(�@nOrg)
	for(y=0; y<PY; y++){
#pragma omp parallel for
		for(x=0; x<PX; x++){
			nMax[x] = 0;
			pkDepth[y][x].nOrg = 510;		//	100�ŊJ�n����(���ʂȒl�Ƃ���)
			for(z=0; z<PZ; z++){
				//uDt[x] = uFro[x][z][y];	//	uFro:�␳�摜->�␳�摜
				uDt[x] = uFrm[x][z][y];		//	uFrm:���␳�摜->�␳�摜
				if((nMax[x]<uDt[x])&&(50<uDt[x])&&(uDt[x]!=255)){
					//	�ő��50���傫��255�łȂ�
					nMax[x] = uDt[x];				//	�߰��l�ێ�
					pkDepth[y][x].nOrg = z;	//	�߰����W�ێ�
				}//	end if
			}//	end for z
		}//	end for x
	}//	end for y

	//	����̨�èݸޏ���(�AdCrv)
//#pragma omp parallel for
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			nPeak[y] = pkDepth[y][x].nOrg;
			//	�߰������o����Ȃ������ꍇ�Ͷ���̨�÷ݸނͽٰ
			if(nPeak[y] == 510){
				pkDepth[y][x].dCrv = 0.0;
				continue;
			}
			/**/
			if(	(nPeak[y]>FIT_NUM/2)&&			//	�J�n�_����
				(nPeak[y]<(PZ-FIT_NUM/2-1))){	//	��~�_����
					zs = nPeak[y] - FIT_NUM/2;	//	�ߖT���W�J�n�_
					ze = nPeak[y] + FIT_NUM/2;	//	�ߖT���W��~�_
					//	�ߖT�_���W
					int n = 0;
					for(z=zs; z<=ze; z++){
						//nFit[y][n] = uFro[x][z][y];
						nFit[y][n] = uFrm[x][z][y];
						n++;
					}
					pkDepth[y][x].dCrv = imgFitting(dDst[y], nFit[y], nPeak[y]);
					/*	
					pkDepth[y][x].a = dDst[y][0];
					pkDepth[y][x].b = dDst[y][1];
					pkDepth[y][x].c = dDst[y][2];
					memcpy(pkDepth[y][x].nFit, nFit[y], sizeof(int)*FIT_NUM);
					*/
			}else{
				//	�߰��Ȃ�
				pkDepth[y][x].dCrv = 0.0;
			}//	end if
		}//	end for x
	}//	end for y
}

//	�ߖT���ϒl�Z�o
//	��ʂ�z�肵�Ă���̂ňُ�l�r���͍l�����Ă��Ȃ�
void CCveDlg::imgNrAvg()
{
	int		x, y, c;
	int		tx, ty;
	double	dSum;
	double	diff;
	double	data;
	double	dAvg, dVar, dSd;
	int		cnt;

#if 0
	//	�ߖT���ϒl�Z�o(�BdAvg)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if((x<=AVG_FRM)||(x>(PX-AVG_FRM-1))||(y<=AVG_FRM)||(y>(PY-AVG_FRM-1))){
				//	�ڰѴر�͎��l
				pkDepth[y][x].dAvg = pkDepth[y][x].dCrv;
			}else{
				//	�ߖT���ϒl
				dSum = 0.0;
				for(ty=y-AVG_FRM; ty<=y+AVG_FRM; ty++){
					for(tx=x-AVG_FRM; tx<=x+AVG_FRM; tx++){
						dSum += pkDepth[y][x].dCrv;
					}
				}
				pkDepth[y][x].dAvg = dSum/(AVG_FRM*2+1)/(AVG_FRM*2+1);	//	�ߖT���ϒl
			}
		}
	}
#else
	//	�W���΍��Z�o
	dSum = 0.0;
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			dSum += pkDepth[y][x].dCrv;
		}
	}
	dAvg = dSum/(PY*PX);		//	�S�̂̕��ϒl
	dVar = 0;	
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			diff = pkDepth[y][x].dCrv - dAvg;
			dVar += diff*diff;
		}
	}
	dSd = 100;					//	�l���������ĕ␳�ł��Ȃ��̈悪������΍�
	//dSd = sqrt(dVar/(PY*PX));	//	�W���΍�(�ԈႢ�������p�ケ�̒l���g�p, dSd��3)
	//dSd = sqrt(dVar)/(PY*PX);	//	�W���΍�

	//	�ߖT���ϒl�Z�o(�BdAvg)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
#ifdef REF_AVG
			//	�֊s�ް����敪
			if((x<=AVG_FRM)||(x>(PX-AVG_FRM-1))||(y<=AVG_FRM)||(y>(PY-AVG_FRM-1))){
				//	�ڰѴر�͎��l
				pkDepth[y][x].dAvg = pkDepth[y][x].dCrv;
			}else{
				//	�ߖT���ϒl
				dSum = 0.0;
				cnt = 0;
				//	��`�ر�ŕ��ω�
				for(ty=y-AVG_FRM; ty<=y+AVG_FRM; ty++){
					for(tx=x-AVG_FRM; tx<=x+AVG_FRM; tx++){
						//dSum += pkDepth[y][x].dCrv;
						//	�ُ�l���߽
						data = pkDepth[ty][tx].dCrv;
						if(data < dAvg-(2*dSd))	// 2015.05.28�ύX
						{
							continue;
						}
						if(dAvg+(2*dSd) < data)	// 2015.05.28�ύX
						{
							continue;
						}
						//	����l�ώZ
						dSum += data;
						++cnt;
					}
				}
				//	����l�ŕ��ϒl�Z�o
				if(cnt){
					pkDepth[y][x].dAvg = dSum/cnt;	//	��`�ر�̕��ϒl
				}else{	
					pkDepth[y][x].dAvg = dAvg;		//	�S�Ĉُ�l�̏ꍇ�͑S�̂̕��ϒl����
				}
				//pkDepth[y][x].dAvg = dSum/(AVG_FRM*2+1)/(AVG_FRM*2+1);	//	�ߖT���ϒl
			}
#else
			//	�ʂ̒��������グ�邽�߂ɋߖT���Ϗ��������Ȃ�(������̧�ݽ�̉e�������ڏo��)
			pkDepth[y][x].dAvg = pkDepth[y][x].dCrv;
#endif // REF_AVG
		}//	end for x
	}//	end for y
#endif
}

//	�����l����̍����l
void CCveDlg::imgLwSub()
{
	int		x, y, c;
	int		tx, ty;
	double	dMin;
	CString	s;

	//	MIN�T��(�CMIN)
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
	//	�ŏ��l����̍����l�Z�o(�DdAvg, �EdCal)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			pkDepth[y][x].dSub = pkDepth[y][x].dAvg - dMin;			//	�����l����̍����l
			//
			//pkDepth[y][x].dSub *= -1;
			//
			//pkDepth[y][x].dCal = pkDepth[y][x].dSmp = 
			//			pkDepth[y][x].dCrv - pkDepth[y][x].dSub;	//	Test:�����l����(���l-�ߖT���ϒl)
			//	���p���ް��͍폜
			pkDepth[y][x].dAvg = pkDepth[y][x].dCal = pkDepth[y][x].dSmp = pkDepth[y][x].dInc = 0.0;
		}
	}
	///////////////////////////////////////////////////////////////////////////////
	//	����̨�èݸނ������W�o��(x=PX/2��ײ�)
	/*
	CFile	f;
	//CString	s;
	f.Open("c:\\temp\\cvx\\peak.csv", CFile::modeCreate|CFile::modeWrite);
	s.Format("y, dCrv, dAvg, dCal");
	f.Write(s, s.GetLength());
	for(y=0; y<PY; y++){
		s.Format("%d,%.3f,%.3f,%.3f", y, pkDepth[y][PX/2].dCrv, pkDepth[y][PX/2].dAvg, pkDepth[y][PX/2].dCal);
		//s.Format("%d,%.3f,%.3f", y, pkDepth[y][PX/2].dSub, pkDepth[y][PX/2].dCal);
		
		//for(int m=0; m<FIT_NUM; m++){
		//	s.Format("%s,%d", s, pkDepth[y][PX/2].nFit[m]);	//	���Ӎ��W�ް�
		//}
		s.Format("%s\n", s);
		f.Write(s, s.GetLength());
	}
	f.Close();
	/**/
	///////////////////////////////////////////////////////////////////////////////
}

//	�����v������
//	dCrv, dSub -> dCal
//	dCal, dAvg -> dSmp
void CCveDlg::imgMeas()
{
	int		x, y, c;
	int		num;
	double	dAvg;

	//	�ʕ␳�l�Čv�Z(�EdCal)
	dAvg = 0.0;
	num = 0;
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(pkDepth[y][x].dCrv != 0){
				//	����̨�èݸނŒl����ꂽ�ꍇ�ɕ␳�l�Z�o
				if(m_bCurv)		pkDepth[y][x].dCal = pkDepth[y][x].dCrv - pkDepth[y][x].dSub;	//	�p�ȕ␳����
				else			pkDepth[y][x].dCal = pkDepth[y][x].dCrv;						//	�p�ȕ␳�Ȃ�
				dAvg += pkDepth[y][x].dCal;
				num++;
			}else{
				//	����̨�èݸނŒl�����Ă��Ȃ��ꍇ�͔r��
				pkDepth[y][x].dCal = 1000.0;		//	�r�����ׂ����ʂȒl
			}//	end if
		}//	end for x
	}//	end for y
	dAvg /= (double)num;	//	���ύ����Z�o(�FAVG)
	//	����0�̍���ϯ��(�GdSmp)
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(pkDepth[y][x].dCrv != 0){
				//	����̨�èݸނŒl����ꂽ�ꍇ�ɍ����l�Z�o
				pkDepth[y][x].dSmp = pkDepth[y][x].dCal - dAvg;
			}else{
				//	����̨�èݸނŒl�����Ă��Ȃ��ꍇ�͔r��
				pkDepth[y][x].dSmp = 1000.0;
			}
		}//	end for x
	}//	end for y
	Invalidate(FALSE);

	///////////////////////////////////////////////////////////////////////////////
	//	����̨�èݸނ������W�o��(x=PX/2��ײ�)
	/*
	CFile	f;
	CString	s;
	f.Open("c:\\temp\\cvx\\peak.csv", CFile::modeCreate|CFile::modeWrite);
	for(y=0; y<PY; y++){
		//s.Format("%d,%.3f,%.3f", y, pkDepth[y][PX/2].dCrv, pkDepth[y][PX/2].dAvg);
		s.Format("%d,%.3f,%.3f,%.3f,%.3f", y, pkDepth[y][PX/2].dSmp, pkDepth[y][PX/2].dCal, pkDepth[y][PX/2].dSub, pkDepth[y][PX/2].dAvg);
		//for(int m=0; m<FIT_NUM; m++){
		//	s.Format("%s,%d", s, pkDepth[y][PX/2].nFit[m]);	//	���Ӎ��W�ް�
		//}
		s.Format("%s\n", s);
		f.Write(s, s.GetLength());
	}
	f.Close();
	/**/
	///////////////////////////////////////////////////////////////////////////////
}

//	�ʉ摜����
//	pixel->um�֕ϊ�����nOrg�֍ēo�^
//	dSmp -> nOrg
void CCveDlg::imgSurface()
{
	int		x, y, c;
	double	dRange = nViewRange;	//	�}5um or �}10um or �}20um	
	int		nDt;

	//	�ʉ摜
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(m_bIncl){
				//	�X�Ζʕ␳����
				nDt = (int)(128.0*UM_RATIO*(pkDepth[y][x].dSmp-pkDepth[y][x].dInc)/dRange+128.0);
			}else{
				//	�X�Ζʕ␳�Ȃ�
				nDt = (int)(128.0*UM_RATIO*pkDepth[y][x].dSmp/dRange+128.0);	//	um�ɕϊ�����[�}dRange]��8bit�ɐ��K��
			}
			//nDt = 128;//(int)(128.0*(x/10)/dRange+128.0);
			if(nDt>255)	nDt = 255;	//	255;
			if(nDt<=0)	nDt = 255;	//	�w�i�̔��_����
			pkDepth[y][x].nOrg = nDt;
		}
	}
}

//	�������]�����ޯ��
void CCveDlg::OnBnClickedInv()
{
	UpdateData();
	Invalidate();
}

//	�p�ȕ␳�����ޯ��
void CCveDlg::OnBnClickedCurv()
{
	UpdateData();
	imgMeas();		//	�p�ȕ␳
	imgUpdate();
}

//	�߰����o�����ޯ��
void CCveDlg::OnBnClickedPeak()
{
	UpdateData();
	imgSlc();		//	X-Y�f�ʐ���
	CWnd*	pw;
	pw = GetDlgItem(IDC_RANGE);
	pw->EnableWindow(m_bPeak);
}


//	�O�p�֐�ð��ٍ쐬
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

//	�C��X�ʒu��Y-Z�f�ʉ摜����
//	uDsp�̐���
void CCveDlg::imgChg()
{
	int		x, y, z, c;
	UpdateData();
	//m_strPathName.Format("Frame:04d/%04d", m_nEdit, PF-1);
	UpdateData(FALSE);

	//	�\���F���I��
	//	�^���װ�\��
#pragma omp parallel for
	for(y=0; y<PY; y++){
		for(z=0; z<PZ; z++){
			if(m_bCal){
				//	�^���װ�\��
				if(m_bCurv)	memcpy(&uDsp[z][y][0], &uClr[uFro[m_nSld][z][y]][0], 3);	//	�p�ȕ␳�摜
				else		memcpy(&uDsp[z][y][0], &uClr[uFrm[m_nSld][z][y]][0], 3);	//	���␳�摜
			}else{
				//	��ڰ���ٕ\��
				if(m_bCurv)	memset(&uDsp[z][y][0], uFro[m_nSld][z][y], 3);	//	�p�ȕ␳�摜
				else		memset(&uDsp[z][y][0], uFrm[m_nSld][z][y], 3);	//	���␳�摜
			}
		}
	}
	Invalidate(FALSE);
}

//	�C��Z�ɂ��X-Y�f�ʉ摜����
//	uSfc�̐���
void CCveDlg::imgSlc()
{
	int	x, y, c;
	UpdateData();

#pragma omp parallel for
	for(x=0; x<PX; x++){
		for(y=0; y<PY ;y++){
			//	Peak�\�ʕ\��
			if(m_bPeak){
				//	�\�e���摜�͐F�����]
				//if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[255-(uchar)pkDepth[y][x].nOrg][0], 3);	//	�^���װ
				//else		memset(&uSfc[x][y][0], (255-(uchar)pkDepth[y][x].nOrg), 3);			//	�ɸ�
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[255-(uchar)pkDepth[y][(PX-1)-x].nOrg][0], 3);	//	�^���װ
				else		memset(&uSfc[x][y][0], (255-(uchar)pkDepth[y][(PX-1)-x].nOrg), 3);			//	�ɸ�
			}else if(m_bCurv){
				//	�p�ȕ␳��ײ��摜
				//if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFro[x][m_nSldCore][y]][0], 3);		//	�^���װ
				//else		memset(&uSfc[x][y][0], uFro[x][m_nSldCore][y], 3);						//	�ɸ�
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFro[(PX-1)-x][m_nSldCore][y]][0], 3);		//	Z���]	�^���װ
				else		memset(&uSfc[x][y][0], uFro[(PX-1)-x][m_nSldCore][y], 3);				//			�ɸ�
			}else{
				//	�p�ȕ␳�O�ײ��摜
				//if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFrm[x][m_nSldCore][y]][0], 3);		//	�^���װ
				//else		memset(&uSfc[x][y][0], uFrm[x][m_nSldCore][y], 3);					//	�ɸ�
				if(m_bCal)	memcpy(&uSfc[x][y][0], &uClr[uFrm[(PX-1)-x][m_nSldCore][y]][0], 3);	//	�^���װ
				else		memset(&uSfc[x][y][0], uFrm[(PX-1)-x][m_nSldCore][y], 3);			//	�ɸ�
			}
		}
	}
	Invalidate(FALSE);
}

//	ϳ��ړ�
void CCveDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// �����݂�������Ă�����
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

//	�����ݸد�
void CCveDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
}

//	�ײ�ް1�̏���
void CCveDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData();
	if(m_nEdit != m_nSld){
		m_nEdit = m_nSld;
		UpdateData(FALSE);
		imgChg();			//	X-Z�f�ʐ���
	}
	*pResult = 0;
}

//	�ײ�ް2�̏���
void CCveDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	static	int	n;
	UpdateData();
	if(m_nSldCore != n){
		n = m_nSldCore;
		m_strCore.Format("%d", m_nSldCore);
		UpdateData(FALSE);
		imgSlc();			//	X-Y�f�ʐ���
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

//	���ύ��W�擾
void CCveDlg::ptAvg(double dst[], CPoint p)
{
	int		x, y, z, sx, sy, sn;
	int		i, j, k;
	double	dSum, dAvg, dStd;
	double	dP[3];

	sn = 0;
	dSum = dAvg = 0.0;
	//	�ʏ�̕��ϒl�Z�o
	for(y=p.y-5; y<p.y+5; y++){
		for(x=p.x-5; x<p.x+5; x++){
			//	�ُ�l����������ǉ�

			//	�݌v
			dSum += pkDepth[y][x].dSmp;
			sn++;
		}
	}
	dAvg = dSum/(double)sn;	//	����
	dSum = dStd = 0.0;
	//	�W���΍��Z�o
	for(y=p.y-5; y<p.y+5; y++){
		for(x=p.x-5; x<p.x+5; x++){
			dSum += ((dAvg - pkDepth[y][x].dSmp)*(dAvg - pkDepth[y][x].dSmp));
		}
	}
	dStd = sqrt(dSum/(double)sn);	//	�W���΍�
	//	�W���΍����ɂ�镽�ύ��W�����Z�o
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
	dst[0] = (double)sx/(double)sn;	//	X���W���ϒl
	dst[1] = (double)sy/(double)sn;	//	Y���W���ϒl
	dst[2] = dSum/(double)sn;		//	Z���W���ϒl
	dst = dP;
}

//	�X�Ζʕ␳�l�Z�o
//	dInc <- dSmp
void CCveDlg::imgIncl()
{
	CWaitCursor	cur;
	int		i, j, k, x, y;
	int		n = 3;		//	�s��̎���
	double	dZ[3] = {0};//	dZ���W
	double	dP[3] = {0};//	���Ұ�{a, b, c}
	double	dQ[3] = {0};
	double	buf;
	int		nS[3];		//	�ʕ␳�Ɏg�p������WNo.(0-999)

	//	�X�Ε␳�p���W�̏�����
	ptD[0] = CPoint(Y1, X1);
	ptD[1] = CPoint(Y2, X2);
	ptD[2] = CPoint(Y3, X3);

	//	�X�Ε␳�p���WNo.������
	nS[0] = P1;
	nS[1] = P2;
	nS[2] = P3;

	//	�X�Ε␳���W���I������
	for(i=0; i<n; i++){
		if(bAnlyz){
			//	���͍ς�
			do{
				//	�X�Ε␳�p���W�ݒ�
				//ptD[i] = CPoint((crIn.ptCirc[nS[i]].y+crOut.ptCirc[nS[i]].y)/2, PY-(crIn.ptCirc[nS[i]].x+crOut.ptCirc[nS[i]].x)/2);
				ptD[i] = CPoint(PX-1-(crIn.ptCirc[nS[i]].x+crOut.ptCirc[nS[i]].x)/2, (crIn.ptCirc[nS[i]].y+crOut.ptCirc[nS[i]].y)/2);
				//	�e���WNo.��Z�l��]������NG�Ȃ���WNo.��ύX����
				dZ[i] = pkDepth[ptD[i].y][ptD[i].x].dSmp;
				if(fabs(dZ[i])>10.0)
					nS[i] += 10;
			}while(fabs(dZ[i])>10.0);
		}else{
			//	������
			//	�e���WNo.��Z�l��]������NG�Ȃ���WNo.��ύX����
			dZ[i] = pkDepth[ptD[i].y][ptD[i].x].dSmp;
		}
	}

	//	���ް��̑��
	double	dT[3][3] = {0};
	double	dA[3][3] = {{ptD[0].x, ptD[0].y, 1},
						{ptD[1].x, ptD[1].y, 1},
						{ptD[2].x, ptD[2].y, 1}};

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
	//�P�ʍs������
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			dT[i][j] = (i==j) ? 1.0 : 0.0;
		}
	}
	
	//	�t�s�񉉎Z(�|���o���@)
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

	//	���Ұ��t�Z
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			dP[i] += dT[i][j] * dZ[j];
		}
	}
	////////////////////////////////////////////////////////////
	CString	s;
	s.Format("a=%.3f, b=%.3f, c=%.3f", dP[0], dP[1], dP[2]);
	//MessageBox(s);
	////////////////////////////////////////////////////////////
	
	//	�X�Ζʕ␳�l�Z�o
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			pkDepth[y][x].dInc = (dP[0]*x + dP[1]*y + dP[2]);
			//pkDepth[y][x].dSmp -= (dP[0]*x + dP[1]*y + dP[2]);
		}
	}
}


//	�X�Ζʕ␳�l�Z�o
//	dInc <- dSmp
void CCveDlg::imgIncl2()
{
	CWaitCursor	cur;
	int		i, j, k, x, y;
	int		n = 4;		//	�s��̎���
	double	dZ[4] = {0};//	dZ���W
	double	dP[4] = {0};//	���Ұ�{a, b, c}
	double	dQ[3] = {0};
	double	buf;

	//	�X�Ε␳���W������
	if(bAnlyz){
		ptD[0] = CPoint((crIn.ptCirc[000].x+crOut.ptCirc[000].x)/2, (crIn.ptCirc[000].y+crOut.ptCirc[000].y)/2);
		ptD[1] = CPoint((crIn.ptCirc[250].x+crOut.ptCirc[250].x)/2, (crIn.ptCirc[250].y+crOut.ptCirc[250].y)/2);
		ptD[2] = CPoint((crIn.ptCirc[500].x+crOut.ptCirc[500].x)/2, (crIn.ptCirc[500].y+crOut.ptCirc[500].y)/2);
		ptD[3] = CPoint((crIn.ptCirc[750].x+crOut.ptCirc[750].x)/2, (crIn.ptCirc[750].y+crOut.ptCirc[750].y)/2);
	}else{
		ptD[0] = CPoint(X1, Y1);
		ptD[1] = CPoint(X2, Y2);
		ptD[2] = CPoint(X3, Y3);
	}

	//	���ް��̑��
	double	dT[4][3] = {0};
	double	dA[4][3] = {{ptD[0].x, ptD[0].y, 1},
						{ptD[1].x, ptD[1].y, 1},
						{ptD[2].x, ptD[2].y, 1},
						{ptD[3].x, ptD[3].y, 1}};

	for(i=0; i<n; i++){
		dZ[i] = pkDepth[ptD[i].y][ptD[i].x].dSmp;
	}
	/**/
	
	for(n=0; n<4; n++){
		ptAvg(dQ, ptD[n]);
		//	XY
		dA[n][0] = dQ[0];	//	X
		dA[n][1] = dQ[1];	//	Y
		dA[n][2] = 1;
		dZ[n] = dQ[2];		//	Z
	}
	/**/
	//�P�ʍs������
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			dT[i][j] = (i==j) ? 1.0 : 0.0;
		}
	}
	
	//	�t�s�񉉎Z(�|���o���@)
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

	//	���Ұ��t�Z
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			dP[i] += dT[i][j] * dZ[j];
		}
	}
	////////////////////////////////////////////////////////////
	CString	s;
	s.Format("a=%.3f, b=%.3f, c=%.3f", dP[0], dP[1], dP[2]);
	MessageBox(s);
	////////////////////////////////////////////////////////////
	
	//	�X�Ζʕ␳�l�Z�o
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			pkDepth[y][x].dInc = (dP[0]*x + dP[1]*y + dP[2]);
			//pkDepth[y][x].dSmp -= (dP[0]*x + dP[1]*y + dP[2]);
		}
	}
}

//	����̨�èݸޏ���
//	ex.	y = ax^2 + bx + c
//	usage	o	dDst	�v�Z����(a,b,c,d)	
//			i	nSrc	�߰��𒆐S��7�ް�
//			i	nPk		�߰����W
//			ret	dPk		�߰��ʒu�v�Z�l
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
		//	�߰��_�v�Z
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

//	ϳ������ݱ���
void CCveDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_Circ.SendMessage(WM_LBUTTONUP);
	CDialogEx::OnLButtonUp(nFlags, point);
}

//	�摜�ۑ�
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
					//0, 0, PZ, PZ,				//	�o�͍��W
					0, 0, PX, PY,				//	�o�͍��W
					0, 0, PX, PY,				//	���摜�̍��W
					uSfc, &bp,			//	�摜�f�[�^
					DIB_RGB_COLORS,	SRCCOPY);

	img.Attach((HBITMAP)bmpForSave);

	// [���O��t���ĕۑ�]�_�C�A���O
#ifdef BMP_SAVE
	//	Bitmap�ŕۑ�
	CFileDialog	dlg(FALSE, ".bmp", "View", OFN_OVERWRITEPROMPT,
							"�摜�t�@�C�� (*.bmp)|*.bmp|�S�Ẵt�@�C��(*.*)|*.*||");
#else
	//	jpg�ŕۑ�
	CFileDialog	dlg(FALSE, ".jpg", "View", OFN_OVERWRITEPROMPT,
							"�摜�t�@�C�� (*.jpg)|*.jpg|�S�Ẵt�@�C��(*.*)|*.*||");
#endif // BMP_SAVE

	if(dlg.DoModal() == IDOK){
		CString strMsg;
		strMsg.Format(("�t�@�C�����F%s\n�p�X�F%s"),  dlg.GetFileName(), dlg.GetPathName());
		//MessageBox(strMsg);
		img.Save(dlg.GetPathName());
	}
}

//	�ݼޑI��
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

//	�װ�ް�쐬
void CCveDlg::makeBar()
{
	int		z, c;
	uchar	uCol;

	for(z=0; z<PZ; z++){
		memcpy(&uBar[0][z][0][0], &uClr[(z/2)][0], 3);	//	�װ
		memset(&uBar[1][z][0][0], (uchar)(z/2), 3);		//	�ɸ�
	}
}

//	���k�ϊ�����
void CCveDlg::OnBnClickedLz4()
{
	CWaitCursor	cur;
	int			n;
	int			ret;
	int			size;
	CString		s;
	double		dTm;
	CFile		f;

	CFileDialog	dlg(TRUE, "*.raw", NULL, OFN_ALLOWMULTISELECT|OFN_EXPLORER, "raw�ް�̧�� (*.raw)|*.raw");
	if(dlg.DoModal() == IDOK){
		bImg = false;
		//	̧��ذ��
		strFolderPath = dlg.GetPathName();
		f.Open(strFolderPath, CFile::modeRead);
		f.Read(uFro, OCTSIZE);
		f.Close();
		//	LZ4���k
		dTm = GetPassTimeWindow();
		ret = LZ4_compress((char*)uFro, (char*)uFrm, OCTSIZE);
		if(ret<=0){
			m_strDet.Format("error, %.1f[ms]", GetPassTimeWindow() - dTm);
		}else{
			m_strDet.Format("%d[Mbyte], %.1f[ms]", ret/1024/1024, GetPassTimeWindow() - dTm);	//	21MB
		}
		UpdateData(FALSE);
		//	̧�ٕۑ�
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

//	�X�Ζʕ␳�����ޯ��
void CCveDlg::OnBnClickedIncl()
{
	UpdateData();
	if(m_bIncl)
		imgIncl();
		//imgIncl2();
	imgUpdate();
}

//	�`��X�V
void CCveDlg::imgUpdate()
{
	imgSurface();	//	�e���摜�ް�����
	imgSlc();		//	X-Y�f�ʐ���
	imgChg();		//	Y-Z�f�ʐ���
}

//	��͏���(�����ʒ��o)
void CCveDlg::OnBnClickedAnlyz()
{
	crIn.reset();
	crOut.reset();
	ftSfc.reset();
	imgTrim();
	imgFlat();
	csvOut();
	Invalidate();
	bAnlyz = true;	//	���͊���(�����ł���true)
}

//	�ʕ]��
void CCveDlg::imgFlat()
{
	int		x, y, c, n;
	double	dSum;

	//	���ω�f�l�Z�o
	n = 0;
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(uBin[x][y] != 0){
				//	ܰ��ر
				ftSfc.dAvg += uSfc[x][y][RED];	//	��f�l�ώZ
				n++;
			}
		}

	}
	ftSfc.dAvg /= (double)n;	//	���ω�f�l
	//	���ϕ��U�l, Ry, Ra�Z�o
	ftSfc.dDist = 0.0;
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			if(uBin[x][y] != 0){
				dSum = (uSfc[x][y][RED]-ftSfc.dAvg);	//	�΍�
				ftSfc.dRa += fabs(dSum);				//	�ݐϕ΍�				
				ftSfc.dDist += (dSum*dSum);				//	�ݐϕ��U�l
				if(dSum>ftSfc.dMax)
					ftSfc.dMax = dSum;
				else if((dSum<ftSfc.dMin)&&(uSfc[x][y][RED]!=0))
					ftSfc.dMin = dSum;
			}
		}
	}
	ftSfc.dRa /= (double)n;				//	Ra
	ftSfc.dRy = ftSfc.dMax-ftSfc.dMin;	//	Ry(PV)
	ftSfc.dDist /= (double)n;			//	���ϕ��U
}

//	�~���]��
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
				nSumN++;	//	�L����f��
				nSumX += x;	//	X���W�݌v
				nSumY += y;	//	Y���W�݌v
			}
		}
	}

	//	�d�S�Z�o
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
#ifdef TH_COL
					uSfc[(int)dX][(int)dY][col] = 255;
#endif
					break;
				}
			}
		}
		crIn.ptCirc[nCnt] = CPoint((int)dX, (int)dY);	//	�֊s�_���W�Q

		//	Outline
		for(dLen=PX/2; dLen>0; dLen-=0.1){
			dX = dLen * cos(dRad) + crOut.ptCg.x;
			dY = dLen * sin(dRad) + crOut.ptCg.y;
			if((dX>0)&&(dX<PX)&&(dY>0)&&(dY<PY)){
				if(uSfc[(int)dX][(int)dY][col] > TH_OTL){
#ifdef TH_COL
					uSfc[(int)dX][(int)dY][col] = 255;
#endif // TH_COL
					break;
				}
			}
		}
		crOut.ptCirc[nCnt] = CPoint((int)dX, (int)dY);	//	�֊s�_���W�Q
		nCnt++;		//	�����
	}
	

	//	�֊s�ް�����(dDist, dRy, dRa)
	crIn.calc();
	crOut.calc();

	//	ܰ����o
	memset(uBin, 0, PX*PY);	
	for(y=0; y<PY; y++){
		for(x=0; x<PX; x++){
			dRn = sqrt((double)((x-crIn.ptCg.x)*(x-crIn.ptCg.x)+(y-crIn.ptCg.y)*(y-crIn.ptCg.y)));
			if((dRn>crIn.dR)&&(dRn<crOut.dR)){
#ifdef TH_COL
				uSfc[x][y][BLUE] = 255;		//	test
#endif
				//	ܰ��\��
				uBin[x][y] = 255;

			}
		}

	}
}

//	���Ұ��o��
void CCveDlg::csvOut()
{
	CFile	f;
	CString	str;

	f.Open("c:\\temp\\cvx\\mt_prm.csv", CFile::modeWrite|CFile::modeCreate);
	str.Format("File name,SAvg,SDist,SRa,SRy,SMax,SMin,IR,IDist,IRa,IRy,OR,ODist,ORa,ORy\n");
	f.Write(str, str.GetLength());
	str.Format("%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", 
		strSfcImg,
		ftSfc.dAvg,ftSfc.dDist, ftSfc.dRa,ftSfc.dRy,
		ftSfc.dMax,ftSfc.dMin,
		crIn.dR,crIn.dDist, crIn.dRa, crIn.dRy,
		crOut.dR,crOut.dDist, crOut.dRa, crOut.dRy);
	f.Write(str, str.GetLength());
	f.Close();
}

//	�ʉ摜(jpg)�����
void CCveDlg::OnBnClickedImgOpen()
{
	CWaitCursor	cur;
	CString		strFileName, strFilePath;

	CFileDialog	dlg(TRUE, "*.jpg", NULL, OFN_ALLOWMULTISELECT|OFN_EXPLORER, "jpg�ް�̧�� (*.jpg)|*.jpg");

	if(dlg.DoModal() == IDOK){
		bAnlyz = false;
		strSfcImg = dlg.GetFileName();
		imgSfcOpen(dlg.GetPathName());
		//	�����ʒ��o����
		OnBnClickedAnlyz();
	}
}


