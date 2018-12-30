#include "g_mac.h"
#include "g_input.h"
#include "fin.h"
#include "level.h"

#include "t_defs.h"
#include "defs.h"

#include <keyboard.h>
#include <console/comdefs.h>
#include <ctype.h>

extern BOOL firstVGA;

void    InitCommand ()
{
  dout<<"Starting..."<<endl;
  dout<<"Allocate varpool...";
  if(!command.vpool.resize(256))
  {
    dout<<"no memory"<<endl;
    delete fin;
  }
  dout<<"Ok"<<endl;
  dout<<"Allocate parstack...";
  if(command.pstack.maxsize(100)!=100)
  {
    dout<<"no memory"<<endl;
    delete fin;
  }
  dout<<"Ok"<<endl;
  command.SetStream(&dout);
}

void    DefaultCommand()
{
  dout<<"Default commands...";

  command.vpool.setf("exit",CmdExit);
  command.vpool.setf("help",CmdHelp);
  command.vpool.setf("list",CmdList);
  command.vpool.setf("cls",CmdCls);
  command.vpool.setf("set",CmdSet);
  command.vpool.setf("kill",CmdKill);
  command.vpool.setf("alias",CmdAlias);
  command.vpool.setf("dump",CmdDump);

        //Your commands
  command.vpool.setf("bind",CmdBind);

  command.vpool.setf("add_forward",add_GoForward);
  command.vpool.setf("sub_forward",sub_GoForward);
  command.vpool.setf("add_back",add_GoBack);
  command.vpool.setf("sub_back",sub_GoBack);
  command.vpool.setf("add_left",add_TurnLeft);
  command.vpool.setf("sub_left",sub_TurnLeft);
  command.vpool.setf("add_right",add_TurnRight);
  command.vpool.setf("sub_right",sub_TurnRight);
  command.vpool.setf("add_moveleft",add_StrafeLeft);
  command.vpool.setf("sub_moveleft",sub_StrafeLeft);
  command.vpool.setf("add_moveright",add_StrafeRight);
  command.vpool.setf("sub_moveright",sub_StrafeRight);
  command.vpool.setf("add_attack",add_Fire);
  command.vpool.setf("sub_attack",sub_Fire);
  command.vpool.setf("mousemove",MSMove);
  command.vpool.setf("invprev",PrevWeapon);
  command.vpool.setf("invnext",NextWeapon);
  command.vpool.setf("slot",SelectWeapon);
  command.vpool.setf("use",PressObject);

  command.vpool.setf("sfxvolume",SfxVolume);
  command.vpool.setf("musicvolume",MusicVolume);

  command.vpool.setf("qsave",QuickSave);
  command.vpool.setf("qload",QuickLoad);
  command.vpool.setf("save",Save);
  command.vpool.setf("load",Load);
  command.vpool.setf("map",LoadLevel);
  command.vpool.setf("pause",Pause);
  command.vpool.setf("exit",Exit);
  command.vpool.setf("add_pause",add_Pause);
  command.vpool.setf("sub_pause",sub_Pause);

  command.vpool.setf("vid_mode",VID_ChangeScreen);
  command.vpool.setf("vid_picmip",VID_PicMip);
  command.vpool.setf("sight",PresentSight);
  command.vpool.setf("cd",CDCmd);

  command.vpool.setf("slist",Request);
  command.vpool.setf("connect",ConnectCmd);
  command.vpool.setf("disconnect",DisconnectCmd);

  command.vpool.link ("text_cache_size",(long*)&TEXT_CACHE_SIZE);
  command.vpool.link ("sprite_cache_size",(long*)&SPRITE_CACHE_SIZE);
  command.vpool.link ("sprlist_cache_size",(long*)&SPRLIST_CACHE_SIZE);
  command.vpool.link ("game_cache_size",(long*)&GAME_OBJ_CACHE_SIZE);
  command.vpool.link ("text_size",(long*)&TEXT_SIZE);
  command.vpool.link ("sprite_size",(long*)&SPRITE_SIZE);
  command.vpool.link ("sprite_list_size",(long*)&SPRITE_LIST_SIZE);
  command.vpool.link ("loadspeed",(long*)&LOAD_RESOURCE_DELAY);
  command.vpool.link ("klip",(long*)&KLIP_PLAY);
  command.vpool.link ("music",(long*)&MUSIC_PLAY);
  command.vpool.link ("sound",(long*)&SOUND_ON);
  command.vpool.link ("snd_cache_size",(long*)&SOUND_CACHE_SIZE);
  command.vpool.link ("net_reanswer_w",(long*)&NET_REANSWER_W);
  command.vpool.link ("net_reanswer_r",(long*)&NET_REANSWER_R);
  command.vpool.link ("r_focus",(double*)&NORM_FOCUS);
  command.vpool.link ("r_fow",(double*)&NORM_VIEW_ANGLE);
  command.vpool.link ("r_wall_scale",(double*)&NORM_WALL_HEIGHT);
  command.vpool.link ("r_detail_angle",(double*)&NORM_DETAIL_ANGLE);
  command.vpool.link ("r_wall_feel",(double*)&NORM_WALL_FEEL);
  command.vpool.link ("r_pack_cache_size",(long*)&PACK_CACHE_SIZE);
  command.vpool.link ("r_use_vga",(long*)&firstVGA);

  dout<<"Ok"<<endl;
}

