class C3DSpr
{
public:
	CArray<CString,CString>	*m_bmp[8];
	CArray<int,int>		*m_color[8];
    int             m_nmov;
    CString			m_ntspr;
	C3DSpr();
};

/*class CListItem
{
public:
   CListItem *Next;
   CListItem *Prev;
   int Number;
   C3DSpr data;
   CListItem(CListItem *P=NULL,CListItem *N=NULL);
   ~CListItem();
};

class CList
{
public:
   CListItem *First;
   CListItem *Last;
   CListItem *Curent;
   int CurentNumber;
   int N;
public:
   CList();
   ~CList();
   AddItem(C3DSpr ad);
   C3DSpr GetItem(int num);
};
*/