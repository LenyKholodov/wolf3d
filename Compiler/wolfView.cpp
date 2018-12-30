// wolfView.cpp : implementation of the CWolfView class
//

#include "stdafx.h"
#include "wolf.h"

#include "wolfDoc.h"
#include "wolfView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWolfView

IMPLEMENT_DYNCREATE(CWolfView, CFormView)

BEGIN_MESSAGE_MAP(CWolfView, CFormView)
	//{{AFX_MSG_MAP(CWolfView)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWolfView construction/destruction

CWolfView::CWolfView()
	: CFormView(CWolfView::IDD)
{
	//{{AFX_DATA_INIT(CWolfView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CWolfView::~CWolfView()
{
}

void CWolfView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWolfView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CWolfView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CWolfView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

}

/////////////////////////////////////////////////////////////////////////////
// CWolfView printing

BOOL CWolfView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWolfView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWolfView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWolfView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CWolfView diagnostics

#ifdef _DEBUG
void CWolfView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWolfView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CWolfDoc* CWolfView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWolfDoc)));
	return (CWolfDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWolfView message handlers

void CWolfView::OnButton1() 
{
	// TODO: Add your control notification handler code here
	SprDlg3D.DoModal();
}


void CWolfView::OnButton2() 
{
	// TODO: Add your control notification handler code here
	SprDlg2D.DoModal();	
}

void CWolfView::OnButton3() 
{
	// TODO: Add your control notification handler code here
	TexturesDlg.DoModal();
}
