// 3DSprDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wolf.h"
#include "wolfdoc.h"
#include "3DSprDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DSprDlg dialog


C3DSprDlg::C3DSprDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C3DSprDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C3DSprDlg)
	m_nmov = 0;
	m_BMP1 = _T("");
	m_BMP2 = _T("");
	m_BMP3 = _T("");
	m_BMP4 = _T("");
	m_BMP5 = _T("");
	m_BMP6 = _T("");
	m_BMP7 = _T("");
	m_BMP8 = _T("");
	m_color1 = 0;
	m_color2 = 0;
	m_color3 = 0;
	m_color4 = 0;
	m_color5 = 0;
	m_color6 = 0;
	m_color7 = 0;
	m_color8 = 0;
	m_nspr = 0;
	m_tspr = 0;
	m_ntspr = _T("");
	m_tf = 0;
	//}}AFX_DATA_INIT
	nspr=0;
}


void C3DSprDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C3DSprDlg)
	DDX_Control(pDX, IDC_SPIN2, m_numCurF);
	DDX_Control(pDX, IDC_SPIN1, m_numCrSpr);
	DDX_Text(pDX, IDC_EDIT1, m_nmov);
	DDX_Text(pDX, IDC_EDIT2, m_BMP1);
//	DDX_Text(pDX, IDC_EDIT2, *m_3DSpr[m_tspr].m_bmp[1]);
	DDX_Text(pDX, IDC_EDIT3, m_BMP2);
	DDX_Text(pDX, IDC_EDIT4, m_BMP3);
	DDX_Text(pDX, IDC_EDIT5, m_BMP4);
	DDX_Text(pDX, IDC_EDIT6, m_BMP5);
	DDX_Text(pDX, IDC_EDIT7, m_BMP6);
	DDX_Text(pDX, IDC_EDIT8, m_BMP7);
	DDX_Text(pDX, IDC_EDIT9, m_BMP8);
	DDX_Text(pDX, IDC_EDIT10, m_color1);
	DDX_Text(pDX, IDC_EDIT11, m_color2);
	DDX_Text(pDX, IDC_EDIT12, m_color3);
	DDX_Text(pDX, IDC_EDIT13, m_color4);
	DDX_Text(pDX, IDC_EDIT14, m_color5);
	DDX_Text(pDX, IDC_EDIT15, m_color6);
	DDX_Text(pDX, IDC_EDIT16, m_color7);
	DDX_Text(pDX, IDC_EDIT17, m_color8);
	DDX_Text(pDX, IDC_EDIT18, m_nspr);
	DDX_Text(pDX, IDC_EDIT20, m_tspr);
	DDX_Text(pDX, IDC_EDIT21, m_ntspr);
	DDX_Text(pDX, IDC_EDIT22, m_tf);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C3DSprDlg, CDialog)
	//{{AFX_MSG_MAP(C3DSprDlg)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN1, OnOutofmemorySpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN2, OnOutofmemorySpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_EN_UPDATE(IDC_EDIT20, OnUpdateEdit20)
	ON_EN_UPDATE(IDC_EDIT22, OnUpdateEdit22)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DSprDlg message handlers

void C3DSprDlg::OnOutofmemorySpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	m_tspr = m_numCrSpr.GetPos();

	UpdateData( FALSE );

	*pResult = 0;
}

