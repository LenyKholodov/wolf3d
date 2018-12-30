#ifndef __TYPE_TYPE_TABLE__
#define __TYPE_TYPE_TABLE__

/////////////////////////////////////////////////////////////////////////////
//////��� ��� ��࠭���� ⠡���� ⨯��
/////////////////////////////////////////////////////////////////////////////
struct  TypeTableTag: public GameObjectTag
{
  int   max_objects;
  int   max_types;
  int   numSaves;       //�������⢮ ��࠭��� ��ꥪ⮢

        TypeTableTag (int __maxObj,int __maxTypes)
               : max_objects (__maxObj),
                 max_types (__maxTypes),
                 numSaves (0)
                  {   }

        //����� ���� ⠡����, ��������� �⠭����� TypeTable
};

#endif