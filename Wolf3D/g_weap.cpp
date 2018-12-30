#include "g_weap.h"
#include "g_anim.h"
#include "g_player.h"
#include "g_system.h"
#include "g_load.h"

#include "cross.h"

#include <debug/debug.h>
#include <tables.h>

make<Weapon,WEAPON>             makeWeapon;
make<Shot,SHOT>                 makeShot;

////////////////////////implementatiotn of class Weapon//////////////////////

void            Weapon::Load (BinFile* file)
{
  GameObject::Load (file);

  WeaponTag     tag;

  file->read (&tag,sizeof(tag));

  weapRes = tag.res;
  box     = tag.exempl.box;
  iterator= tag.iterator;
  state   = tag.state;
  shotOff = tag.shotOff;

  if (tag.owner != ID(-1,-1)) callBack = tag.owner;

  last          = clock();
  lastAnim      = clock();

  owner->DelActive (id());
}

void            Weapon::Save (BinFile* file)
{
  GameObject::Save (file);

  WeaponTag     tag;

  tag.res               = weapRes;
  tag.owner             = callBack;
  tag.exempl.box        = box;
  tag.iterator          = iterator;
  tag.state             = state;
  tag.shotOff           = shotOff;

  file->write (&tag,sizeof(tag));
}

BOOL            Weapon::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case WP_SHOT_ON:
    {
      ShotOn ();

      return TRUE;
    }
    case WP_SHOT_OFF:
    {
      ShotOff ();

      return TRUE;
    }
    case TICK:
    {
      if (state != PASSIVE && !shotOff)
      {
        GetPlayerInfoMessage  getinfo (id());

        owner->PutMessage (&getinfo,callBack,sizeof(getinfo));
      }

            //Анимация

      if (state != NO_ANIM && clock()-lastAnim >= faceAnimTime[state])
      {
        int temp = state;

        WeaponChangeFaceMessage message (NextFace (faceTable[state],iterator,temp));

        owner->PutMessage(&message,callBack,sizeof(message),id());

        if (shotOff && temp == NO_ANIM)
        {
          iterator = 0;
          state    = PASSIVE;
          shotOff  = FALSE;
        }

        lastAnim = clock();
      }

      return TRUE;
    }
    case WP_GET_COUNT:
    {
      if (box != ID(-1,-1))
      {
        WeaponChangeCountMessage countChange (box);
        owner->PutMessage (&countChange,mes->callBack,sizeof(countChange),id());
      }

      return TRUE;
    }
    case WP_SET_BOX:
    {
      WeaponSetBoxMessage* message = (WeaponSetBoxMessage*)mes;

      SetBox(message->box);

      return TRUE;
    }
    case WP_GET_INFO:
    {
      WeaponInfoMessage message (weapRes);
      owner->PutMessage (&message,mes->callBack,sizeof(message),id());

      return TRUE;
    }
    case PLAYER_INFO:
    {
      if (state == ACTIVE)
      {
        PlayerInfoMessage*  info = (PlayerInfoMessage*)mes;

        Fire (info->x,info->y,info->angle);
      }

      return TRUE;
    }
    case WP_ANIM_MODE_ON:
    {
      refresh (TRUE);
      return TRUE;
    }
    case WP_ANIM_MODE_OFF:
    {
      refresh (FALSE);
      return TRUE;
    }
    default: return FALSE;
  }
}

void            Weapon::dump (ostream& os) const
{
  os<<"Weapon "<<weapRes.gameBox<<" store "<<weapRes.count;
}

void            Weapon::Fire (Fixed x,Fixed y,Angle angle)
{
  if (clock()-last>=weapRes.speed)
  {
        //check for count

    if (box != ID(-1,-1))
       if (((GameBox*)owner->GetObject (box))->count() == 0)
       {
         WeaponNoPatronMessage  message (id());

         dout<<"generate call back message 'No patron' for "<<callBack<<endl;

         owner->PutMessage (&message,callBack,sizeof(message));

         ShotOff();

         return ;
       }
       else
         ((GameBox*)owner->GetObject (box))->SubStore (1);

     if (box)
     {
       WeaponChangeCountMessage countChange (box);

       owner->PutMessage (&countChange,callBack,sizeof(countChange),id());
     }

        // create shot

    CreateShot shot;

    shot.param().x     = x + mul (WEAPON_UNDANGER_DIST,cos(angle)); //opt
    shot.param().y     = y + mul (WEAPON_UNDANGER_DIST,sin(angle)); //opt
    shot.param().angle = angle;
    shot.resourceClass = weapRes.shot;
    shot.param().owner = callBack;

    owner->CreateObject(SHOT,&shot,callBack);

        //play fire sound

    PlaySoundMessage play (weapRes.a_sound,fixed2int(x),fixed2int(y));

    owner->PutMessage (&play,sizeof(play),callBack);

        //for time delay

    last = clock();
  }
}

