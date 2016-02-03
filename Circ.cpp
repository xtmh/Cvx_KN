//	CCirc.cpp : 実装ファイル

#include "StdAfx.h"
#include "Cve.h"
#include "CveDlg.h"
#include "Circ.h"
#include "param.h"

//	円筒画像描画クラス
IMPLEMENT_DYNAMIC(CCirc, CStatic)

CCirc::CCirc(void)
{
	pcbRed = new CBrush(COLORREF(RGB(255,0,0)));
	pcbPpl = new CBrush(COLORREF(RGB(255,0,255)));
	pcbGrn = new CBrush(COLORREF(RGB(0,255,0)));
	pcpX = new CPen(PS_SOLID, 1, COLORREF(RGB(0,0,0)));
	pcpG1 = new CPen(PS_SOLID, 1, COLORREF(RGB(150,150,150)));
	pcpG2 = new CPen(PS_SOLID, 1, COLORREF(RGB(200,200,200)));
	pcpB = new CPen(PS_SOLID, 1, COLORREF(RGB(0,0,125)));
	pcpW = new CPen(PS_SOLID, 1, COLORREF(RGB(128,128,255)));
	pcpR = new CPen(PS_DOT, 1, COLORREF(RGB(255,0,0)));
	pcpR2 = new CPen(PS_DOT, 2, COLORREF(RGB(255,0,0)));
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


CCirc::~CCirc(void)
{
	delete	pcbRed;
	delete	pcbPpl;
	delete	pcbGrn;
	delete	pcpX;
	delete	pcpG1;
	delete	pcpG2;
	delete	pcpR;
	delete	pcpR2;
	delete	pcpG;
	delete	pcpB;
	delete	pcpW;
	delete	pcpP;
	delete	pcpDG;
	delete	pcpD;
}
BEGIN_MESSAGE_MAP(CCirc, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CCirc::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	int	nPos;
	//	
	dc.SetBkMode(TRANSPARENT);
	//dc.SetStretchBltMode(COLORONCOLOR);
	dc.SetStretchBltMode(HALFTONE);
	dc.SetTextColor(COLORREF(RGB(255,255,255)));	//	背景：白
	CCveDlg* d = (CCveDlg*)pv;
	if(!d->bImg)
		return;		//	表示用画像の準備ができていないので即抜ける

	pOldPen = dc.SelectObject(pcpR2);
	if(d->bOrg){
		//	ｵﾘｼﾞﾅﾙｻｲｽﾞ
		StretchDIBits(	dc.m_hDC,
						0, 0, PZ, PZ,				//	出力座標
						d->ptPos.x/EXPND, d->ptPos.y/EXPND, 
						PX/EXPND, PY/EXPND,			//	元画像の座標
						d->uSfc, &d->bp,			//	画像データ
						DIB_RGB_COLORS,	SRCCOPY);
	}else{
		//	全体表示
		StretchDIBits(	dc.m_hDC,
						0, 0, PZ, PZ,				//	出力座標
						0, 0, PX, PY,				//	元画像の座標
						d->uSfc, &d->bp,			//	画像データ
						DIB_RGB_COLORS,	SRCCOPY);
		if(d->m_bIncl){
			for(int p=0; p<3; p++){
				dc.Rectangle(	(d->ptD[p].x-5)/EXPND, (d->ptD[p].y-5)/EXPND, 
								(d->ptD[p].x+5)/EXPND, (d->ptD[p].y+5)/EXPND);
			}
		}
	}

	//	ｽﾗｲｽﾗｲﾝ描画
	dc.SelectObject(pcpR);
	nPos = d->m_nSld/EXPND;
	dc.MoveTo(0, nPos);
	dc.LineTo(0+PX/EXPND, nPos);
	//	
#ifdef	Z_RVS
	d->m_strXY.Format("Click:(%d, %d)\nMove:(%d, %d)\n → H：%+.2f[um]", 
						d->ptPos.x, d->ptPos.y,
						d->ptMov.x, d->ptMov.y,
						//(-1*UM_RATIO*d->pkDepth[d->ptMov.y][d->ptMov.x].dSmp/(double)d->nViewRange));
						-(UM_RATIO*d->pkDepth[d->ptMov.x][d->ptMov.y].dSmp));
#else
	d->m_strXY.Format("Click:(%d, %d)\nMove:(%d, %d)\n → H：%+.2f[um]", 
						d->ptPos.x, d->ptPos.y,
						d->ptMov.x, d->ptMov.y,
						//(-1*UM_RATIO*d->pkDepth[d->ptMov.y][d->ptMov.x].dSmp/(double)d->nViewRange));
						(UM_RATIO*d->pkDepth[d->ptMov.x][d->ptMov.y].dSmp));
#endif
	dc.SelectObject(pOldPen);
	if(d->m_bPeak){
		CString	s;
		dc.SetTextColor(RGB(255, 255, 255)); //	pink
		pOldFont = (CFont*)dc.SelectObject(&Font2);
		s.Format("+%dum", d->nViewRange);
		dc.TextOut(5, 5, s);
		s.Format("+%dum", d->nViewRange);
		s.Format("  0um");
		dc.TextOut(5, 250, s);
		s.Format("-%dum", d->nViewRange);
		dc.TextOut(5, 498, s);
	}
	d->UpdateData(FALSE);
}


void CCirc::PreSubclassWindow()
{
	GetClientRect(&rc);

	CStatic::PreSubclassWindow();
}

//	ﾏｳｽﾑｰﾌﾞ
void CCirc::OnMouseMove(UINT nFlags, CPoint point)
{
	CCveDlg* d = (CCveDlg*)pv;
	switch(nFlags){
	//	左ﾎﾞﾀﾝが押された状態(ｽﾗｲｽﾚﾍﾞﾙ決定)
	case	MK_RBUTTON:
		d->m_nEdit = d->m_nSld = point.y*2;
		d->UpdateData(FALSE);
		d->imgChg();
		Invalidate(FALSE);
		break;
	case	MK_LBUTTON:
		d->ptMov.x = d->ptPos.x/2 + point.x;
		d->ptMov.y = d->ptPos.y/2 + (PY/EXPND-point.y-1);
		Invalidate(FALSE);
		break;
	}
	CStatic::OnMouseMove(nFlags, point);
}

//	左ﾎﾞﾀﾝﾀﾞｳﾝ
void CCirc::OnLButtonDown(UINT nFlags, CPoint point)
{
	CCveDlg* d = (CCveDlg*)pv;
	d->bOrg = true; 
	d->ptPos.x = point.x*EXPND;
	d->ptPos.y = (PY/EXPND-point.y-1)*EXPND;
	d->ptMov = d->ptPos;
	Invalidate(FALSE);
	CStatic::OnLButtonDown(nFlags, point);
}

//	左ﾎﾞﾀﾝｱｯﾌﾟ
void CCirc::OnLButtonUp(UINT nFlags, CPoint point)
{
	CCveDlg* d = (CCveDlg*)pv;
	d->bOrg = false;
	Invalidate(FALSE);
	CStatic::OnLButtonUp(nFlags, point);
}
