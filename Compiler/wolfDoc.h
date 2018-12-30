// wolfDoc.h : interface of the CWolfDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WOLFDOC_H__1EC89B72_8165_466B_B99A_F31663EBCF3C__INCLUDED_)
#define AFX_WOLFDOC_H__1EC89B72_8165_466B_B99A_F31663EBCF3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWolfDoc : public CDocument
{
protected: // create from serialization only
	CWolfDoc();
	DECLARE_DYNCREATE(CWolfDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWolfDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWolfDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWolfDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WOLFDOC_H__1EC89B72_8165_466B_B99A_F31663EBCF3C__INCLUDED_)
