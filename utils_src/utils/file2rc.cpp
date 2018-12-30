#include <fstream.h>
#include <sbinfile.h>

void main(int argc,char* argv[])
{
  if(argc<2)
    return;
  char cname[100];
  if(argc==2)
  {
    strcpy(cname,argv[1]);
    char* s=strrchr(cname,'.');
    if(s==NULL)
      s=cname+strlen(cname);
    strcpy(s,".cpp"); 
  }
  else
    strcpy(cname,argv[2]);
  ofstream ofs(cname,ios::text | ios::out);
  BinFile* r=new stdBinFile(argv[1],BinFile::OPEN_RO);
  if(IsGood(r))
  {
    uchar buffer[16];
    for(int len=sizeof(buffer);len==sizeof(buffer);)
    {
      len=r->read(buffer,sizeof(buffer));
      for(int i=0;i<len;i++)
        ofs<<((unsigned int)buffer[i])<<",\t";
      ofs<<endl;
    }
  }
  else
    cout<<"Error! Can't open file "<<argv[1]<<" for read\n";
  delete r;
}