void            Weapon::ShotOn()
{
  if (state != NO_ANIM)
  {
    iterator = 0;
    state    = ACTIVE;
    shotOff  = FALSE; //!!

    owner->AddActive (id());
  }
  else
    owner->AddActive (id());
}

void            Weapon::ShotOff ()
{
  if (state != NO_ANIM)
    shotOff = TRUE;
  else
    owner->DelActive (id());
}

void            Weapon::SetBox (ID _box)
{
  if (_box == ID(-1,-1))
     return;

  box = _box;

  if (weapRes.count != 0 )
  {
    ((GameBox*)owner->GetObject(box))->AddStore (weapRes.count);
    weapRes.count = 0;
  }

      //Изменение колличеств патронов

  if (box)
  {
    WeaponChangeCountMessage countChange (box);

    owner->PutMessage (&countChange,callBack,sizeof(countChange),id());
  }
}

void            Weapon::refresh (BOOL toState)
{
  if (toState)
  {
              //Присоединение анимации

    WeaponSetChangeModeOkMessage message (weapRes.sprites,
                                          NextFace (faceTable[PASSIVE],
                                                    iterator=0,
                                                    state=PASSIVE)) ;

    owner->PutMessage (&message,callBack,sizeof(message));

    owner->AddActive (id());

        //Изменение колличеств патронов

    if (box)
    {
      WeaponChangeCountMessage countChange (box);

      owner->PutMessage (&countChange,callBack,sizeof(countChange),id());
    }
  }
  else
  {
          //Отключаем анимацию текущего оружия

    owner->DelActive (id());

    state    = PASSIVE;
    iterator = 0;
  }
}

Weapon::Weapon (ArgPtr ptr)
       : last (clock()), lastAnim (clock()), state (NO_ANIM), iterator (0),
         box (ID(-1,-1)), shotOff (FALSE)
{
  if (ptr == NULL)
     return;

  CreateWeapon* arg = (CreateWeapon*)ptr;

  weapRes = arg->resourceClass;

  if (arg->param().count != 0) weapRes.count = arg->param().count;

  SetBox (arg->param().box);

  faceTable    [PASSIVE] = weapRes.p_anim;
  faceTable    [ACTIVE]  = weapRes.a_anim;
  faceAnimTime [PASSIVE] = weapRes.p_changeTime;
  faceAnimTime [ACTIVE]  = weapRes.a_changeTime;
}

Weapon::Weapon (BinFile* file)
       :  box (ID(-1,-1))
{
  Load (file);

  faceTable    [PASSIVE] = weapRes.p_anim;
  faceTable    [ACTIVE]  = weapRes.a_anim;
  faceAnimTime [PASSIVE] = weapRes.p_changeTime;
  faceAnimTime [ACTIVE]  = weapRes.a_changeTime;
}

Weapon::~Weapon ()
{       }

///////////////////////implementation of class Shot//////////////////////////

void            Shot::Load (BinFile* file)
{
  GameObject::Load (file);

  ShotTag       tag;

  file->read    (&tag,sizeof(tag));

  shotRes   = tag.shot;
  x         = tag.x;
  y         = tag.y;

  state     = tag.state;
  block     = tag.block;
  oldIndex  = tag.oldIndex;
  oldBlock  = tag.oldBlock;
  oldId     = tag.oldId;
  bahIndex  = tag.bahIndex;
  crossIndex= tag.crossIndex;
  dist      = tag.dist;

  if (tag.owner) callBack = tag.owner;

  last      = clock();
}

