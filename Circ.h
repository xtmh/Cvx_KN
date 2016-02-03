#pragma once

//#include "afxwin.h"

class CCirc : public CStatic
{
	DECLARE_DYNAMIC(CCirc)
public:
	CCirc(void);
	virtual	~CCirc(void);

public:
	void*	pv;
	CBrush*	pcbRed;
	CBrush*	pcbPpl;
	CBrush*	pcbGrn;
	CPen*	pcpX;
	CPen*	pcpG1;
	CPen*	pcpG2;
	CPen*	pcpB;
	CPen*	pcpW;
	CPen*	pcpR;
	CPen*	pcpR2;
	CPen*	pcpG;
	CPen*	pcpP;
	CPen*	pcpDG;
	CPen*	pcpD;
	CPen*	pOldPen;
	CFont	Font1, Font2;
	CFont*	pOldFont;

	CRect	rc;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

