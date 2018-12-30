#ifndef __TYPE_SINGLE_GAME__
#define __TYPE_SINGLE_GAME__

/////////////////////////Для SigleGame///////////////////////////////////////

enum  SingleGameMessageId
{
  LOAD_GAME = SINGLE_GAME_LIMIT,
  SAVE_GAME,
  SFX_VOLUME,
  MUSIC_VOLUME,
  PAUSE_SINGLE_GAME,
  RELOAD_SINGLE
};

/////////////////////////////////////////////////////////////////////////////
//////Структура - запрос на сохранение
/////////////////////////////////////////////////////////////////////////////
struct   GameSaveMessage: public GameMessage
{
                GameSaveMessage (const char* x)
                {
                  id = SAVE_GAME;
                  strcpy (name,x);
                }

                char    name [128];   //Имя для сохранения
};

/////////////////////////////////////////////////////////////////////////////
//////Структура - запрос на загрузку
/////////////////////////////////////////////////////////////////////////////
struct   GameLoadMessage: public GameMessage
{
                GameLoadMessage (const char* x)
                {
                  id = LOAD_GAME;
                  strcpy (name,x);
                }

                char    name [128];   //Имя для загрузки
};

/////////////////////////////////////////////////////////////////////////////
/////SFX Volume Message (max = 63)
/////////////////////////////////////////////////////////////////////////////
struct  SFXVolumeMessage: public GameMessage
{
        SFXVolumeMessage (int vol): volume(vol) { id = SFX_VOLUME; }

        int volume;     //Уровень громкости
};

/////////////////////////////////////////////////////////////////////////////
/////Music Volume Message (max = 63)
/////////////////////////////////////////////////////////////////////////////
struct  MusicVolumeMessage: public GameMessage
{
        MusicVolumeMessage (int vol): volume(vol) { id = MUSIC_VOLUME; }

        int volume;     //Уровень громкости
};

/////////////////////////////////////////////////////////////////////////////
////Пауза
/////////////////////////////////////////////////////////////////////////////
struct  PauseSingleGameMessage: public GameMessage
{
        PauseSingleGameMessage (BOOL x) : state (x)
         { id = PAUSE_SINGLE_GAME;}

        BOOL    state;
};

/////////////////////////////////////////////////////////////////////////////
////Перезагрузка при загрузке
/////////////////////////////////////////////////////////////////////////////
struct  ReloadSingleOk: public GameMessage
{
        ReloadSingleOk () { id = RELOAD_SINGLE; }
};

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения одноигровки
/////////////////////////////////////////////////////////////////////////////
struct  SingleGameTag: public GameObjectTag
{
                    SingleGameTag () { tag = CLIENT_TAG; }

  char               levelFile [128]; //name
  ID                 player;          //single player ID
  ID                 face;            //interface for GameSystem

  BOOL               isPause;
  int                fragCount;
};

#endif