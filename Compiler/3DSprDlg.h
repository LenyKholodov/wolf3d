#if !defined(AFX_3DSPRDLG_H__026D5B04_6CDC_4E55_B941_2A784D372D0E__INCLUDED_)
#define AFX_3DSPRDLG_H__026D5B04_6CDC_4E55_B941_2A784D372D0E__INCLUDED_

#include "list.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DSprDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////

// C3DSprDlg dialog

class C3DSprDlg : public CDialog
{
// Construction
public:
	int nmov;
	int nspr;
	CArray<C3DSpr,C3DSpr> m_3DSpr;
	C3DSprDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(C3DSprDlg)
	enum { IDD = IDD_DIALOG1 };
	CSpinButtonCtrl	m_numCurF;
	CSpinButtonCtrl	m_numCrSpr;
	int		m_nmov;
	CString	m_BMP1;
	CString	m_BMP2;
	CString	m_BMP3;
	CString	m_BMP4;
	CString	m_BMP5;
	CString	m_BMP6;
	CString	m_BMP7;
	CString	m_BMP8;
	int		m_color1;
	int		m_color2;
	int		m_color3;
	int		m_color4;
	int		m_color5;
	int		m_color6;
	int		m_color7;
	int		m_color8;
	int		m_nspr;
	int		m_tspr;
	CString	m_ntspr;
	int		m_tf;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DSprDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C3DSprDlg)
	afx_msg void OnOutofmemorySpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutofmemorySpin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton1();
	afx_msg void OnUpdateEdit20();
	afx_msg void OnUpdateEdit22();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DSPRDLG_H__026D5B04_6CDC_4E55_B941_2A784D372D0E__INCLUDED_)
