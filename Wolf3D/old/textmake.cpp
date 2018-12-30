#include "textures.h"

#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <ctype.h>

#include <sbinfile.h>
#include <cbinfile.h>

#include <debug.h>

#define  NO_ERROR 0
#define  MAX_TEXT 4096

enum  LineType {
      TEXT_LIST = 1,
      END,
      COMMENT,
      SRC_DIR,
      DEST_DIR,
      CACHE_FILE
      };

struct  Param
{
        LineType        type;           //type of string-line
        char            name[128];      //name (file)
        int             num;            //count of objects
        int             index;          //index in list
};

char            tempBuffer   [1024];
char*           temp1        = tempBuffer;
char*           temp2        = tempBuffer+512;

char            destDirName  [128];
char            srcDirName  [128];
char*           cacheName = "default";

stdBinFile              file;
cacheBinFile            cfile;
int                     line;

char*           NextLine        (BinFile*);
BOOL            ProcessLine     (char*,Param&);
char*           NextWord        (char*,int& pos);

int             Process         (BinFile*);
int             ProcessText     (BinFile*,char* []);
void            MakeCache       (const char*,char* [],int);

int     main(int argc,char* argv[])
{
  dout<<endl<<"UNT Game Studio. Textures compile utility v 1.0"<<endl;
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
  else if (cfile.open(&file,DEFAULT_CACHE_SIZE))
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

void    MakeCache    (const char* name,char* files[],int count)
{
  TextCache* wsf = new TextCache(files,count,1,TEXT_SIZE);

  char dest [256];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,name);
  strcat (dest,".wtf");

  cout<<"Save Sprites Cache "<<dest<<endl;
  wsf->Save(dest);

  delete wsf;
}

int             Process (BinFile* fPtr)
{
  fPtr->reset();

  Param         param;
  char          destFileName   [128];
  char*         cacheList      [MAX_TEXT];
  int           cacheCount     = 0;

  for (char* curLine=NextLine(fPtr);curLine;curLine=NextLine(fPtr))
  {
        if (!ProcessLine (curLine,param))
           return line;

        switch (param.type)
        {
                case DEST_DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case SRC_DIR:
                {
                        strcpy (srcDirName,param.name);
                        break;
                }
                case CACHE_FILE:
                {
                        strcpy (cacheName,param.name);
                        break;
                }
                case TEXT_LIST:
                {
                  if ((cacheCount=ProcessText(fPtr,cacheList))==-1)
                     return line;

                  break;
                }
                case END:           return line;
                case COMMENT:       continue;

        }
  }

  MakeCache (cacheName,cacheList,cacheCount);

  for (int i=0;i<cacheCount;i++)
      delete cacheList[i];

  return NO_ERROR;
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
        const int         typeCount = 5;
        const char*       types [typeCount] = {"LIST","DEST","SRC",
                                               "END","CACHE"};
        const LineType    realTypes [typeCount] = {TEXT_LIST,DEST_DIR,
                                                   SRC_DIR,END,CACHE_FILE};

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
                case TEXT_LIST: return TRUE;
                case END: return TRUE;
                case SRC_DIR:
                case DEST_DIR:
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

int      ProcessText     (BinFile* fPtr,char* names[])
{
  int   pos       = 0;
  int   index     = 0;

//  char* nPal      = "npal.exe";
  char* mmtMake   = "mmtmake.exe";
  char  name    [128];
  char  name1   [128];
  char* argList [3];

  for (char* procStr = NextWord(NextLine(fPtr),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(fPtr),pos),index++)
  {
    strcpy (name,destDirName);
    strcat (name,"/");
    strcat (name,procStr);
    strcat (name,".mmt");

    strcpy (name1,srcDirName);
    strcat (name1,"/");
    strcat (name1,procStr);
    strcat (name1,".bmp");

//    argList [1] = name1;
//    argList [2] = name1;

//    if (spawnv (0,nPal,argList))
//      return -1;

    argList [1] = name1;
    argList [2] = name;

    if (spawnv (0,mmtMake,argList))
      return -1;

    names [index] = new char [strlen(name)+1];

    strcpy (names[index],name);
  }

  return index;
}