void            Shot::Save (BinFile* file)
{
  GameObject::Save (file);

  ShotTag       tag;

  tag.shot      = shotRes;
  tag.x         = x;
  tag.y         = y;

  tag.state     = state;
  tag.block     = block;
  tag.oldIndex  = oldIndex;
  tag.oldBlock  = oldBlock;
  tag.oldId     = oldId;
  tag.bahIndex  = bahIndex;
  tag.crossIndex= crossIndex;
  tag.dist      = dist;

  tag.owner     = callBack;

  file->write (&tag,sizeof(tag));
}

BOOL            Shot::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:  bahIndex == -1 ? tick() : bah(bahIndex); return TRUE;
    case TELEPORT_TO:
    {
      TeleportToMessage* message = (TeleportToMessage*)mes;

      x = frac(x) + int2fixed(message->x);
      y = frac(y) + int2fixed(message->y);

      return TRUE;
    }
    case CHANGE_LEVEL:  return TRUE;
    default:    return FALSE;
  }
}

void            Shot::dump (ostream& os) const
{
  os<<"\t\tShot"<<endl;

  os<<"Speed:       "<<(shotRes.speed/65536.0f)<<endl;
  os<<"Max dist:    "<<(shotRes.max_dist/65536.0f)<<endl;
  os<<"Force:       "<<(int)shotRes.force<<endl;
  os<<"Radius:      "<<(int)shotRes.radius<<endl;
  os<<"Sprite #:    "<<shotRes.sprites<<endl;
  os<<"Fly Sound #: "<<shotRes.fly_sound<<endl;
  os<<"Bah Sound #: "<<shotRes.bah_sound<<endl;

  os<<"Position :    X "<<fixed2float(x)<<" Y "<<fixed2float(y)<<endl;
}

void            Shot::Present (int index) const
{
        //Поздравляем всех, кому не повезло

  int ystart = (index>>MAP_SHIFT)    - (shotRes.radius>>1),
      xstart = (index & MAX_X_TILE)  - (shotRes.radius>>1),
      yend   = ystart + shotRes.radius,
      xend   = xstart + shotRes.radius;

  if (ystart<0)                 ystart = 0;
  if (ystart>=MAP_HEIGHT)       ystart = MAP_HEIGHT-1;
  if (xstart<0)                 xstart = 0;
  if (xstart>=MAP_WIDTH)        xstart = MAP_WIDTH-1;

  for (int y=ystart;y<yend;y++)
  {
      int ind = Index (y,xstart);
      for (int x=xstart;x<xend;x++,ind++)
          if (owner->GetBlock (ind)->type==Map::MONSTR)
          {
            if (owner->GetObject(ind)==callBack && !shotRes.back_force)
               continue;   //ВОзможна ли отдача

            PlayerCrossShot cross (shotRes.force,callBack);

            owner->PutMessage (&cross,owner->GetObject(ind),sizeof(cross));
          }
  }

}

void            Shot::bah     (int index)
{
  if (state == 0)
  {
    Present (crossIndex);

    delete this;
    return;
  }
  else if (state == -1)
  {
    if (!owner->IsCross (bahIndex) )    //Если анимация взрыва невозможна
    {
      bahIndex = -1;
      Present (crossIndex);
      delete this;
      return;
    }

    if (owner->GetType(crossIndex) == SHOT)
    {
      state = 0;
      return;
    }

             //ВЗРЫВ.SND

    PlaySoundMessage   snd (shotRes.bah_sound,fixed2int(x),fixed2int(y));

    owner->PutMessage (&snd,sizeof(snd),callBack);

          //Приготовся Дороти!

    int flag                = 1;
    state                   = 0;

    block.type              = Map::SPRITE;
    block.cross             = 0;
    block.id.sprite.z       = SPRITE_CENTER;
    block.id.sprite.id      = shotRes.sprites;
    block.id.sprite.face    = NextFace (shotRes.bah_anim,state,flag);
    block.id.sprite.xfrac   = frac(x)>>12;
    block.id.sprite.yfrac   = frac(y)>>12;
    block.id.sprite.alpha   = angle>>13;
    block.id.sprite.already = 0;

    last                    = clock() + SHOT_CHANGE_FACE_TIME;
  }

  if (clock()-last>=SHOT_CHANGE_FACE_TIME)
  {
    int flag = 1;

    block.id.sprite.face = NextFace (shotRes.bah_anim,state,flag);
    owner->SetBlock (bahIndex,block);

    last = clock();
  }
}

