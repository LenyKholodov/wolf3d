// wolfView.h : interface of the CWolfView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WOLFVIEW_H__80F34020_1D6B_4974_B84C_D71EC648CB7A__INCLUDED_)
#define AFX_WOLFVIEW_H__80F34020_1D6B_4974_B84C_D71EC648CB7A__INCLUDED_

#include "3DSprDlg.h"	// Added by ClassView
#include "2DSprDlg.h"	// Added by ClassView
#include "TexturesDlg.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWolfView : public CFormView
{
protected: // create from serialization only
	CWolfView();
	DECLARE_DYNCREATE(CWolfView)

public:
	//{{AFX_DATA(CWolfView)
	enum{ IDD = IDD_WOLF_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CWolfDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWolfView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	CTexturesDlg TexturesDlg;
	C2DSprDlg SprDlg2D;
	C3DSprDlg SprDlg3D;
	virtual ~CWolfView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWolfView)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in wolfView.cpp
inline CWolfDoc* CWolfView::GetDocument()
   { return (CWolfDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WOLFVIEW_H__80F34020_1D6B_4974_B84C_D71EC648CB7A__INCLUDED_)