void    LoadAutoexec (const char* name)
{
  char  buf [1024];

  dout<<"Load autoexec...";
  ifstream ifs(name,ios::text | ios::in);
  if(ifs)
  {
    for(;ifs;) //screen->refresh()
    {
      ifs.getline(buf,1024);
      command(buf);
    }
    ifs.close();
    dout<<"Ok"<<endl;
  }
  else
    dout<<"failed"<<endl;
}

static char temp2 [1024];

char*   NextWord (char* str,int& pos)
{
  for (char simb = str[pos++];!isalnum(simb) && simb != '_' && simb != '"' &&
       simb != '-';
       simb=str[pos++]);

  int i         = 0;
  int len       = strlen (str);

  if (simb!=10 && simb != '"') //2???
    for (;isalnum(simb) || simb=='_' || simb=='.' || simb =='-' || simb =='+';
          temp2[i++]=simb,simb=str[pos++]);

  if (simb == '"' && pos<len)
     for (simb=str[pos++];
          isalnum (simb) || simb =='_' || simb=='.' || simb == ' '
          || simb=='/' || simb=='\\' || simb ==':' || simb == '-'
          || simb=='+';
          temp2[i++]=simb,simb=str[pos++]);

  temp2 [i] = 0; //end of line

  return temp2;
}


void    CmdBind   ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()!=2)
  {
    os<<"Usage: bind 'KEY_NAME/MOUSE_EVENT' 'BIND_MACROS' "<<endl;
    return;
  }

  int   pos      = 0;
  char  str1[128],
        str2 [128];

  strcpy (str1,NextWord(parm[0].value.zs,pos));
  strcpy (str2,NextWord(parm[0].value.zs,pos));

  char* event = str1;
  char* mac   = str2;

  BOOL  up_down   = mac[0] == '+';

  if (up_down) mac++;

  for (int i=0;i<0x80;i++)
      if (!strcmp(event,Keyboard::GetKeyName(i)))
         break;

  if (i==0x80)
     for (;i<0x80+0xF;i++)
       if (!strcmp(event,MSNames[i-0x80]))
          break;

  if (i==0x80+0xF)
  {
    os<<"Undefined key/event '"<<event<<"'"<<endl;
    return;
  }

  InputTranslator::UnBind (i);

  if (up_down)
  {
    char  down_str [128],
          up_str   [128];

    strcpy (down_str,"add_");
    strcpy (up_str,"sub_");
    strcat (down_str,mac);
    strcat (up_str,mac);

    InputTranslator::Bind   (i,InputTranslator::DOWN,down_str);
    InputTranslator::Bind   (i,InputTranslator::UP,up_str);
  }
  else
    InputTranslator::Bind   (i,InputTranslator::DOWN,mac);
}

