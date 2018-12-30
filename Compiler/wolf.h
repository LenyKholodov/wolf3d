// wolf.h : main header file for the WOLF application
//

#if !defined(AFX_WOLF_H__5567D31C_5844_4160_8F34_5335156A8F6E__INCLUDED_)
#define AFX_WOLF_H__5567D31C_5844_4160_8F34_5335156A8F6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWolfApp:
// See wolf.cpp for the implementation of this class
//

class CWolfApp : public CWinApp
{
public:
	CWolfApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWolfApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWolfApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WOLF_H__5567D31C_5844_4160_8F34_5335156A8F6E__INCLUDED_)
