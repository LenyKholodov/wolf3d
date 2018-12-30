#include "g_types.h"

int          SOUND_CACHE_SIZE    = 128000;
int          TEXT_SIZE           = 0x10000;
int          SPRITE_SIZE         = 500000;
int          SPRITE_LIST_SIZE    = 450000;

socket_t     SERVER_WRITE        = 0x4513;
socket_t     SERVER_READ         = 0x4514;
socket_t     CLIENT_WRITE        = SERVER_READ;
socket_t     CLIENT_READ         = SERVER_WRITE;

clock_t      NET_REANSWER_W      = CLK_TCK<<3;
clock_t      NET_REANSWER_R      = CLK_TCK<<4;

BOOL         SOUND_ON            = TRUE;

double NORM_FOCUS          = 2.0;
double NORM_VIEW_ANGLE     = 1.0;
double NORM_WALL_HEIGHT    = 3.0; //3.0
double NORM_DETAIL_ANGLE   = 0;
double NORM_WALL_FEEL      = 12.0;

long PACK_CACHE_SIZE = 1024*1024;

addrnode_t   servers [8];