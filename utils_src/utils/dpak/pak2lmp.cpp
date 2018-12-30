#include <dirpak.h>
#include <sbinfile.h>
#include <string.h>
#include <io.h>

char buffer[65536];

void main(int argc,char* argv[])
{
  cout<<"Starting pak2lmp ver 1.00\n";
  if(argc<2)
  {
    cout<<"usage: pak2lmp <pak-name>\n";
    return;
  }
  BinFile* r=new stdBinFile(argv[1],BinFile::OPEN_RO);
  if(IsBad(r))
  {
    cout<<"Error: can't open input file\n";
  }
  else
  {
    BinFile* out;
    for(long i=100000001;!r->eof();i++)
    {
      tLumpTag tag;
      if(r->read(&tag,sizeof(tag))!=sizeof(tag))
      {
        debug("Error: Tag corrupted\n");
        break;
      }
      char s[256];
      if(IsBad(out=new stdBinFile(strcat(ltoa(i,s,10),".lmp")+1,BinFile::OPEN_CR)))
      {
        cout<<"Error: can't create output file "<<s+1<<endl;
	delete out;
        break;
      }
      out->write(&tag,sizeof(tag));
      for(int l=tag.size;l>sizeof(buffer);l-=sizeof(buffer))
        out->write(buffer,r->read(buffer,sizeof(buffer)));
      out->write(buffer,r->read(buffer,l));
      delete out;
    }
  }
  delete r;
}