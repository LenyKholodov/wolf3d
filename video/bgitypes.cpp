#include <grtypes.h>
#include <sBinFile.h>
#include <cbinfile.h>

tBGIFont* tBGIFont::load(BinFile* file)
{
  if(IsBad(file))
    return NULL;
        const char RealHead[]={'P','K',8,8};
          char  head[4];
          file->read(head,4);
          if(memcmp(head,RealHead,4))
                return NULL;
          while(file->getc()!=0x1A);
          if(file->eof())
                return NULL;
          tCHRName FontName;
          file->read(&FontName,sizeof(tCHRName));
          while(file->getc()!='+');
          if(file->eof())
                return NULL;
        ulong size=sizeof(tBGIFont)+FontName.FontSize-1;
          tBGIFont* font=(tBGIFont*)malloc(size);
          if(font!=NULL)
        {
                  file->read(font,15);//sizeof(Header)=15
                  file->read(font->Offsets+font->FirstChar,sizeof(uint16)*font->CharsCount);
                  file->read(font->Widths+font->FirstChar,font->CharsCount);
                  file->read(font->data,FontName.FontSize);
                font->FontSize=size;
        }
          return font;
}

tBGIFont*  tBGIFont::load(const char* path)
{
  BinFile *f=new stdBinFile(path,BinFile::OPEN_RO),
          *cache=new cacheBinFile(f,DEFAULT_CACHE_SIZE);
  tBGIFont* font=load(IsGood(cache)?cache:f);
  delete cache;
  delete f;
  return font;
}

int tBGIFont::width(int i) const
{
        if(!in(i))
                return 0;
          uint16*  vec=getdata(i);
          for(int xx=0;((*vec)&0x8080);vec++)
            xx=(*vec) & 0x7F;
          return xx;
}

int     tBGIFont::height(int i) const
{
  if (!in(i))
             return 0;
  uint16*  vec=getdata(i);
  int yy,yMax=0;
  for(int k=0;(vec[k] & 0x8080);k++)
    if(yMax<(yy=(vec[k]>>8) & 0x7F))
                    yMax=yy;
  return yMax;
}

int  tBGIFont::width(const char* s) const
{
  for(int w=0;*s;s++)
          w+=width(*s);
  return w;
}

int  tBGIFont::height(const char* s) const
{
  for(int h=0;*s;s++)
          h=max(h,height(*s));
  return h;
}
