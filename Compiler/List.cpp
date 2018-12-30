#include "stdafx.h"
#include "list.h"

/*CListItem::CListItem(CListItem *P,CListItem *N)
   {
      Next=N;
      Prev=P;
      if(Prev!=NULL)
      {
         Number=Prev->Number+1;
      }
       else Number=0;
   }

CListItem::~CListItem()
{
//   printf("\nDeleting %d",Number);
   delete Next;
}

CList::CList()
{
   First=NULL;
   Last=NULL;
   Curent=NULL;
   CurentNumber=0;
   N=0;
}

CList::~CList()
{
   delete First;
}

CList::AddItem(C3DSpr ad)
   {
      if(First!=NULL)
      {
         Last->Next=new CListItem(Last,NULL);
         Last=Last->Next;
         Last->data=ad;
         N++;
      }
      else
      {
         First=new CListItem(NULL,NULL);
         N++;
         First->data=ad;
         Last=First;
      }
   }

C3DSpr CList::GetItem(int num)
   {
      if(Curent==NULL)
         Curent=First;
      if(CurentNumber==num)
         return Curent->data;
      if(CurentNumber<num)
      {
         while(CurentNumber!=num)
         {
            CurentNumber++;
            Curent=Curent->Next;
         }
         return Curent->data;
      }
      else
      {
         while(CurentNumber!=num)
         {
            CurentNumber--;
            Curent=Curent->Prev;
         }
         return Curent->data;
      }
   }
*/
C3DSpr::C3DSpr()
{
int i;
CString s;
	for(i=0;i<8;i++)
	{
		m_bmp[i]=new CArray<CString,CString>;
		m_color[i]=new CArray<int,int>;
	}
//	m_bmpd=new CArray<CString,CString>;
//	m_bmpd->Add(s);
}

/*void C3DSpr::deb()
{
	CString s=m_bmp[0][0][0];
}*/
