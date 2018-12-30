// wolfDoc.cpp : implementation of the CWolfDoc class
//

#include "stdafx.h"
#include "wolf.h"

#include "wolfDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWolfDoc

IMPLEMENT_DYNCREATE(CWolfDoc, CDocument)

BEGIN_MESSAGE_MAP(CWolfDoc, CDocument)
	//{{AFX_MSG_MAP(CWolfDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWolfDoc construction/destruction

CWolfDoc::CWolfDoc()
{
	// TODO: add one-time construction code here

}

CWolfDoc::~CWolfDoc()
{
}

BOOL CWolfDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWolfDoc serialization

void CWolfDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWolfDoc diagnostics

#ifdef _DEBUG
void CWolfDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWolfDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWolfDoc commands