void            Shot::tick    ()        //optimize
{
  CrossIndex index ;
  int        flag;
  int        temp = Index (fixed2int(y),fixed2int(x));

                //sprite animation

  FindCross (x,y,angle,shotRes.speed,owner->GetMap(),index);

  if (index.crossIndex == -1 && dist<shotRes.max_dist)
  {
    x    += mul(cos(angle),shotRes.speed);
    y    += mul(sin(angle),shotRes.speed);
    dist += shotRes.speed;

    block.id.sprite.xfrac   = frac(x)>>12;
    block.id.sprite.yfrac   = frac(y)>>12;
    block.id.sprite.face    = NextFace (shotRes.fly_anim,state,flag);

    owner->SetBlock  (oldIndex,oldBlock);
    owner->SetObject (oldIndex,oldId);

    oldIndex = Index(fixed2int(y),fixed2int(x));
    oldBlock = *owner->GetBlock (oldIndex);
    oldId    = owner->GetObject (oldIndex);

    owner->SetBlock (temp=Index(fixed2int(y),fixed2int(x)),block);
    owner->SetObject(temp,id());
  }
  else if (dist>=shotRes.max_dist)
  {
        //Закончился срок годности

    if (oldIndex!=-1)
    {
      owner->SetBlock  (oldIndex,oldBlock);
      owner->SetObject (oldIndex,oldId);

      oldIndex = -1;
    }

    delete this;
  }
  else
  {
        //Ура - цель достигнута

    if (oldIndex!=-1)
    {
      owner->SetBlock  (oldIndex,oldBlock);
      owner->SetObject (oldIndex,oldId);

      oldIndex = -1;
    }

    crossIndex = index.crossIndex;
    oldBlock   = *owner->GetBlock (oldIndex=bahIndex=index.prevIndex);
    oldId      = owner->GetObject (bahIndex);
    state      = -1;

    if (owner->GetBlock(crossIndex)->type != Map::MONSTR) bah (bahIndex);
    else state = 0;
  }
}

BOOL            Shot::IsCross ()
{
  int _x = fixed2int(x),
      _y = fixed2int(y);

  return owner->GetBlock (Index(_y,_x))->cross;
}

Shot::Shot (ArgPtr ptr)
     :dist (0), state (0), bahIndex (-1), oldId(ID(-1,-1)),
      oldIndex(-1)
{
  if (ptr == NULL)
  {
    dout<<"Can't create shot! Ptr == NULL"<<endl;
    return;
  }

        //как всегда

  CreateShot*   shot = (CreateShot*)ptr;

  shotRes   = shot->resourceClass;
  x         = shot->param().x;
  y         = shot->param().y;
  angle     = shot->param().angle;

  if (shot->param().owner) callBack = shot->param().owner;

                //fly shot sound
  PlaySoundMessage snd(shotRes.fly_sound,fixed2int(x),fixed2int(y));

  owner->PutMessage (&snd,sizeof(snd),callBack);

  if (!owner->IsCross ( crossIndex = Index( fixed2int(y), fixed2int(x) ) ))
  {
     Present (crossIndex);
     delete this;
     return;
  }

        //продумать когда пуля невидима

  block.type              = Map::SPRITE;
  block.cross             = 0;
  block.id.sprite.alpha   = angle>>13;
  block.id.sprite.already = 0;
  block.id.sprite.id      = shotRes.sprites;
  block.id.sprite.face    = 0;
  block.id.sprite.z       = SPRITE_CENTER;

  oldIndex = Index(fixed2int(y),fixed2int(x));
  oldBlock = *owner->GetBlock (oldIndex);
  oldId    = owner->GetObject (oldIndex);

        //in active queue
  owner->AddActive (id());
}

Shot::Shot (BinFile* file)
{
  Load (file);
}

Shot::~Shot ()
{
  if (oldIndex != -1)
  {
     Block* bl = owner->GetBlock (bahIndex);

     if (bl->type==Map::SPRITE && bl->id.sprite.id == shotRes.sprites)
        owner->SetBlock (oldIndex,oldBlock);
     if (oldId == id())
        owner->SetObject(oldIndex,oldId);
  }

  owner->DelActive (id());
}