void            add_GoForward   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (ADD_MOVE_FORWARD);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            sub_GoForward   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (SUB_MOVE_FORWARD);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            add_GoBack   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (ADD_MOVE_BACK);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            sub_GoBack   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (SUB_MOVE_BACK);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            add_TurnLeft   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (ADD_TURN_LEFT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            sub_TurnLeft   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (SUB_TURN_LEFT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            add_TurnRight   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (ADD_TURN_RIGHT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            sub_TurnRight   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (SUB_TURN_RIGHT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            add_StrafeLeft   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (ADD_STRAFE_LEFT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            sub_StrafeLeft   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (SUB_STRAFE_LEFT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            add_StrafeRight   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (ADD_STRAFE_RIGHT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            sub_StrafeRight   ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (SUB_STRAFE_RIGHT);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            add_Fire        ( VarPool&, ParmStack&, ostream&)
{
  PlayerShotOn  message;

  Send (&message,sizeof(PlayerShotOn),GAME_PLAYER);
}

void            sub_Fire        ( VarPool&, ParmStack&, ostream&)
{
  PlayerShotOff  message;

  Send (&message,sizeof(PlayerShotOff),GAME_PLAYER);
}

void            PressObject     ( VarPool&, ParmStack&, ostream&)
{
  PlayerMessage   playerMessage (PRESS);
  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            SelectWeapon    ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size() < 1)
  {
    os<<"slot <number>"<<endl;
  }

  UserSelectWeaponMessage message (parm[0].value.i-1);
  Send (&message,sizeof(message),GAME_PLAYER);
}

void            NextWeapon      ( VarPool&, ParmStack&, ostream&)
{
  UserNextWeaponMessage message;
  Send (&message,sizeof(message),GAME_PLAYER);
}

void            PrevWeapon      ( VarPool&, ParmStack&, ostream&)
{
  UserPrevWeaponMessage message;
  Send (&message,sizeof(message),GAME_PLAYER);
}

void            MSMove    ( VarPool&, ParmStack& parm, ostream&)
{
  PlayerMessage   playerMessage (PLAYER_MOVE);

  playerMessage.dist  = (-parm[1].value.i<<10);
  playerMessage.angle = parm[0].value.i<<4;

  Send (&playerMessage,sizeof(playerMessage),GAME_PLAYER);
}

void            SfxVolume       ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size() < 1)
  {
    os << "sfxvolume <volume>"<<endl;
    return;
  }

  SFXVolumeMessage message (parm[0].value.i);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void       MusicVolume       ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size() < 1)
  {
    os << "musicvolume <volume>"<<endl;
    return;
  }

  MusicVolumeMessage message (parm[0].value.i);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            QuickSave       ( VarPool&, ParmStack&, ostream&)
{
  GameSaveMessage message ("save/quick.sav");

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            QuickLoad       ( VarPool&, ParmStack&, ostream&)
{
  GameLoadMessage message ("save/quick.sav");

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            Save       ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()!=1)
  {
    os<<"save <save_game> "<<endl;
    return;
  }

  int  pos = 0;
  char dest [128];

  strcpy (dest,"save/");
  strcat (dest,NextWord(parm[0].value.zs,pos));
  strcat (dest,".sav");

  os<<"Save game to '"<<dest<<"'"<<endl;

  GameSaveMessage message (dest);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            Load       ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()!=1)
  {
    os<<"load <save_game> "<<endl;
    return;
  }

  int  pos  = 0;
  char dest [128];

  strcpy (dest,"save/");
  strcat (dest,NextWord(parm[0].value.zs,pos));
  strcat (dest,".sav");

  os<<"Load game from '"<<dest<<"'"<<endl;

  GameLoadMessage message (dest);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            VID_ChangeScreen    ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size() < 1)
  {
    os<<"vid_mode <mode_index>"<<endl;
    return;
  }

  ChangeScreen (parm[0].value.i);

  UserRefreshMessage message;

  Send (&message,sizeof(message),GAME_PLAYER);
}

void            VID_PicMip       ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<1)
  {
    os<<"vid_picmip <detail_level>"<<endl;
    return;
  }

  ViewPicMipMessage picmip (parm[0].value.i);

  Send (&picmip,sizeof(picmip),SINGLE_GAME);

  UserRefreshMessage message;

  Send (&message,sizeof(message),GAME_PLAYER);
}

