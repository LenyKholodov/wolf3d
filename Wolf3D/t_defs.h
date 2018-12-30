#ifndef __TYPE_GAME_DEFS__
#define __TYPE_GAME_DEFS__

                //Global defines

#define __NO_PROCESS__
//#define __NO_OBJECT_MESSAGE__
//#define __LOAD_GAME_FULL_INFO__

#include <types.h>
#include <fixed.h>
#include <time.h>
#include <ipx.h>

#include "t_id.h"
#include "lscreen.h"

typedef int16   PoolIndex;
typedef void*   ArgPtr;

const   int     GAME_CLASTER_SIZE = 1024;
const   int     NO_OBJECT         = -1;
const   int     END_OF_LIST       = -1;

const   int     DEFAULT_ATTR      = 0;

static  const   ulong   END_OF_TAG      = *((ulong*)"END ");
static  const   ulong   SERVER_TAG      = *((ulong*)"SERV");
static  const   ulong   CLIENT_TAG      = *((ulong*)"CLNT");

const   int     MAX_TYPES         = 256;
const   int     MAX_OBJECTS       = 1024;
const   int     MAX_TABLES        = 512;
const   int     MAX_SLOTS         = 0x100;
const   int     QUEUE_SIZE        = 0x10000;
const   int     MAX_MESSAGES      = 8092;
const   int     INPUT_QUEUE_SIZE  = 16384;
const   int     MAX_RING_SIZE     = 256;
const   int     MAX_KEYS          = 0x80*2;

const   int     GAME_MESSAGE_MAX_SIZE  = BUFFER_SIZE-20;

const   int     SEQUENCE_VALUE    = 0xF;
const   int     MAX_CLIENTS       = 0xF;

const   int     MAX_WEAPONS       = 0xF;
const   int     MAX_FREE_PAUSE    = 8;

const   int     MIN_VOLUME_LIMIT  = 32;

        //animation

const   int     ANIM_NUM          = 16;          //Кратно двойки!
const   int     ANIM_MASK         = ANIM_NUM-1;
const   int     ANIM_DEFAULT_FACE = -1;
const   int     END_OF_ANIM       = 255;

        //teleport

const   int     TELEP_ANIM_NUM    = 16;

        //priority

const   int            PRIOR_MASK = 255;

extern volatile int    PR_DOOR;
extern volatile int    PR_WEAP;
extern volatile int    PR_SHOT;
extern volatile int    PR_INPUT;
extern volatile int    PR_VIEW;
extern volatile int    PR_PLAYER;
extern volatile int    PR_MONSTR;
extern volatile int    PR_SERVER;
extern volatile int    PR_CLIENT;
extern volatile int    PR_TELEP;
extern volatile int    PR_ANIMATION;
extern volatile int    PR_MULTI;

        //DOOR

const   int     DOOR_WAIT_TIME    = 15*0xF;
const   int     DOOR_CROSS_STATE  = 32;

        //PLAYER

const   int     PLAYER_STEP       = FX_ONE>>2;
const   int     PLAYER_TURN       = ANGLE_1*3;
const   int     PLAYER_MAX_ACTIVES= 256;
const   int     PLAYER_BOXES      = 16;
const   int     PLAYER_SHOTGUN    = 130;
const   int     PLAYER_DIE        = 12;

const   int     PLAYER_HEALTH     = 100;
const   int     PLAYER_PROTECT    = 75;

const   int     PLAYER_CHANGE_FACE_TIME         = 30;   //out

                //WEAPON

const   Fixed   WEAPON_UNDANGER_DIST   = FX_ONE;
const   int     SHOT_CHANGE_FACE_TIME  = 1;

                //MONSTR

const   Fixed   SEARCH_DIST            = int2fixed (32);
const   Fixed   MAX_ATACK_DIST         = int2fixed (32);
const   int     MAX_TARGET_IQ          = 15;
const   int     MONSTR_CHANGE_TIME     = 2;
const   Angle   MONSTR_TURN            = ANGLE_45;

