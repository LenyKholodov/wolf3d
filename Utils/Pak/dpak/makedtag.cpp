#include <dirpak.h>

void main(int argc,char* argv[])
{
  if(argc!=3)
  {        
    cout<<"DirPak info utility\nUSAGE: name.lmp \"info string\"\n";
    return;
  }
  tLumpTag tag;
  tDirPakInfoLump lmp;
  BinFile* bf=new stdBinFile(argv[1],BinFile::OPEN_CR);
  if(IsBad(bf))
  {
    delete bf;
    cout<<"Error: can't create output tag file\n";
  }
  tag.tag=DirPakInfoTag;
  tag.size=sizeof(tDirPakInfoLump)+strlen(argv[2]);
  bf->write(&tag,sizeof(tag));
  lmp.ver=1;
  lmp.size=0;
  lmp.crc=0;
  lmp.ioff=0;
  bf->write(&lmp,sizeof(lmp)-1);
  bf->write(argv[2],strlen(argv[2])+1);
  delete bf;
}