#include "g_view.h"
#include "g_player.h"
#include "g_system.h"

#include "cross.h"

#include <mscreen.h>

/////////////////////////implementation of class VideoSystem/////////////////

void            VideoSystem::DrawView (Fixed x,Fixed y,Angle ang)
{
  Flag                  flag ("VIEW_IN",TRUE);

  if (!mConsole)
  {
    if (render) render->DrawView (x,y,ang);
    if (face)   face   -> Draw();
  }
  else
    screen->cls(1);

  console-> tick();
  screen -> refresh();
}

void            VideoSystem::InitFaceSystem (Level* level)
{
  ResourceIndex*    resTable    = level->GetResourceTable();
  SpriteListCache*  spriteCache = (SpriteListCache*)level->GetSpriteListCache ();

  for (int i=0;i<MAX_ITEMS && item_desc [i].type != ItemDesc::__NULL__;i++)
  {
    FaceItem* item = NULL;

    int      x     = TransformCoord (item_desc[i].xperc,screen->width()),
             y     = TransformCoord (item_desc[i].yperc,screen->height());
    Justify  xJust = item_desc[i].xJust,
             yJust = item_desc[i].yJust;

    switch (item_desc [i].type)
    {
      case ItemDesc::BIN_FONT:
      {
        item = new FaceBinFont (x,y,level->GetBinFont (resTable->GetIndex(item_desc[i].resName) ),
                                xJust,yJust);
        break;
      }
      case ItemDesc::BGI_FONT:
      {
        item = new FaceBGIFont (x,y,level->GetBGIFont (resTable->GetIndex(item_desc[i].resName) ),
                                xJust,yJust);
        break;
      }
      case ItemDesc::SPRITE_LIST:
      {
        item = new FaceSprite  (x,y,spriteCache,resTable->GetIndex(item_desc[i].resName) ,
                                xJust,yJust);
        break;
      }
      default : item = NULL; break;
    }

    if (item)
    {
      items[i] = face->AddFace (item);

      if (item_desc[i].sendInt != -1)  item->Send (item_desc[i].sendInt);
      if (item_desc[i].sendString[0])  item->Send (item_desc[i].sendString);
    }
  }
}

void            VideoSystem::Send    (int i,int face)
{
  if (i > MAX_ITEMS || i < 0 || items[i] == NULL || items[i]->item == NULL)
      return;

  items[i]->item->Send (face);
}

void            VideoSystem::Send    (int i,const char* x)
{
  if (i > MAX_ITEMS || i < 0 || items[i] == NULL || items[i]->item == NULL)
      return;

  items[i]->item->Send (x);
}

void            VideoSystem::Reload  (int i,int res)
{
  if (i > MAX_ITEMS || i < 0 || items[i] == NULL || items[i]->item == NULL)
      return;

  items [i]->item->Reload (res);
}

void            VideoSystem::MoveStart(int i)
{
  if (i > MAX_ITEMS || i < 0 || items[i] == NULL || items[i]->item == NULL)
      return;

  int x = TransformCoord (item_desc[i].xperc,screen->width()),
      y = TransformCoord (item_desc[i].yperc,screen->height());

  items [i]->item->MoveTo (x,y);
}

void            VideoSystem::MoveTo  (int i,int x,int y)
{
  if (i > MAX_ITEMS || i < 0 || items[i] == NULL || items[i]->item == NULL)
      return;

  x     = TransformCoord (x,screen->width());
  y     = TransformCoord (y,screen->height());

  items [i]->item->MoveTo (x,y);
}

void            VideoSystem::MoveRel  (int i,int x,int y)
{
  if (i > MAX_ITEMS || i < 0 || items[i] == NULL || items[i]->item == NULL)
      return;

  x     = items[i]->item->getx()+TransformCoord (x,screen->width());
  y     = items[i]->item->gety()+TransformCoord (y,screen->height());

  items [i]->item->MoveTo (x,y);
}

void            VideoSystem::refresh (Level* lev)
{
  level        = lev;

  if (console)     console->open (screen,consoleFont);
  else             console = new Console (screen,consoleFont);

  if (lev == NULL)
     return;

  int upRender = level->GetRenderType() & 0xFF0000;
  int index    = upRender == SKY ? 1 : 0;
  int detail   = render && mPicMip!=-1 ? picmipTable[index][mPicMip]
                                       : level->GetRenderType();

  if (face)   delete face;
  if (render) delete render;

  render       = new Render(screen,detail,FALSE);

  render->StagePtr() -> SetTexture  (level->GetTextCache());
  render->StagePtr() -> SetMap      (level->GetMap());
  render->StagePtr() -> SetLightMap (level->GetLightMap());
  render->StagePtr() -> SetCache    (level->GetSpriteCache());
  render->FloorPtr() -> SetTexture  (level->GetFloorText());
  render->SkyPtr()   -> SetTexture  (level->GetSkyText());

  render             -> Reset();

        //Графический интерфейс

//  if (face == NULL)
//  {
    face    = new ScreenFace (screen);

    InitFaceSystem (level);
//  }
}

void      VideoSystem::refresh (Screen* scr)
{
  if (screen) delete screen;

  if ((screen = GimmeMirror (scr)) == NULL)
  {
    dout<<"Video system: No enough memory for double screen"<<endl;
    delete fin;
  }

  screen->cls();

  refresh(level);
}

