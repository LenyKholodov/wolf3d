#include <klip.h>
#include <ctype.h>
#include <mscreen.h>

#include <debug/debug.h>

struct WKFHeader
{
  char          id [4];         //"WKF "
  int           count;          //Колличество кадров
  int           speed;          //Скорость вывода
  char          sound [128];    //Индекс файла с звуковым семплом
  int           pict [];        //Индексы картинок (кадров)
};

void        KlipPlayer::Save     (DirPak* dPak)
{
  if (dPak == NULL) dPak = pak;

  BinFile* file = dPak->open ("info",BinFile::OPEN_RW);

  WKFHeader     header;

  memcpy (header.id,"WKF ",4);
  strcpy (header.sound,soundFile);

  header.count     = count();
  header.speed     = speed();

  file->write (&header,sizeof(WKFHeader));
  file->write (pictIndexTable,sizeof(int)*count());
  file->close ();
}

void        KlipPlayer::Save     (const char* pakName)
{
  DirPak* temp = new DirPak ();

  if (!temp->openDP(pakName,DEFAULT_CACHE_SIZE,FALSE))
  {
    dout<<"Klip player: Error open save file '"<<pakName<<"'"<<endl;
    return;
  }

  Save (temp);

  delete temp;
}

BOOL        KlipPlayer::Load     (DirPak* dPak)
{
  if (dPak == NULL) dPak = pak;
  else if (pak != dPak)
  {
    if (createPak) delete pak;
    pak       = dPak;
    createPak = FALSE;
  }

  BinFile* file = dPak->open ("info",BinFile::OPEN_RO);

  WKFHeader header;

  file->read (&header,sizeof(WKFHeader));

  if (!strcmp(header.id,"WKF "))
  {
    dout<<"Klip player: Wrong file format '";
    for (int i=0;i<4;dout<<header.id[i++]);
    dout<<"'"<<endl;

    return FALSE;
  }

  DoneTables();

  countPictures  = header.count;
  fps            = header.speed;

  strcpy (soundFile,header.sound);

  InitTables();

  file->read (pictIndexTable,sizeof(int)*count());
  file->close();

  nextFrameTime  = clock();

  return TRUE;
}

BOOL        KlipPlayer::Load     (const char* pakName)
{
//  DirPak* temp = new DirPak ();

  if (!pak->openDP(pakName,128*1024))
  {
    dout<<"Klip player: Error open load file '"<<pakName<<"'"<<endl;
    return FALSE;
  }

  BOOL state = Load (pak);

//  delete temp;

  return state;
}

void        KlipPlayer::create   (const char* klpFile,const char* sndFile,int cnt)
{
  if (pak == NULL || cnt>pak->count())
     return;

  DoneTables();

  count(cnt);

  char buf  [128],
       dest [128],
       name [128];
  int  len  = strlen(klpFile);

  for (int i=0;i<len;name[i]=toupper(klpFile[i]),i++);
  name [len] = 0;

  InitTables();

  len             = strlen(klpFile)+strlen(".BMP")+4+1;

  for (i=0;i<count();i++)
  {
        //Form file name

    itoa (i,buf,10);

    memset (dest,'0',4-strlen(buf));
    strcpy (dest+4-strlen(buf),buf);

    strcpy (buf,name);
    strcat (buf,dest);
    strcat (buf,".BMP");

    pictIndexTable [i] = pak->FindEntry (buf);
  }

  len = strlen(sndFile);

  for (i=0;i<len;soundFile[i]=toupper(sndFile[i]),i++);
  soundFile[len] = 0;
}

void        KlipPlayer::DoneTables()
{
  if (pictIndexTable) delete pictIndexTable;
  if (pictTable)
  {
    for (int i=0;i<count();i++)
        if (pictTable [i])
          delete pictTable [i];
    delete pictTable;
  }

  for (int i=0;i<VSCREEN;i++)
  {
      if (screen [i]) delete screen[i];
      screen[i] = NULL;
  }

  pictIndexTable = NULL;
  pictTable      = NULL;
}

void        KlipPlayer::InitTables ()
{
  DoneTables();

  pictIndexTable = new int       [count()];
  pictTable      = new cPicture* [count()];

  memset(pictTable,0,sizeof(cPicture*)*count());
  memset(pictIndexTable,-1,sizeof(int)*count());
}

BOOL        KlipPlayer::SetScreen  (Screen* dest)
{
  for (int i=0;i<VSCREEN;i++)
  {
      if (screen [i]) delete screen[i];
      screen [i] = GimmeMirror (dest);

      if (screen [i] == NULL)
         return FALSE;

      screen [i]->cls();
  }

  return TRUE;
}

cPicture*   KlipPlayer::LoadPicture   (int i)
{
  Flag flag ("LOAD_PICTURE");

  if (i<0 || i>=count())    return NULL;
  if (pictTable [i])       return pictTable [i];

  pictTable [i]        = new cPicture ();
  BinFile*  file       = pak->open(pictIndexTable[i],BinFile::OPEN_RO);

  if (!pictTable [i]->load (file))
  {
    dout<<"Klip player: Error open frame #"<<i<<endl;
    return NULL;
  }
  else
    return pictTable [i];
}

void        KlipPlayer::UnloadPicture (int index)
{
  if (index<0 || index>=count() || pictTable [index] == NULL)
     return;

  delete pictTable [index];
  pictTable [index] = NULL;
}

