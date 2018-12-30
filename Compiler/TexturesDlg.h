#if !defined(AFX_TEXTURESDLG_H__D70F52FA_8056_42DF_8C31_E7328B2F3ACE__INCLUDED_)
#define AFX_TEXTURESDLG_H__D70F52FA_8056_42DF_8C31_E7328B2F3ACE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TexturesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTexturesDlg dialog

class CTexturesDlg : public CDialog
{
// Construction
public:
	int m_ntex;
	CTexturesDlg(CWnd* pParent = NULL);   // standard constructor
	CArray <CString,CString> m_Textures;
// Dialog Data
	//{{AFX_DATA(CTexturesDlg)
	enum { IDD = IDD_DIALOG3 };
	CSpinButtonCtrl	m_t;
	int		m_ttex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTexturesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTexturesDlg)
	afx_msg void OnButton1();
	afx_msg void OnOutofmemorySpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTURESDLG_H__D70F52FA_8056_42DF_8C31_E7328B2F3ACE__INCLUDED_)