void            PresentSight     ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size() < 1)
  {
    os<<"sight <on/off>"<<endl;
    return;
  }

  int  flag = -1;

  if       (!strcmp("on",parm[0].value.zs))  flag=0;
  else if  (!strcmp("off",parm[0].value.zs)) flag=1;

  if (flag == -1)
     return ;

  ViewChangeItemMessage message (IM_SIGHT,flag);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            Pause           ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size() < 1)
  {
    os<<"pause <on/off>"<<endl;
    return;
  }

  int  flag = -1;

  if       (!strcmp("on",parm[0].value.zs))  flag=1;
  else if  (!strcmp("off",parm[0].value.zs)) flag=0;

  if (flag == -1)
     return ;

  PauseSingleGameMessage message (flag);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            add_Pause     ( VarPool&, ParmStack& parm, ostream& os)
{
  PauseSingleGameMessage message (TRUE);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            sub_Pause     ( VarPool&, ParmStack& parm, ostream& os)
{
  PauseSingleGameMessage message (FALSE);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            Exit            ( VarPool&, ParmStack&, ostream& os)
{
  os<<"Ok! System destruct"<<endl;
  delete fin;
}

void            LoadLevel       ( VarPool& var, ParmStack& parm, ostream& os)
{
  if (parm.size()<1)
  {
    os<<"map <'map_name.lev'> <x_coord> <y_coord>"<<endl;
    return;
  }

  int   pos = 0;
  char  dest [128];
  char  dir  [128];
  char  name [128];

  if (!var.gets("MAPS_DIR",dir))
  {
    os<<"Write in your 'autoexec.cfg' set MAPS_DIR 'MAPS_DIR_NAME'"<<endl;
  }

  strcpy (name,NextWord(parm[0].value.zs,pos));

  strcpy (dest,dir);
  strcat (dest,"/");
  strcat (dest,name);
  strcat (dest,"/");
  strcat (dest,name);
  strcat (dest,".lev");

  PortalChangeLevelMessage  message (dest,
                                     parm[1].value.i,
                                     parm[2].value.i);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void    CDCmd     ( VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size ()<1)
  {
    os<<"cd <command:prev, next, pause, continue, eject, load, lock, ulock, drive>"<<endl;
    return;
  }

  static  int   track = 0;

  char          cmd [128];
  int           pos = 0;
  cdromDevice*  cd  = GetCDROM();

  if (cd == NULL)
  {
    os<<"CD-ROM not initialized"<<endl;
    return;
  }

  strcpy (cmd,NextWord(parm[0].value.zs,pos));

  if       (!strcmp (cmd,"pause"))      cd->pause();
  else if  (!strcmp (cmd,"continue"))   cd->resume();
  else if  (!strcmp (cmd,"eject"))      cd->eject();
  else if  (!strcmp (cmd,"load"))       cd->load();
  else if  (!strcmp (cmd,"lock"))       cd->lock(TRUE);
  else if  (!strcmp (cmd,"unlock"))     cd->lock(FALSE);
  else if  (!strcmp (cmd,"drive"))
  {
    if (parm.size()<2)
    {
      os<<"cd drive <cdromDrive: D, E, F, G, ...>"<<endl;
      return;
    }
    cd->CurrentDrive(parm[1].value.zs[0]);
  }
  else if  (!strcmp (cmd,"prev"))
  {
    track--;
    if(track<cd->loaudio())
      track=cd->loaudio();
    cd->play(track);
  }
  else if  (!strcmp (cmd,"next"))
  {
    track++;
    if(track<cd->loaudio())
      track=cd->loaudio();
    cd->play(track);
  }
  else  os<<"Wrong command: '"<<cmd<<"'"<<endl;
}

void            Request          ( VarPool&, ParmStack&, ostream& os)
{
  addrnode_t addr;
  memset (addr.node,0xFF,6);
  GetServerInfoMessage message (addr);

  Send (&message,sizeof(message),SINGLE_GAME);
}

void            ConnectCmd       ( VarPool&, ParmStack& parm,ostream& os)
{
  if (parm.size ()<1)
  {
    os<<"connect <server_number>"<<endl;
    return;
  }

  int index = parm[0].value.i;

  if (index<0 || index>=8)
  {
    os<<"server number must be in span (0;7)"<<endl;
    return;
  }

  addrnode_t test;
  memset (test.node,0xFF,6);

  if (!memcmp(test.node,servers[index].node,6))
  {
    os<<"server #"<<index<<" is empty"<<endl;
    return;
  }

  ConnectToServerMessage        connect;

  memcpy (connect.addr.node,servers[index].node,6);

  Send (&connect,sizeof(connect),SINGLE_GAME);
}

void            DisconnectCmd       ( VarPool&, ParmStack& parm,ostream& os)
{
  HaltSystemMessage        disconnect;

  Send (&disconnect,sizeof(disconnect),SINGLE_GAME);
}