void C3DSprDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateData( TRUE );
	int n=m_tspr;

	m_3DSpr[m_tspr].m_ntspr=m_ntspr;
	m_3DSpr[m_tspr].m_nmov=m_nmov;

	if(m_3DSpr[m_tspr].m_nmov!=0)
	{
		m_3DSpr[m_tspr].m_bmp[0][0][m_tf]=m_BMP1;
		m_3DSpr[m_tspr].m_bmp[1][0][m_tf]=m_BMP2;
		m_3DSpr[m_tspr].m_bmp[2][0][m_tf]=m_BMP3;
		m_3DSpr[m_tspr].m_bmp[3][0][m_tf]=m_BMP4;
		m_3DSpr[m_tspr].m_bmp[4][0][m_tf]=m_BMP5;
		m_3DSpr[m_tspr].m_bmp[5][0][m_tf]=m_BMP6;
		m_3DSpr[m_tspr].m_bmp[6][0][m_tf]=m_BMP7;
		m_3DSpr[m_tspr].m_bmp[7][0][m_tf]=m_BMP8;

		m_3DSpr[m_tspr].m_color[0][0][m_tf]=m_color1;
		m_3DSpr[m_tspr].m_color[1][0][m_tf]=m_color2;
		m_3DSpr[m_tspr].m_color[2][0][m_tf]=m_color3;
		m_3DSpr[m_tspr].m_color[3][0][m_tf]=m_color4;
		m_3DSpr[m_tspr].m_color[4][0][m_tf]=m_color5;
		m_3DSpr[m_tspr].m_color[5][0][m_tf]=m_color6;
		m_3DSpr[m_tspr].m_color[6][0][m_tf]=m_color7;
		m_3DSpr[m_tspr].m_color[7][0][m_tf]=m_color8;
	}

	m_tspr = m_numCrSpr.GetPos()-65536;
	if(m_tspr>=nspr) m_tspr=nspr-1;

//	UpdateData( FALSE );

	m_ntspr=m_3DSpr[m_tspr].m_ntspr;
	nmov=m_3DSpr[m_tspr].m_nmov;
	m_nmov=m_3DSpr[m_tspr].m_nmov;
	m_tf=0;
	
	if(m_3DSpr[m_tspr].m_nmov!=0)
	{
		m_BMP1=m_3DSpr[m_tspr].m_bmp[0][0][m_tf];
		m_BMP2=m_3DSpr[m_tspr].m_bmp[1][0][m_tf];
		m_BMP3=m_3DSpr[m_tspr].m_bmp[2][0][m_tf];
		m_BMP4=m_3DSpr[m_tspr].m_bmp[3][0][m_tf];
		m_BMP5=m_3DSpr[m_tspr].m_bmp[4][0][m_tf];
		m_BMP6=m_3DSpr[m_tspr].m_bmp[5][0][m_tf];
		m_BMP7=m_3DSpr[m_tspr].m_bmp[6][0][m_tf];
		m_BMP8=m_3DSpr[m_tspr].m_bmp[7][0][m_tf];

		m_color1=m_3DSpr[m_tspr].m_color[0][0][m_tf];
		m_color2=m_3DSpr[m_tspr].m_color[1][0][m_tf];
		m_color3=m_3DSpr[m_tspr].m_color[2][0][m_tf];
		m_color4=m_3DSpr[m_tspr].m_color[3][0][m_tf];
		m_color5=m_3DSpr[m_tspr].m_color[4][0][m_tf];
		m_color6=m_3DSpr[m_tspr].m_color[5][0][m_tf];
		m_color7=m_3DSpr[m_tspr].m_color[6][0][m_tf];
		m_color8=m_3DSpr[m_tspr].m_color[7][0][m_tf];
	}
	else
	{
		m_BMP1="";
		m_BMP2="";
		m_BMP3="";
		m_BMP4="";
		m_BMP5="";
		m_BMP6="";
		m_BMP7="";
		m_BMP8="";

		m_color1=0;
		m_color2=0;
		m_color3=0;
		m_color4=0;
		m_color5=0;
		m_color6=0;
		m_color7=0;
		m_color8=0;
	}

	UpdateData( FALSE );

	*pResult = 0;
}

void C3DSprDlg::OnOutofmemorySpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	m_tf = m_numCurF.GetPos();
	UpdateData( FALSE );
	OnUpdateEdit20();
	*pResult = 0;
}

