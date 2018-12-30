#ifndef __TYPE_VIEW__
#define __TYPE_VIEW__

#include "g_scrfc.h"

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения просмотровщика
/////////////////////////////////////////////////////////////////////////////
struct    ViewTag
{
  ID              viewPlayer;

  Fixed           x,y;
  Angle           angle;
  Justify         xJust,yJust;
};

////////////////////Определение элементов интерфейсов////////////////////////

struct  ItemDesc
{
  enum  ResourceType
  {
    __NULL__ = 0,
    BIN_FONT = 1,
    BGI_FONT,
    SPRITE_LIST,
    IMAGE_LIST
  };

  ResourceType    type;
  int             xperc, yperc;
  char            resName [128],
                  sendString [32];
  int             sendInt;
  Justify         xJust,yJust;
};

enum    Items
{
  IM_HEALTH = 0,
  IM_PROTECT,
  IM_PLAYER_FACE = 4,
  IM_AM_HAND = 5,
  IM_WP_COUNT=6,
  IM_CENTER_MES,
  IM_SIGHT = 8,
  IM_FRAG_STRING,
  IM_FRAG_COUNT
};

        //CONST!!!

static const ItemDesc        item_desc [MAX_ITEMS] =
{
  {ItemDesc::BIN_FONT,5,90,"BFNT_W3D","",100,LEFT,TOP},
  {ItemDesc::BIN_FONT,95,90,"BFNT_W3D","",100,LEFT,TOP},
  {ItemDesc::SPRITE_LIST,3,80,"SL_IFACE","",0,LEFT,TOP},
  {ItemDesc::SPRITE_LIST,92,80,"SL_IFACE","",1,LEFT,TOP},
  {ItemDesc::SPRITE_LIST,50,90,"SL_CH_FACE","",0,CENTER,TOP},
  {ItemDesc::SPRITE_LIST,100,100,"SL_AUTOMAT","",0,RIGHT,BOTTOM},
  {ItemDesc::BIN_FONT,75,90,"BFNT_W3D","",0,LEFT,TOP},
  {ItemDesc::BGI_FONT,50,50,"BGIFNT_TRIP"," ",0,CENTER,CENTER},
  {ItemDesc::SPRITE_LIST,50,35,"SL_SIGHT","",0,CENTER,CENTER},
  {ItemDesc::BGI_FONT,90,5,"BGIFNT_TRIP","Frags: ",FACE_NO_VALUE,LEFT,TOP},
  {ItemDesc::BIN_FONT,95,6,"BFNT_W3D","",0,LEFT,TOP},
  {ItemDesc::__NULL__,0,0,"",LEFT,TOP}
};

////////////////////////////////Сообщения GameView///////////////////////////

enum    GameViewMessagesId
{
  CHANGE_ITEM_VIEW = GAME_VIEW_LIMIT,
  CHANGE_STR_ITEM_VIEW,
  RELOAD_ITEM_VIEW,
  MOVE_ITEM_START,
  MOVE_ITEM_TO,
  MOVE_ITEM_REL,
  CHANGE_SCREEN,
  PIC_MIP
};

/////////////////////////////////////////////////////////////////////////////
////Сменить фазу
/////////////////////////////////////////////////////////////////////////////
struct   ViewChangeItemMessage: public GameMessage
{
         ViewChangeItemMessage (int itemDesc,int _face):
                               item (itemDesc), face (_face)
           { id = CHANGE_ITEM_VIEW; }

         int    item, face;
};

struct   ViewChangeStrItemMessage: public GameMessage
{
         ViewChangeStrItemMessage (int itemDesc,const char* x):
                               item (itemDesc)
           { id = CHANGE_STR_ITEM_VIEW; strcpy (dest,x); }

         int    item;
         char   dest [128];
};

/////////////////////////////////////////////////////////////////////////////
////Перезагрузить эелемент
/////////////////////////////////////////////////////////////////////////////
struct   ViewReloadItemMessage: public GameMessage
{
         ViewReloadItemMessage (int itemDesc,int index):
                              item (itemDesc), resource (index)
           { id = RELOAD_ITEM_VIEW; }

         int    item, resource;

};

/////////////////////////////////////////////////////////////////////////////
////Переместить элемент
/////////////////////////////////////////////////////////////////////////////
struct   ViewMoveToMessage: public GameMessage
{
         ViewMoveToMessage (int itemDesc,int _x,int _y):
                              item (itemDesc), x(_x), y(_y)
           { id = MOVE_ITEM_TO; }

         int    item;
         int    x,y;
};

/////////////////////////////////////////////////////////////////////////////
////Вернуть в начальное положение
/////////////////////////////////////////////////////////////////////////////
struct   ViewMoveStartMessage: public GameMessage
{
         ViewMoveStartMessage (int itemDesc):
                              item (itemDesc)
           { id = MOVE_ITEM_START; }

         int    item;
};

/////////////////////////////////////////////////////////////////////////////
////Переместить элемент
/////////////////////////////////////////////////////////////////////////////
struct   ViewMoveRelMessage: public GameMessage
{
         ViewMoveRelMessage (int itemDesc,int _x,int _y):
                              item (itemDesc), x(_x), y(_y)
           { id = MOVE_ITEM_REL; }

         int    item;
         int    x,y;
};

/////////////////////////////////////////////////////////////////////////////
////Новый экран
/////////////////////////////////////////////////////////////////////////////
struct   ViewChangeScreenMessage: public GameMessage
{
         ViewChangeScreenMessage (Screen* dest):
                              screen(dest)
           { id = CHANGE_SCREEN; }

        Screen* screen;
};

/////////////////////////////////////////////////////////////////////////////
////Детализация
/////////////////////////////////////////////////////////////////////////////
struct   ViewPicMipMessage: public GameMessage
{
         ViewPicMipMessage (int picMip):
                              picmip(picMip)
           { id = PIC_MIP; }

         int picmip;
};



#endif