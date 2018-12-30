#ifndef __GAME_BOX__
#define __GAME_BOX__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
///////Коробка с чем-то (патроны, жизнь, броня и т.д)
/////////////////////////////////////////////////////////////////////////////
class   GameBox: public GameObject
{
  public:
//                        GameBox  (int = 0);
                        GameBox  (int first,int max); //default = 0
                        GameBox  (ArgPtr);
                        GameBox  (BinFile*);
         virtual        ~GameBox ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)       ;
    virtual     void            Save (BinFile*)       ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_BOX; }
    virtual     void            dump (ostream&) const;

        //Принадлежность какому - то типу коробок
    virtual     BoxTypes        belong () const { return (BoxTypes)-1; }
                BOOL            full   () const { return mCount == mMax; }

                void            AddStore  (int);
                void            SubStore  (int);
                void            SetStore  (int);

                int             count   () const { return mCount; }
                int             GetStore () const { return mCount; }

  protected:
                int             mCount;
                int             mMax;
};

extern GameBox  gigaBox;        //коробка с бесконечными патронами

#endif