void C3DSprDlg::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );
	
	m_3DSpr[m_tspr].m_bmp[0][0][m_tf]=m_BMP1;
	m_3DSpr[m_tspr].m_bmp[1][0][m_tf]=m_BMP2;
	m_3DSpr[m_tspr].m_bmp[2][0][m_tf]=m_BMP3;
	m_3DSpr[m_tspr].m_bmp[3][0][m_tf]=m_BMP4;
	m_3DSpr[m_tspr].m_bmp[4][0][m_tf]=m_BMP5;
	m_3DSpr[m_tspr].m_bmp[5][0][m_tf]=m_BMP6;
	m_3DSpr[m_tspr].m_bmp[6][0][m_tf]=m_BMP7;
	m_3DSpr[m_tspr].m_bmp[7][0][m_tf]=m_BMP8;

	m_3DSpr[m_tspr].m_color[0][0][m_tf]=m_color1;
	m_3DSpr[m_tspr].m_color[1][0][m_tf]=m_color2;
	m_3DSpr[m_tspr].m_color[2][0][m_tf]=m_color3;
	m_3DSpr[m_tspr].m_color[3][0][m_tf]=m_color4;
	m_3DSpr[m_tspr].m_color[4][0][m_tf]=m_color5;
	m_3DSpr[m_tspr].m_color[5][0][m_tf]=m_color6;
	m_3DSpr[m_tspr].m_color[6][0][m_tf]=m_color7;
	m_3DSpr[m_tspr].m_color[7][0][m_tf]=m_color8;

	m_tf = m_numCurF.GetPos() - 65536;
	if(m_tf>=m_nmov) m_tf=m_nmov-1;

	m_BMP1=m_3DSpr[m_tspr].m_bmp[0][0][m_tf];
	m_BMP2=m_3DSpr[m_tspr].m_bmp[1][0][m_tf];
	m_BMP3=m_3DSpr[m_tspr].m_bmp[2][0][m_tf];
	m_BMP4=m_3DSpr[m_tspr].m_bmp[3][0][m_tf];
	m_BMP5=m_3DSpr[m_tspr].m_bmp[4][0][m_tf];
	m_BMP6=m_3DSpr[m_tspr].m_bmp[5][0][m_tf];
	m_BMP7=m_3DSpr[m_tspr].m_bmp[6][0][m_tf];
	m_BMP8=m_3DSpr[m_tspr].m_bmp[7][0][m_tf];

	m_color1=m_3DSpr[m_tspr].m_color[0][0][m_tf];
	m_color2=m_3DSpr[m_tspr].m_color[1][0][m_tf];
	m_color3=m_3DSpr[m_tspr].m_color[2][0][m_tf];
	m_color4=m_3DSpr[m_tspr].m_color[3][0][m_tf];
	m_color5=m_3DSpr[m_tspr].m_color[4][0][m_tf];
	m_color6=m_3DSpr[m_tspr].m_color[5][0][m_tf];
	m_color7=m_3DSpr[m_tspr].m_color[6][0][m_tf];
	m_color8=m_3DSpr[m_tspr].m_color[7][0][m_tf];

	UpdateData( FALSE );
	*pResult = 0;
}

void C3DSprDlg::OnUpdateEdit20() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
/*	m_3DSpr[m_tspr].m_ntspr=m_ntspr;
	m_3DSpr[m_tspr].m_nmov=nmov;

	m_ntspr=m_3DSpr[m_tspr].m_ntspr;
	nmov=m_3DSpr[m_tspr].m_nmov;*/
}

void C3DSprDlg::OnUpdateEdit22() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
}

void C3DSprDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	C3DSpr s;
	CString ss="Spr";
	s.m_nmov=0;
//	s.m_ntspr=ss;
	m_3DSpr.Add(s);
	nspr++;
}

void C3DSprDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	CString s;
	int c=0;
	int i;
	for(i=0;i<8;i++)
	{
		m_3DSpr[m_tspr].m_bmp[i]->Add(s);
		m_3DSpr[m_tspr].m_color[i]->Add(c);
	}
	m_nmov++;
	UpdateData(FALSE);
}

BOOL C3DSprDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}
