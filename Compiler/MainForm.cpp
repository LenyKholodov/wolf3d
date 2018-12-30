// MainForm.cpp : implementation file
//

#include "stdafx.h"
#include "wolf.h"
#include "MainForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainForm dialog


CMainForm::CMainForm(CWnd* pParent /*=NULL*/)
	: CDialog(CMainForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainForm)
	m_ResPath = _T("");
	m_LevelFile = _T("");
	m_LevelName = _T("");
	//}}AFX_DATA_INIT
}


void CMainForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainForm)
	DDX_Text(pDX, IDC_EDIT1, m_ResPath);
	DDX_Text(pDX, IDC_EDIT2, m_LevelFile);
	DDX_Text(pDX, IDC_EDIT3, m_LevelName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainForm, CDialog)
	//{{AFX_MSG_MAP(CMainForm)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainForm message handlers