void      VideoSystem::picmip  (int picMip)
{
  if (picMip>=MAX_PIC_MIP) picMip = MAX_PIC_MIP-1;
  if (picMip<0)            picMip = 0;

  mPicMip      = picMip;
  int upRender = level->GetRenderType() & 0xFF0000;
  int index    = upRender == SKY ? 1 : 0;

  render->Open(screen,picmipTable[index][picMip],FALSE);

  render->StagePtr() -> SetTexture  (level->GetTextCache());
  render->StagePtr() -> SetMap      (level->GetMap());
  render->StagePtr() -> SetLightMap (level->GetLightMap());
  render->StagePtr() -> SetCache    (level->GetSpriteCache());
  render->FloorPtr() -> SetTexture  (level->GetFloorText());
  render->SkyPtr()   -> SetTexture  (level->GetSkyText());

  render             -> Reset();
}

void      VideoSystem::OnlyConsole (BOOL x)
{
  mConsole = x;

  if (x)
  {
    grout<<"Only console mode ON"<<endl;

    console->show();
  }
  else
  {
    grout<<"Only console mode OFF"<<endl;

    console->hide();
  }
}

VideoSystem::VideoSystem (Level* lev,Screen* dest)
         : screen(NULL), render(NULL), face (NULL), console(NULL),
           level(lev), mPicMip (-1), consoleFont (NULL), mConsole (FALSE)
{
  consoleFont = tBinFont::load (CONSOLE_FONT_NAME);

  if (consoleFont == NULL)
  {
    dout<<"Video system: Error load console font '"<<CONSOLE_FONT_NAME<<"'"<<endl;
    D_Error ("HALT!");
  }

  refresh (dest);
}

VideoSystem::~VideoSystem ()
{
  if (render) delete render;
  if (face)   delete face;
  if (screen) delete screen;
  if (console)delete console;

  if (consoleFont) free (consoleFont);
}


/////////////////////////implementation of class GameView////////////////////

void            GameView::Load (BinFile* file)
{
  GameObject::Load (file);
}

void            GameView::Save (BinFile* file)
{
  GameObject::Save (file);
}

BOOL            GameView::Exec (GameMessage* mes)
{
  Flag  flag ("GAME_VIEW_EXEC_IN");

  switch (mes->id)
  {
    case TICK:
    {
      GetPlayerInfoMessage  getinfo (id());

      owner->PutMessage (&getinfo,viewPlayer,sizeof(getinfo),id());

      if (state.x != -1 && state.y != -1)
      {
        Redraw ();
      }

      return TRUE;
    }
    case PLAYER_INFO:
    {
      PlayerInfoMessage*  info = (PlayerInfoMessage*)mes;

      state = *info;

      return TRUE;
    }
    case  RELOAD_ITEM_VIEW:
    {
      ViewReloadItemMessage* message = (ViewReloadItemMessage*)mes;

      video->Reload (message->item,message->resource);

      return TRUE;
    }
    case  MOVE_ITEM_TO:
    {
      ViewMoveToMessage* message = (ViewMoveToMessage*)mes;

      video->MoveTo (message->item,message->x,message->y);

      return TRUE;
    }
    case  MOVE_ITEM_START:
    {
      ViewMoveStartMessage* message = (ViewMoveStartMessage*)mes;

      video->MoveStart (message->item);

      return TRUE;
    }
    case  MOVE_ITEM_REL:
    {
      ViewMoveRelMessage* message = (ViewMoveRelMessage*)mes;

      video->MoveRel (message->item,message->x,message->y);

      return TRUE;
    }
    case  CHANGE_ITEM_VIEW:
    {
      ViewChangeItemMessage* message = (ViewChangeItemMessage*)mes;

      video->Send (message->item,message->face);

      return TRUE;
    }
    case  CHANGE_STR_ITEM_VIEW:
    {
      ViewChangeStrItemMessage* message = (ViewChangeStrItemMessage*)mes;

      video->Send (message->item,message->dest);

      return TRUE;
    }
    case CHANGE_SCREEN:
    {
      ViewChangeScreenMessage* message = (ViewChangeScreenMessage*)mes;

      video->refresh(message->screen);

      return TRUE;
    }
    case PIC_MIP:
    {
      ViewPicMipMessage* message = (ViewPicMipMessage*)mes;

      video->picmip(message->picmip);

      return TRUE;
    }
    default: return FALSE;
  }
}

void            GameView::Redraw()
{
//  if (state.x != -1 && state.y != -1)
//  {
    clock_t start= clock();

    video->Send     (IM_HEALTH,state.health);
    video->Send     (IM_PROTECT,state.protect);
    video->DrawView (state.x,state.y,state.angle);

    clock_t end = clock();

    if (end-start > MAX_VIEW_TIME && curPrior>1)
    {
       owner->AddActive (id(),--curPrior);
    }
    else if (end-start < MIN_VIEW_TIME && curPrior<31)
    {
       owner->AddActive (id(),++curPrior);
    }
//  }
}

void            GameView::dump (ostream& os) const
{
  os<<"View render: player ("<<viewPlayer.type<<","
    <<viewPlayer.id<<")"<<endl;
}

GameView::GameView (ArgPtr)
         : viewPlayer (ID(-1,-1)), state (-1,-1,0), video (NULL),
           curPrior(PR_VIEW)
           {   }

GameView::GameView (BinFile* file)
         : viewPlayer (ID(-1,-1)), state (-1,-1,0), video (NULL),
           curPrior(PR_VIEW)
{
  Load (file);
}

GameView::GameView (VideoSystem* vid,ID player)
         : viewPlayer (player), video(vid),
           state (-1,-1,0),curPrior(PR_VIEW)
{
  owner->AddActive (id(),PR_VIEW);
}

GameView::~GameView ()
{
  owner->DelActive (id());
}

