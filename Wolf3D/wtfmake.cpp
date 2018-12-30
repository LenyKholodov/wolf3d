#include <iostream.h>
#include <ctype.h>
#include <string.h>
#include <types.h>
#include <sbinfile.h>

#include "textures.h"

#define MAX_TEXTURES    4096

stdBinFile      file;

void   NextWord(char* dest)
{
  char* tmp = dest;
  while (!file.eof() && ((*tmp=file.getc())==13 || *tmp==10 || *tmp==' '));
  tmp++;
  for (; !file.eof() && (*tmp=file.getc())!=13 && *tmp!=10 && *tmp!=' ';tmp++);
  *tmp = 0;
}

int    main(int argc,char* argv[])
{
  cout<<endl<<"UNT Game Studio. Textures compile utility v 1.0"<<endl;
  cout<<"Make-file for WTF. Max textures = "<<MAX_TEXTURES<<endl;
  if (argc<2)
  {
    cout<<"list.txt dest.wtf"<<endl;
    return 1;
  }

  if (!file.open(argv[1],BinFile::OPEN_RO))
  {
    cout<<"File not found: "<<argv[1]<<endl;
    return 1;
  }
  cout<<"Prepare textures list..."<<endl;

  char*  list[MAX_TEXTURES];
  long   count = 0;
  char   data[128];

  while (!file.eof())
  {
    NextWord(data);
    list[count] = new char[strlen(data)+1];
    strcpy(list[count++],data);
  }
  if (count == 0)
  {
    cout<<"List must have textures"<<endl;
    cout<<"List"<<endl;
    cout<<"\t\tfile1.mmt\n\t\tfile2.mmt\n\t\t...\n\t\tfilen.mmt"<<endl;
    return 1;
  }
  cout<<"Find "<<count<<" textures. Make WTF "<<endl;

  TextCache* wtf = new TextCache(list,count,1,TEXT_SIZE);
  cout<<"Save WTF in "<<argv[2]<<endl;
  wtf->Save(argv[2]);

  file.close();
  for (int i=0;i<count;i++)
      delete list[i];
  delete wtf;

  return 0;
}