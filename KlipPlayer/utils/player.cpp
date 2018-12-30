#include <video.h>
#include <iostream.h>
#include <string.h>

#include <time.h>

const char* name = "test/klip";
const int   NUM  = 42;

char**      nameTable;
cPicture*   pict [NUM];

void    FormNameTable (const char*,int);
void    InitPicture   (int);
int     LoadPictures  (int,int);
void    UnLoadPicture (int);
void    DeleteTable   (int);
void    DrawPicture   (cPicture*,Screen*);

void    main()
{
  FormNameTable (name,NUM);
  InitPicture(NUM);

  VESAdevice vesa;

  vesa.open ();
  vesa.SetMode (VESA320x200x16);

  Screen* screen = vesa.CurrentScreen();
  Screen* dbuf   = GimmeMirror(screen);

  dbuf->cls();

  int     curPict=0,num = NUM;

  if ((num = LoadPictures   (curPict,num))==0)
  {
    cout<<"No free memory!"<<endl;
    return;
  }

  clock_t start = clock();

  for (int i=0;i<NUM;i++)
  {
    if (pict[i] == NULL)
    {
      if (pict[curPict])
      {
         UnLoadPicture (curPict);
         curPict+=num;
      }

      if ((num = LoadPictures   (curPict,num))==0)
      {
        cout<<"No free memory!"<<endl;
        return;
      }
    }

    DrawPicture (pict[i],dbuf);

    WaitForVsync();

    dbuf->refresh();
  }

  clock_t end = clock();

  cout<<"FPS: "<<(float)NUM/((float)(end-start)/CLK_TCK)<<endl;
  cout<<"Time: "<<(float)(end-start)/CLK_TCK<<endl;

  delete dbuf;

  vesa.close();

  DeleteTable (NUM);
  UnLoadPicture(curPict);
}

void    FormNameTable (const char* name,int num)
{
  int  len = strlen(name)+strlen(".bmp")+4+1;
  char buf [128], dest [128];

  nameTable = new char* [num];

  for (int i=0;i<num;i++)
  {
    nameTable [i] = new char [len];

    itoa (i,buf,10);

    memset (dest,'0',4-strlen(buf));
    strcpy (dest+4-strlen(buf),buf);

    strcpy (nameTable[i],name);
    strcat (nameTable[i],dest);
    strcat (nameTable[i],".bmp");
  }
}

void    DeleteTable (int num)
{
  for (int i=0;i<num;i++)
    delete nameTable[i];

  delete nameTable;
}

void    DrawPicture (cPicture* pPtr,Screen* screen)
{
  tImage* image = pPtr->GetImage();

  screen->PutImage (0,0,image);
}

void    InitPicture   (int num)
{
  for (int i=0;i<num;pict[i++]=NULL);
}

void    UnLoadPicture (int start)
{
  for (int i=start;pict[i];i++)
  {
    delete pict [i];
    pict [i] = NULL;
  }
}

int    LoadPictures  (int start,int num)
{
  for (int i=start;i<num+start;i++)
    if ((pict [i] = new cPicture (nameTable[i]))==NULL)
      return i-start;
//    else
//      ShiftPalette (256,(RGBStruct*)pict[i]->GetPalette(),-2);

  return num;
}
