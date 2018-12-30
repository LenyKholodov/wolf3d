#ifndef __CACHE__
#define __CACHE__

#include <stdlib.h>      //for NULL

#include <memory/memory.h>
#include <binfile.h>

/////////////////////////////////////////////////////////////////////////////
////���-���� (������騩 ࠧ���)
/////////////////////////////////////////////////////////////////////////////
class   Cache;
struct  CacheList
{
  private:
        friend class    Cache;
        long            id;
        CacheList       *prev,
                        *next;

  public:
        uchar           data[1];  //�����
};

/////////////////////////////////////////////////////////////////////////////
////����� �����⢫��騩 ���஢���� ���ଠ樨 (����㧪� ����㠫쭠)
/////////////////////////////////////////////////////////////////////////////
////����� �࠭���� �� ���ᠭ��� ���-�����
/////////////////////////////////////////////////////////////////////////////
class   Cache
{
  public:
                       Cache  ();
                       Cache  (long id_num,long cache_size,long cl_size = 0);
          virtual      ~Cache ();

          virtual void     Close  ();
          virtual void     Open   (long id_num,long cache_size,long cl_size);

/////////////////////////////////////////////////////////////////////////////
////����� � �����
/////////////////////////////////////////////////////////////////////////////
          void*        GetData(long id);

/////////////////////////////////////////////////////////////////////////////
////��㦥���
/////////////////////////////////////////////////////////////////////////////
          long         GetNum()         const { return id_size; }
          long         GetCacheSize()   const { return cache_size; }
          long         GetClasterSize() const { return cl_size;}

  protected:
    virtual  void         LoadData(long id,CacheList* dest) = 0;
             void         ChangeReq(CacheList* block);

  protected:
////////////////////////////����� � ��⥬��////////////////////////////////
           Pool*        queue_pool;     //�� �����஢
           CacheList**  fast;           //������ ����ண� ����㯠 � ����

           CacheList    *first,         //��� ��⠢�� � ����� �� ���
                        *last;

//////////////////////////////��㦥���� ���ଠ��///////////////////////////
           long         cache_size,
                        id_size,
                        cl_size;

//////////////////////////////�㭪樨////////////////////////////////////////
           CacheList*   Alloc(CacheList* prev=NULL,CacheList* next=NULL);


};

////////////////////////////////////////////////////////////////////////////
////���ᠭ�� 䠩�� Name Cache
////////////////////////////////////////////////////////////////////////////
#pragma pack (1)
struct  NameCacheHeader
{
          uchar         id[4];          //"NCCH"
          long          num;            //�������⢮ ����㥬�� ����⮢
          long          cl_size;        //������ ������
          long          names_size;     //������ �㫠 � �������
          long          names_num;      //�������⮢ ����
};
#pragma pack(0)

/////////////////////////////////////////////////////////////////////////////
///////��� ��� (���ਬ�� 䠩���)
/////////////////////////////////////////////////////////////////////////////
class     NameCache: public Cache
{
  public:
                  NameCache  (char* names[],
                              int  names_num,
                              long id_num,
                              long cache_sz,
                              long cl_sz);
                  NameCache  (BinFile* file,long cache_sz=1);
                  NameCache  (const char* name,long cache_sz=1);
    virtual       ~NameCache ()                               { Close(); }

    virtual void            Close();

            int             GetNamesNum () const { return namesNum; }

/////////////////////////////////////////////////////////////////////////////
/////��� ����� �� �������
/////////////////////////////////////////////////////////////////////////////
            char*           GetName (int) const;

/////////////////////////////////////////////////////////////////////////////
/////���࠭���� - ����㧪� ��� ���
/////////////////////////////////////////////////////////////////////////////
    virtual void            Save (BinFile* file);
            void            Save (const char* name);
    virtual BOOL            Load (BinFile* file,int size);
            BOOL            Load (const char* name,int size);

  protected:
            char*          pool;        //���ᨢ ��� � ᬥ饭��
            ulong*         files;       //���ᨢ ᬥ饭�� ���
            ulong          pool_size;   //������ �㫠 � �������
            int            namesNum;    //������⢮ ���

  protected:
//////////////////////��㦥��� �㭪樨//////////////////////////////////////
                                //����㧪� �� ��⮬���
    virtual void            LoadData (long,CacheList* x) { x->data[0] = -1; }

};

#endif