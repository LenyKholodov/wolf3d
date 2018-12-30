// TexturesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wolf.h"
#include "TexturesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTexturesDlg dialog


CTexturesDlg::CTexturesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexturesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTexturesDlg)
	m_ntex=0;
	m_ttex = 0;
	//}}AFX_DATA_INIT
}


void CTexturesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTexturesDlg)
	DDX_Control(pDX, IDC_SPIN1, m_t);
	DDX_Text(pDX, IDC_EDIT1, m_ttex);
	if(m_ntex!=0)
		{
		DDX_Text(pDX, IDC_EDIT2, m_Textures[m_ttex]);
		}
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CTexturesDlg, CDialog)
	//{{AFX_MSG_MAP(CTexturesDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN1, OnOutofmemorySpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTexturesDlg message handlers

void CTexturesDlg::OnOutofmemorySpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	UpdateData( FALSE );

	*pResult = 0;
}

void CTexturesDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	m_ttex = m_t.GetPos()-65536;
	if(m_ttex>=m_ntex) m_ttex=m_ntex-1;

	UpdateData( FALSE );
	
	*pResult = 0;
}

void CTexturesDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString s;
	m_Textures.Add(s);
	m_ntex++;
}
