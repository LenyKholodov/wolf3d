#define DEBUG
#include <dirpak.h>
#include <string.h>
#include <io.h>

char buffer[65536];

void main(int argc,char* argv[])
{
  if(argc<2)
  {
    cout<<"usage: makeflmp [/p] <input-name> <output-name>\n";
    return;
  }
  BOOL packed=!stricmp(argv[1],"/P");
  BinFile* r=new stdBinFile(argv[packed?2:1],BinFile::OPEN_RO);
  if(IsBad(r))
  {
    delete r;
    cout<<"Error: can't open input file\n";
  }
  char lmpname[100];
  if((packed && argc==3) || (!packed && argc==2))
  {
    strcpy(lmpname,argv[packed?2:1]);
    char* s=strrchr(lmpname,'.');
    if(s==NULL)
      s=lmpname+strlen(lmpname);
    strcpy(s,".lmp"); 
  }
  else
    strcpy(lmpname,argv[packed?3:2]);
  BinFile* bf=new stdBinFile(lmpname,BinFile::OPEN_CR);
  if(IsBad(bf))
  {
    delete bf;
    delete r;
    cout<<"Error: can't create output .lmp- file\n";
  }
  tLumpTag tag;
  tFileInfoTag fit;
//  bf->seek(sizeof(tag)+sizeof(fit));
  bf->write(&tag,sizeof(tag));
  bf->write(&fit,sizeof(fit));

  BinFile *abin=NULL,*out=bf;
  if(packed)
  {
    abin=new aBinFile(bf,sizeof(fit)+sizeof(tag),0,BinFile::can_all);
    if(IsBad(out=new pakBinFile(abin)))
    {
      delete out;
      delete abin;
      delete bf;
      unlink(argv[3]);
      cout<<"Error: can't create packed file\n";
      return;
    }
  }

  cout<<"Creating .lmp file.";

  for(;!r->eof();out->write(buffer,r->read(buffer,sizeof(buffer))))
   cout<<".";
  delete r;
  cout<<"Ok";
  if(packed)
  {
    delete out;
    delete abin;
    tag.tag=PakFileTag;
  }
  else
  {
    tag.tag=FileTag;
  }

  tag.size=bf->size()-sizeof(tag);
  strncpy(fit.name,argv[packed?2:1],_MAX_PATH);
  fit.ver=1;
  fit.crc=0xFFFFFFFF;

  // calc crc & size

  bf->seek(sizeof(tag)+sizeof(fit));

  for(bf->seek(sizeof(tag)+sizeof(fit));!bf->eof();)
    fit.crc=CRC32(buffer,bf->read(buffer,sizeof(buffer)),fit.crc);

  (*bf)[0].write(&tag,sizeof(tag));
  bf->write(&fit,sizeof(fit));

  delete bf;
}