void        KlipPlayer::UnloadAll  ()
{
  for (int i=0;i<count();i++)
      if (pictTable [i])
      {
        delete pictTable [i];
        pictTable [i] = NULL;
      }
}

BOOL        KlipPlayer::InitSound     (MidasSoundShell* snd)
{
  if (snd==NULL)
  {
     dout<<"Klip player: Error create sound shell"<<endl;
     return FALSE;
  }

  sound = snd;

  if(!sound->open(pak,1))
  {
    dout<<"Klip player: Error load dir pak"<<endl;
    return FALSE;
  }

  strcpy (soundSample.name,soundFile);
  soundSample.loop = TRUE;

  if(!sound->load(&soundSample,1))
  {
    dout<<"Klip player: Error loading sample"<<endl;
    return FALSE;
  }

  return TRUE;
}

void        KlipPlayer::GoMusic       (int volume)
{
  if (sound) sound->play (0,volume,0);
}

void        KlipPlayer::DrawPicture (int index,Screen* scr)
{
  Flag flag ("DRAW_PICTURE");

  if (index<0 || index>=count() || pictTable [index] == NULL)
  {
     dout<<"Klip player: Frame #"<<index<<" not load"<<endl;
     return;
  }

  scr->PutImage (0,0,pictTable[index]->GetImage());
}

void        KlipPlayer::DrawNext ()
{
  if (readScreen!=writeScreen)
     screen [readScreen]->refresh();
  else
    return;

  readScreen = (readScreen+1) & VSCREEN_MASK;
}

void        KlipPlayer::LoadNext ()
{
  Flag flag ("LOAD_NEXT");

  if (readScreen==((writeScreen+1)&VSCREEN_MASK))
     return;

  if (lastLoadPicture!=curPicture)
    UnloadPicture (lastLoadPicture++);

  if (!LoadPicture   (curPicture))     return;

  DrawPicture (curPicture++,screen[writeScreen]);

  writeScreen = (writeScreen+1) & VSCREEN_MASK;
}

//static clock_t lastLoad = 0;

void        KlipPlayer::tick()
{
  if (curPicture == count())
     return;
  else if (curPicture == 0)
  {
    nextFrameTime = clock()+speed();
  }

  if (clock()-last >= speed())
  {
    if (clock() > nextFrameTime)
    {
       nextFrameTime += speed();
       curPicture++;
       return;
    }

    DrawNext();

    nextFrameTime += speed();
    last           = clock();
  }
  else if (speed()-(clock()-last) > lastLoad || readScreen == writeScreen)
  {
    clock_t start = clock();
    LoadNext();
    lastLoad = clock()-start;
  }
}

KlipPlayer::KlipPlayer (DirPak* dPak,const char* klipName,int cnt,const char* sndName,int sp)
          : fps (sp), last(0), curPicture(0), lastLoadPicture (-1),
            pictTable(NULL), pictIndexTable(NULL), countPictures(cnt),
            pak(dPak), sound(NULL),
            readScreen(0), writeScreen(0), createPak (FALSE),
            nextFrameTime (0), lastLoad (0)
{
  for (int i=0;i<VSCREEN;screen[i++] = NULL);

  create (klipName,sndName,cnt);

  strcpy (klipFile,"");
}

KlipPlayer::KlipPlayer (const char* pakName,const char* klipName,int cnt,const char* sndName,int sp)
          : fps (sp), last(0), curPicture(0), lastLoadPicture (-1),
            pictTable(NULL), pictIndexTable(NULL), countPictures(cnt),
            pak(new DirPak()), sound(NULL),
            readScreen(0), writeScreen(0), createPak(TRUE),
            nextFrameTime (0), lastLoad (0)
{
  if (!pak->openDP (pakName,128*1024))
  {
    dout<<"Klip player: Error open DirPak '"<<pakName<<"'"<<endl;
    return;
  }

  for (int i=0;i<VSCREEN;screen[i++] = NULL);

  create (klipName,sndName,cnt);

  strcpy (klipFile,pakName);
}

KlipPlayer::KlipPlayer (const char* name,Screen* scr)
          : fps (0), last(0), curPicture(0), lastLoadPicture (-1),
            pictTable(NULL), pictIndexTable(NULL), countPictures(0),
            pak(new DirPak), sound(NULL),
            readScreen(0), writeScreen(0), createPak(TRUE),
            nextFrameTime (0), lastLoad (0)
{
  for (int i=0;i<VSCREEN;screen[i++] = NULL);

  Load (name);
  SetScreen (scr);

  strcpy (klipFile,name);
}

KlipPlayer::KlipPlayer (DirPak* file,Screen* scr)
          : fps (0), last(0), curPicture(0), lastLoadPicture (-1),
            pictTable(NULL), pictIndexTable(NULL), countPictures(0),
            pak(file), sound(NULL),
            readScreen(0), writeScreen(0), createPak(FALSE),
            nextFrameTime (0), lastLoad (0)
{
  for (int i=0;i<VSCREEN;screen[i++] = NULL);

  Load (file);
  SetScreen (scr);

  strcpy (klipFile,"");
}

KlipPlayer::~KlipPlayer()
{
  pak->closeDP();
  if (createPak) delete pak;

  UnloadAll ();
  DoneTables();
}
