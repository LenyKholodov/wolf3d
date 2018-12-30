#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <picture.h>
#include <sbinfile.h>
#include <cbinfile.h>

#include <debug.h>

enum  LineType {
      SPRITE_HEADER = 1,
      END,
      COMMENT,
      DIR,
      CACHE_SPRITE_FILE,
      TEXTURES_LIST,
      MAP_HEADER,

      };

struct  Param
{
        LineType        type;           //type of string-line
        char            name[128];      //name (file)
};

char*           NextLine        (BinFile*);
BOOL            ProcessLine     (char*,Param&);
char*           NextWord        (char*,int& pos);

int             Process             (BinFile*);

void            SaveSprite          (const char*,GameSprite3D*);
void            MakeCache             (const char*,char* files[],int count);

char            tempBuffer   [1024];
char*           temp1        = tempBuffer;
char*           temp2        = tempBuffer+512;

char            destDirName  [128];
char*           cacheName = "default";

stdBinFile              file;
cacheBinFile            cfile;
int                     line;

#define                 NO_ERROR 0

int     main(int argc,char* argv[])
{
  dout<<endl<<"UNT Game Studio. Sprite compile utility v 1.0"<<endl;
  if (argc<2 || argc>3)
  {
    dout<<"list.txt"<<endl;
    return 1;
  }
  if (!file.open(argv[1],BinFile::OPEN_RO))
  {
    dout<<"List file not found"<<endl;
    return 1;
  }
  else if (cfile.open(file,DEFAULT_CACHE_SIZE))
  {
    int l;
    if (l = Process(&cfile))
    {
      dout<<"Error in line "<<l<<endl;
      return 1;
    }
    return 0;
  }
  else
  {
    int l;
    if (l = Process(&file))
    {
      dout<<"Error in line "<<l<<endl;
      return 1;
    }
    return 0;
  }

  return 0;
}

void    SaveSprite (const char* name,GameSprite3D* s3D)
{
  char    dest [256];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,name);

  dout<<"Save sprite: "<<dest<<endl;

  s3D->Save (dest);
}

void    MakeCache    (const char* name,char* files[],int count)
{
  TextCache* wsf = new TextCache(files,count,1,SPRITE_SIZE);

  char dest [256];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,name);
  strcat (dest,".wsc");

  cout<<"Save Sprites Cache "<<dest<<endl;
  wsf->Save(dest);

  delete wsf;
}

int     Process (BinFile* fPtr)
{
  fPtr->reset();

  Param         param;
  char          destFileName [128];
  char*         cacheList      [MAX_SPRITES];
  int           cacheCount     = 0;
  GameSprite3D* s3D          = NULL;

  for (char* curLine=NextLine(fPtr);curLine;curLine=NextLine(fPtr))
  {
        if (!ProcessLine (curLine,param))
           return line;

        switch (param.type)
        {
                case DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case CACHE_FILE:
                {
                        strcpy (cacheName,param.name);
                        break;
                }
                case SPRITE_HEADER:
                {
                        strcpy (destFileName,param.name);
                        strcat (destFileName,".wsf");

                        param.index = cacheCount;

                        if ((s3D = ProcessSpriteHeader(fPtr,param))==NULL)
                           return line;
                        else
                        {
                          SaveSprite (destFileName,s3D);

                                //Prepare
                          char dest [256];

                          strcpy (dest,destDirName);
                          strcat (dest,"/");
                          strcat (dest,destFileName);

                          cacheList[cacheCount] = new char [strlen(dest)+1];
                          strcpy (cacheList[cacheCount++],dest);

                          delete s3D;
                        }

                        break;
                }
                case FACE_HEADER:   return line;
                case IMAGE_HEADER:  return line;
                case END:           return line;
                case COMMENT:       continue;

        }
  }

  MakeCache (cacheName,cacheList,cacheCount);

  for (int i=0;i<cacheCount;i++)
      delete cacheList[i];

  return NO_ERROR;
}

void            ClearSHeaderList (Sprite3D** list,int count)
{
  for (int i=0;i<count;i++)
      delete list[i];
}

GameSprite3D*   ProcessSpriteHeader (BinFile* fPtr,const Param& pIn)
{
  Sprite3D**      list = new Sprite3D* [pIn.num];
  Param           param;
  int             count = 0;

  dout<<"Create sprite header: #"<<pIn.index<<" "<<
        pIn.name<<" num = "<<pIn.num<<endl;

  for (char* curLine=NextLine(fPtr);curLine;curLine=NextLine(fPtr))
  {
        if (!ProcessLine (curLine,param))
           return NULL;

        switch (param.type)
        {
                case FACE_HEADER:
                {
                        if ((list [param.index] = ProcessSpriteFace(fPtr,param)) == NULL)
                        {
                           delete list;
                           return NULL;
                        }

                        count++;

                        break;
                }
                case CACHE_FILE:
                case SPRITE_HEADER: return NULL;
                case IMAGE_HEADER:  return NULL;
                case DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case COMMENT:       continue;
                case END:
                {
                        if (count!=pIn.num || count == 0)
                        {
                          dout<<"Error in num sprite faces"<<endl;
                          delete list;
                          return NULL;
                        }

                        GameSprite3D*   s3D = makeGameSprite3D(list,pIn.num);

                        ClearSHeaderList (list,count);
                        delete list;

                        return s3D;
                }

        }

  }

  delete list;
  return NULL;
}

