#if !defined(AFX_2DSPRDLG_H__265BBC45_A641_41F6_BC3F_5C0DC5CE9254__INCLUDED_)
#define AFX_2DSPRDLG_H__265BBC45_A641_41F6_BC3F_5C0DC5CE9254__INCLUDED_

#include "2DSpr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 2DSprDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C2DSprDlg dialog

class C2DSprDlg : public CDialog
{
// Construction
public:
	int m_nspr;
	C2DSprDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext);
	CArray <C2DSpr,C2DSpr> m_2DSpr;
// Dialog Data
	//{{AFX_DATA(C2DSprDlg)
	enum { IDD = IDD_DIALOG2 };
	CSpinButtonCtrl	m_NumTSpr;
	int		m_tspr;
	CString	m_ntspr;
	int	m_first;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C2DSprDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C2DSprDlg)
	afx_msg void OnButton1();
	afx_msg void OnOutofmemorySpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEdit2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DSPRDLG_H__265BBC45_A641_41F6_BC3F_5C0DC5CE9254__INCLUDED_)
