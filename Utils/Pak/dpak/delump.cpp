#include <dirpak.h>
#include <pakbfile.h>
#include <stdio.h>
#include <string.h>
#include <io.h>

char buffer[65536];

void main(int argc,char* argv[])
{
  if(argc<2)
  {
    cout<<"usage: delump <lmp-name>\n";
    return;
  }
  BinFile* r=new stdBinFile(argv[1],BinFile::OPEN_RO);
  if(IsBad(r))
  {
    delete r;
    cout<<"Error: can't open input file\n";
    return;
  }
  cout<<"read tag...";
  tLumpTag tag;
  if(r->read(&tag,sizeof(tag))!=sizeof(tag))
  {
    delete r;
    cout<<"Error: tag of lmp-file corrupted\n";
    return;
  }
  cout<<"Ok\n";
  if(r->size()!=sizeof(tag)+tag.size)
    cout<<"Warning: bad size in header info\n";  
  if(tag.tag==DirPakInfoTag) // directory info
  {
    tDirPakInfoLump* dinf=(tDirPakInfoLump*)malloc(tag.size);
    r->read(dinf,tag.size);
    FILE* f=fopen("dirinfo.txt","wt");
    fprintf(f,"Dirpak info:\nver. %d.%d\nRem: %s\n",hiword(dinf->ver),
    		      		  	      	    loword(dinf->ver),
						    dinf->text);
    fclose(f);
    free(dinf);
  }  
  else
  if(tag.tag==PakFileTag || tag.tag==FileTag)
  {
    // read file info
    tFileInfoTag fileinfo;
    r->read(&fileinfo,sizeof(tFileInfoTag));
    BinFile* bf=new stdBinFile(fileinfo.name,BinFile::OPEN_CR);
    if(bf==NULL || bf->bad())
      cout<<"Error: can't create file output\n";
    else
    { 
      BinFile* f=tag.tag==PakFileTag?
      	       		new unpakBinLump(r,r->tell(),r->size()-r->tell(),-1):
			new aBinFile(r,r->tell(),r->size()-r->tell());
      if(IsBad(f))
        cout<<"Error: bad archive or lmp-file\n";
      else
      {
        cout<<"unpacking("<<fileinfo.name<<")";
        for(;!f->eof();bf->write(buffer,f->read(buffer,sizeof(buffer))))
          cout<<".";
        cout<<"Ok\n";
      }
      delete f;
    }
    delete bf; 
  }
  else
  {
    char buffer[5]="    ";
    *((long*)buffer)=tag.tag;
    cout<<"Error: unexpected tag-type('"<<buffer<<"')\n";
  }

  delete r;
}