void            ClearFHeaderList (SimpleSprite** list,int count)
{
  for (int i=0;i<count;i++)
      delete list[i];
}

Sprite3D*       ProcessSpriteFace(BinFile* fPtr,const Param& pIn)
{
  SimpleSprite**  list = new SimpleSprite* [pIn.num];

  Param           param;
  int             count = 0;

  dout<<"Create sprite face: #"<<pIn.index<<" num_images = "<<pIn.num<<endl;

  for (char* curLine=NextLine(fPtr);curLine;curLine=NextLine(fPtr))
  {
        if (!ProcessLine (curLine,param))
           return NULL;

        switch (param.type)
        {
                case IMAGE_HEADER:
                {
                        char dest [512];

                        strcpy(dest,destDirName);
                        strcat(dest,"/");
                        strcat(dest,param.name);
                        strcpy(param.name,dest);

                        if ((list [param.index] = ProcessSpriteImage(fPtr,param)) == NULL)
                        {
                           delete list;
                           return NULL;
                        }
                        count++;

                        break;
                }
                case DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case CACHE_FILE:
                case FACE_HEADER:   return NULL;
                case SPRITE_HEADER: return NULL;
                case COMMENT:       continue;
                case END:
                {
                        if (count!=pIn.num || count==0)
                        {
                          dout<<"Error in num sprite face images"<<endl;
                          delete list;
                          return NULL;
                        }

                        Sprite3D*   s3D = makeSprite3D(list,count);

                        ClearFHeaderList (list,count);
                        delete list;

                        return s3D;
                }

        }

  }

  delete list;
  return NULL;
}

SimpleSprite*   ProcessSpriteImage (BinFile* fPtr,const Param& pIn)
{
  cPicture      picture;

  char          name [128];

  strcpy (name,pIn.name);
  strcat (name,".bmp");

  dout<<"Create sprite plane: #"<<pIn.index<<" "<<name<<endl;

  if (!picture.load(name))
  {
    dout<<"Error open simple image"<<endl;
    return NULL;
  }

  return makeSimpleSprite((tImage*)picture.GetImage(),pIn.z_color);
}

char*   NextLine (BinFile* fPtr)
{
  if (fPtr->eof())
     return NULL;

  int i = 0;
  for (char simb = fPtr->getc();simb!=10 && !fPtr->eof();temp1[i++]=simb,simb=fPtr->getc());

  temp1 [i] = 0; //end of line

  line++;
  return temp1;
}

char*   NextWord (char* str,int& pos)
{
  for (char simb = str[pos++];!isalnum(simb);simb=str[pos++]);

  int i = 0;
  if (simb!=10)
    for (;isalnum(simb);temp2[i++]=simb,simb=str[pos++]);

  temp2 [i] = 0; //end of line

  return temp2;
}

BOOL            ProcessLine     (char* str,Param& param)
{
        const int         typeCount = 6;
        const char*       types [typeCount] = {"SPRITE","FACE","IMAGE",
                                               "DIR","END","CACHE"};
        const LineType    realTypes [typeCount] = {SPRITE_HEADER,FACE_HEADER,
                                                   IMAGE_HEADER,DIR,END,CACHE_FILE};

        char            list [100][128];
        int             count = 0;
        int             tLen  = strlen(str);

        for (int pos = 0;pos<tLen;)
            strcpy (list[count++],NextWord(str,pos));

        if (count == 0)
           return FALSE;

                //Find type
        for (int j=0;j<typeCount;j++)
            if (!strcmp(types[j],list[0]))
               break;

        if (j != typeCount)
           param.type = realTypes [j];
        else
           return FALSE;

        switch (param.type)
        {
                case SPRITE_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  strcpy(param.name,list[1]);
                  param.num = atoi (list[2]);

                  break;
                }
                case FACE_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  param.index = atoi(list[1]);
                  param.num   = atoi(list[2]);

                  break;
                }
                case IMAGE_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  strcpy(param.name,list[1]);
                  param.index   = atoi(list[2]);
                  param.z_color = atoi(list[3]);

                  break;
                }
                case END: return TRUE;
                case DIR:
                {
                  if (count<2)
                     return FALSE;

                  strcpy(param.name,list[1]);

                  break;
                }
                case CACHE_FILE:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
        }

  return TRUE;
}