const   int     IQ_MAX_PROBS           = 0x10;
const   int     VOLUME_QUAD            = 30;

const   int     MAX_ITEMS              = 0x1F;

                //SINGLE

const   int     IFACE_COUNT            = 16;
const   int     FACE_NO_VALUE          = -32768;
static
const   char*   CONSOLE_FONT_NAME      = "fonts/font.fnt";
extern  BOOL    MUSIC_PLAY;
extern  BOOL    SOUND_ON;

                //USER

const   int     USER_WEAPONS           = 0x10;
const   int     MAX_WEAPON_MOVE        = 16;
const   int     WEAPON_MOVE_MASK       = MAX_WEAPON_MOVE-1;
const   int     WEAPON_MOVE_TIME       = CLK_TCK>>3;

                //View

const   int     MAX_VIEW_TIME          = CLK_TCK>>4;
const   int     MIN_VIEW_TIME          = CLK_TCK>>5;
const   int     DEFAULT_KLIP_MODE      = VESA320x200x16;

                //Sound

const   int     MAX_CHANNELS           = 16;
const   int     VOLUME_SHIFT           = 8;
const   int     MAX_VOLUME             = (1l<<VOLUME_SHIFT)-1;
const   int     DEF_SFX_VOLUME         = 85;
const   int     DEF_MUSIC_VOLUME       = 85;
extern  int     SOUND_CACHE_SIZE;

                //Light

const   int     MAX_WALL_LIGHT         = 256;
const   int     LIGHT_ANGLE_STEP       = ANGLE_360/MAX_WALL_LIGHT;

                //Multiplayer

const   int      MULTI_MAX              = 16;
const   int      REQUEST_TIME           = CLK_TCK*2;
const   int      MAX_NET_SEQUENCE       = 16384;
const   clock_t  NET_VIEW_REFRESH       = CLK_TCK>>5;
const   clock_t  NET_NO_PACKET_REQUEST  = CLK_TCK>>2;
const   int      NET_ANSWER_COUNT       = 10;
const   int      NET_IN_QUEUE_SIZE      = 4096;
const   clock_t  NET_REQUEST_TIME       = CLK_TCK<<2;
extern  clock_t  NET_REANSWER_W;
extern  clock_t  NET_REANSWER_R;
extern  socket_t SERVER_WRITE, SERVER_READ,CLIENT_WRITE,CLIENT_READ;

static  const char* DEFAULT_PLAYER = "game/play.plr";

enum
{
  GAME_SYSTEM_LIMIT     = 0,
  GAME_PLAYER_LIMIT     = GAME_SYSTEM_LIMIT+0x3F,
  DOOR_LIMIT            = GAME_PLAYER_LIMIT+0x3F,
  TRIGER_LIMIT          = DOOR_LIMIT,
  INPUT_LIMIT           = DOOR_LIMIT+0xF,
  SINGLE_GAME_LIMIT     = INPUT_LIMIT+0x1F,
  SUBJECT_LIMIT         = SINGLE_GAME_LIMIT+0x1F,
  SHOT_LIMIT            = SUBJECT_LIMIT + 0xF,
  WEAPON_LIMIT          = SHOT_LIMIT + 0xF,
  GAME_BOX_LIMIT        = WEAPON_LIMIT + 0xF,
  USER_PLAYER_LIMIT     = GAME_BOX_LIMIT + 0xF,
  GAME_VIEW_LIMIT       = USER_PLAYER_LIMIT + 0xFF,
  TELEPORT_LIMIT        = GAME_VIEW_LIMIT + 0x1F,
  MULTI_LIMIT           = 10000
};

                //Формат загрузочной таблицы

enum
{
  LM_LEVEL_FILE = 0,
  LM_OPERATION_ID,
  LM_LEVEL_NAME,
  LM_CURRENT_LOAD,
};

static tPoint          mesPoint [MAX_LOAD_MESSAGES] =
{
        {50,10},
        {50,20},
        {50,30},
        {50,55},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0}
};

#endif