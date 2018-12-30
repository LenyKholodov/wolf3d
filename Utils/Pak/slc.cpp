#include <cbinfile.h>
#include <mscreen.h>
#include <sbinfile.h>
#include <fstream.h>
#include <console\command.h>

char slcName[_MAX_PATH]="sprites.spl";
char pictureName[_MAX_PATH]="",
     maskName[_MAX_PATH]="";

char cfgname[_MAX_PATH]="autoexec.cfg";

cPicture *picture=NULL,
	 *mpicture=NULL;

void maskNameCmd(VarPool& vp,ParmStack& ps,ostream& os)
{
  if(ps.size()==0 || ps.top().ptype!=PT_STRING)
  {
    os<<"maskName cmd error: to few parameters or bad type of parameter\n";
    return;
  }
  SubStrData& sd=ps.top().value.s;
  strncpy(maskName,sd.data+1,sd.length-2);
  maskName[sd.length-2]='\0';
  delete mpicture;
  mpicture=new cPicture(maskName);
  if(IsBad(mpicture))
    os<<"maskName cmd error: can't load mask file\n";
  if(mpicture->bpp()!=8)
  {
    os<<"maskName cmd error: invalid bpp (only 8bpp supported!)\n";
    delete mpicture;
    mpicture=NULL;
  }
}

void pictureNameCmd(VarPool& vp,ParmStack& ps,ostream& os)
{
  if(ps.size()==0 || ps.top().ptype!=PT_STRING)
  {
    os<<"pictureName cmd error: to few parameters or bad type of parameter\n";
    return;
  }
  SubStrData& sd=ps.top().value.s;
  strncpy(pictureName,sd.data+1,sd.length-2);
  pictureName[sd.length-2]='\0';
  delete picture;
  picture=new cPicture(pictureName);
  if(IsBad(picture))
    os<<"pictureName cmd error: can't load file\n";
  if(picture->bpp()!=8)
  {
    os<<"pictureName cmd error: invalid bpp (only 8bpp supported!)\n";
    delete picture;
    picture=NULL;
  }
}

#pragma pack(1)
typedef struct 
{ 
  long Id; 
  int16 count,bpp,palsize; 
} ImageListHeader;
#pragma pack()

ImageListHeader iHeader;

BinFile* outfile=NULL;
BinFile* routfile=NULL;
int Count=0;

void Finally(void)
{
  delete picture;
  delete mpicture;
  picture=mpicture=NULL;
  if(IsGood(outfile))
  {
    iHeader.count=Count;
    (*outfile)[0].write(&iHeader,sizeof(iHeader));
  }
  delete outfile;
  if(routfile!=outfile)
    delete routfile;
  routfile=outfile=NULL;
}

void SLNameCmd(VarPool& vp,ParmStack& ps,ostream& os)
{
  if(IsBad(picture) || IsBad(mpicture))
  {
    os<<"SListName cmd error: not defined picture & mask of picture\n";
    return;
  }
  if(ps.size()==0 || ps.top().ptype!=PT_STRING)
  {
    os<<"SListName cmd error: to few parameters or bad type of parameter\n";
    return;
  }
  char slname[_MAX_PATH];
  SubStrData& sd=ps.top().value.s;
  strncpy(slname,sd.data+1,sd.length-2);
  slname[sd.length-2]='\0';
  routfile=new stdBinFile(slname,BinFile::OPEN_CR);
  if(IsBad(routfile))
    os<<"SListName cmd error: can't open file\n";
  outfile=new cacheBinFile(routfile,1000000);
  if(IsBad(outfile))
  {
    delete outfile;
    outfile=routfile;
  }
  iHeader.Id=tag2long("SLST");
  iHeader.count=0;
  iHeader.bpp=8;
  iHeader.palsize=picture->GetPaletteSize();
  outfile->write(&iHeader,sizeof(iHeader));
  outfile->puts(0);			// start of palette
  outfile->puts(iHeader.palsize);       // sizeof palette
  outfile->write(picture->GetPalette(),sizeof(RGBStruct)*ulong(iHeader.palsize));
}

