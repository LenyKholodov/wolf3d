#ifndef __RESOURCE_HEADER__
#define __RESOURCE_HEADER__

#include "t_obj.h"

#include <binfile.h>

const   int     RES_NAME_LEN         = 128;
const   int     MAX_RES_PARAM_LEN    = 128;

////////////////���ᠭ�� ����ࠪ⭮� �奬� �����///////////////////////////

struct  ResourceHeader
{
  ulong         id ;
  GameObjects   type;

     void        Refresh (GameObjects);

     GameObjects Load (const char*,void* dest); //ࠧ��� >= ࠧ��� �����
     GameObjects Load (BinFile*);
     void        Save (BinFile*,GameObjects);
};

/////////////////////////////////////////////////////////////////////////////
////���ᠭ�� ������� � ���� ���㦥���
/////////////////////////////////////////////////////////////////////////////
struct  Exemplar
{
  char           resName       [RES_NAME_LEN];      //��� 䠩�� ᮤ�ঠ饣� �����
  char           resourceParam [MAX_RES_PARAM_LEN]; //���ᠭ�� �������
};

/////////////////////////////////////////////////////////////////////////////
////�奬� ᮧ����� ��ꥪ� (�� ������)
/////////////////////////////////////////////////////////////////////////////
      //�������� union
template <class Resource,class Param>
class   CreateExemplar
{
  public:
          Param&  param()  { return *((Param*)paramData); }

  public:
    char           paramData    [MAX_RES_PARAM_LEN];
    Resource       resourceClass   ;    
};

////////////////////////////////////////////////////////////////////////////
/////���� ������஢ ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
class   EnvironmentMap
{
  public:
                        EnvironmentMap  (int max = 512);
                        EnvironmentMap  (BinFile* file);
                        EnvironmentMap  (const char* name);
         virtual        ~EnvironmentMap ();

/////////////////////////////////////////////////////////////////////////////
/////����� � ���⮩
/////////////////////////////////////////////////////////////////////////////
         void              AddObject (const Exemplar&);
 const   Exemplar&         GetObject (int) const;
         void              ClearAll  ();

 const   int               count   () const { return mCount; }

/////////////////////////////////////////////////////////////////////////////
/////����㧪� - ���࠭����
/////////////////////////////////////////////////////////////////////////////
         BOOL           Load (BinFile*);
         BOOL           Load (const char*);
         void           Save (BinFile*)      const;
         void           Save (const char*)   const;

  protected:
             Exemplar*          mMap;         //���� ������஢ ��ꥪ⮢
             int                mSize;        //ࠧ��� �����
             int                mCount;       //������ � �⥪�
};

#endif