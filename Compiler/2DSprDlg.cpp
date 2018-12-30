// 2DSprDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wolf.h"
#include "2DSprDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C2DSprDlg dialog


C2DSprDlg::C2DSprDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C2DSprDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C2DSprDlg)
	m_tspr = 0;
	m_nspr = 0;
	m_ntspr = _T("");
	m_first = 1;
	//}}AFX_DATA_INIT
}


void C2DSprDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C2DSprDlg)
	try
	{
	DDX_Control(pDX, IDC_SPIN1, m_NumTSpr);
	DDX_Text(pDX, IDC_EDIT1, m_tspr);
	if(m_nspr!=0)
	{
		DDX_Text(pDX, IDC_EDIT2, m_2DSpr[m_tspr].m_ntspr);
		DDX_Text(pDX, IDC_EDIT3, m_2DSpr[m_tspr].m_bmp[0][0][0]);
		DDX_Text(pDX, IDC_EDIT4, m_2DSpr[m_tspr].m_bmp[1][0][0]);

		DDX_Text(pDX, IDC_EDIT5, m_2DSpr[m_tspr].m_color[0][0][0]);
		DDX_Text(pDX, IDC_EDIT6, m_2DSpr[m_tspr].m_color[1][0][0]);
	}
	}
	catch(char *error)
	{
	}
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C2DSprDlg, CDialog)
	//{{AFX_MSG_MAP(C2DSprDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN1, OnOutofmemorySpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_EN_UPDATE(IDC_EDIT2, OnUpdateEdit2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DSprDlg message handlers

void C2DSprDlg::OnOutofmemorySpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );
	UpdateData( FALSE );

	*pResult = 0;
}

void C2DSprDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_tspr = m_NumTSpr.GetPos()-65536;
	if(m_tspr>=m_nspr) m_tspr=m_nspr-1;

	UpdateData( FALSE );

	*pResult = 0;
}


void C2DSprDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	C2DSpr Spr2D;
	m_2DSpr.Add(Spr2D);
	m_nspr++;
//	UpdateData(TRUE);
//	UpdateData(FALSE);
}

void C2DSprDlg::OnUpdateEdit2() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	UpdateData( FALSE );
}

BOOL C2DSprDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}
