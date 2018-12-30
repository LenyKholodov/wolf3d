#if !defined(AFX_MAINFORM_H__1E78801B_13F3_43F2_B89C_DC55A27B90CF__INCLUDED_)
#define AFX_MAINFORM_H__1E78801B_13F3_43F2_B89C_DC55A27B90CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainForm dialog

class CMainForm : public CDialog
{
// Construction
public:
	CMainForm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMainForm)
	enum { IDD = IDD_WOLF_FORM };
	CString	m_ResPath;
	CString	m_LevelFile;
	CString	m_LevelName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainForm)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFORM_H__1E78801B_13F3_43F2_B89C_DC55A27B90CF__INCLUDED_)
