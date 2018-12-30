#ifndef __GAME_LOADER__
#define __GAME_LOADER__

#include "g_types.h"
#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
/////��� make-function
/////////////////////////////////////////////////////////////////////////////

        //��� ᮧ������ ��ꥪ�� � ��ࠬ��ࠬ� �ਬ�� ����

typedef GameObject* (*tLoadObject)(BinFile*,ID,GameSystem*);
typedef GameObject* (*tMakeObject)(ArgPtr,ID,GameSystem*);

/////////////////////////////////////////////////////////////////////////////
/////���ᨢ make-functions ��� ᮧ����� ��ꥪ⮢ �ॡ���� ����㧪�
/////////////////////////////////////////////////////////////////////////////

extern tMakeObject     makeFn [MAX_TYPES];
extern tLoadObject     loadFn [MAX_TYPES];

/////////////////////////////////////////////////////////////////////////////
/////������ ��� ॣ����樨 make-function
/////////////////////////////////////////////////////////////////////////////

template <class ClassName,GameObjects ClassType>
class make
{
  public:
                                make  () {
                     makeFn [ClassType] = &makeObject;
                     loadFn [ClassType] = &loadObject;
                                          }

    static      GameObject*     loadObject (BinFile* file,
                                            ID id,
                                            GameSystem* owner = gameSystem)
          {  return new (id,ID(-1,-1),owner) ClassName (file);    }

    static      GameObject*     makeObject (ArgPtr ptr,
                                            ID callback = ID(-1,-1),
                                            GameSystem* owner = gameSystem)
          { return new (ClassType,callback,owner) ClassName (ptr); }

};

/////////////////////////////////////////////////////////////////////////////
////�㭪�� ॣ����樨 ������ ��஢��� �����
/////////////////////////////////////////////////////////////////////////////

BOOL   RegisterGameClass (GameObjects,tMakeObject);


#endif