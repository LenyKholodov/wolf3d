#include "stdafx.h"
#include "2DSpr.h"

C2DSpr::C2DSpr()
{
int i;
CString bmp;
int color=0;

	for(i=0;i<2;i++)
	{
		m_bmp[i]=new CArray<CString,CString>;
		m_color[i]=new CArray<CString,CString>;

		m_bmp[i]->Add(bmp);
		m_color[i]->Add(color);
	}
}