void rectCmd(VarPool& vp,ParmStack& ps,ostream& os)
{
  if(IsBad(picture) || IsBad(mpicture))
  {
    os<<"rect cmd error: not defined picture & mask of picture\n";
    return;
  }
  if(IsBad(outfile))
  {
    os<<"rect cmd error: bad outfile\n";
    return;
  }
  if(ps.size()<4)
  {
    os<<"rectCmd error: to few parameters or bad type of parameter\n";
    return;
  }
// подготовка видеоповерхностей
  mScreen8bit screen(picture->GetPicture());
  mScreen8bit mscreen(mpicture->GetPicture());
  char* lbuffer=new char[screen.width()];
  if(IsBad(&screen) || IsBad(&mscreen) || lbuffer==NULL)
  {
    os<<"rect cmd error: other error\n";
    delete lbuffer;
    return;
  }


// выгрузка заголовка
  int x=0,y=0,w=0,h=0,sx=0,sy=0;
  if(ps[0].ptype==PT_INT)
    x=ps[0].value.i;
  if(ps[1].ptype==PT_INT)
    y=ps[1].value.i;
  if(ps[2].ptype==PT_INT)
    w=ps[2].value.i;
  if(ps[3].ptype==PT_INT)
    h=ps[3].value.i;
  if(ps.size()>4 && ps[4].ptype==PT_INT)
    sx=ps[4].value.i;
  if(ps.size()>5 && ps[5].ptype==PT_INT)
    sy=ps[5].value.i;
  debug("Call rectCmd("<<x<<','<<y<<','<<w<<','<<h<<','<<sx<<','<<sy<<")\n");
  tSprite spriteHeader;
  spriteHeader.width=w;
  spriteHeader.height=h;
  spriteHeader.x=sx;
  spriteHeader.y=sy;
  outfile->write(&spriteHeader,sizeof(spriteHeader)-1);
// построчное сканирование и выгрузка
  for(int i=0;i<h;i++)
  {
    debug("\tLine #"<<i<<":\t");
    char* cptr=lbuffer;
    int yl=y+i;
    for(int xl=x;xl<x+w;)
    {
      // анализ цепи
      int j;
      if(mscreen.GetPixel(xl,yl)) // непусто, копируемая цепь
      {
        uint16* controlWord=(uint16*)cptr;
	cptr+=2;
	debug('['<<xl<<',');
	for(j=0;mscreen.GetPixel(xl,yl) && xl<x+w;j++)
	  *cptr++=screen.GetPixel(xl++,yl);
        *controlWord=0x4000+j;        
	debug(xl-1<<']');
      }
      else
      {
       	debug('('<<xl<<',');
        for(j=0;!mscreen.GetPixel(xl,yl) && xl<x+w;j++)
          xl++;
//        if(xl>=x+w) // последний фрагмент не в счёт
//          break;
	uint16* controlWord=(uint16*)cptr;
	cptr+=2;
	*controlWord=j;
	debug(xl-1<<')');
      }      
    }
    outfile->puts(cptr-lbuffer+2);
    if(cptr!=lbuffer)
      outfile->write(lbuffer,cptr-lbuffer);
    debug("\tl="<<(cptr-lbuffer+2)<<endl);
  }
  delete lbuffer;
  Count++;
  debug("rectCMD: end call\n");
}

Command command;

void main(int argc,char* argv[])
{
  cout<<"Sprite list maker. version 0.9"<<endl;
  if(argc==2)
    strcpy(cfgname,argv[1]);
  if(!command.setup(&cout))
    return;
  command.vpool.setf("SpriteList",SLNameCmd);
  command.vpool.setf("Picture",pictureNameCmd);
  command.vpool.setf("MaskPicture",maskNameCmd);
  command.vpool.setf("sprite",rectCmd);
  ifstream ifs(cfgname,ios::text | ios::in);
  if(ifs)
  {
    for(char buffer[200];ifs;command(buffer))
      ifs.getline(buffer,sizeof(buffer));
    ifs.close();
    Finally();    
  }
}