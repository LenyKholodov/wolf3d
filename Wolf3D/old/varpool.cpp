#include <command.h>

uint VarPool::insert(const char* name,long crcn,uint owner)
{
  uint i=AllocUnit();
  if(i<mCount)
  {
    mPool[i].owner=owner;
    mPool[i].crc=crcn;
    strncpy(mPool[i].name,name,NAME_SIZE);
    mPool[i].name[NAME_SIZE-1]='\0';
    if(owner<mCount)
    {
      if(mPool[owner].crc>=crcn)
      {
        mPool[i].left=mPool[owner].left;
    	mPool[i].right=NOTHING;
	mPool[owner].left=i;
      }      
      else
      {
        mPool[i].right=mPool[owner].right;
    	mPool[i].left=NOTHING;
	mPool[owner].right=i;
      }
    }
    else
    {
      mFirstVar=i;
      mPool[i].left=mPool[i].right=NOTHING;
    }
  }
  return i;
}

long VarPool::crc(const char* s) const
{
  long c=0;
  for(int i=0;i<5 && s[i];c=(c<<6)|(s[i++]&0x0000003F));
  return c;
}

void VarPool::restoreFreeChunks(void)
{
  if(mCount==0)
    return;
  uint last=NOTHING;
  for(uint i=mCount;i--;)
    if(mPool[i].type==VT_FREE)
    {
      if(last<mCount)
	mPool[last].owner=i;
      mPool[i].left=last;
      last=i;
    }
  if((mFirstFree=last)<mCount)
    mPool[mFirstFree].owner=NOTHING;
}

int VarPool::sort_function(const void *a,const void *b)
{
   return ((VarUnit*)a)->crc-((VarUnit*)b)->crc;
}

uint VarPool::makethree(uint start,uint end,uint owner)
{
  if(end<start || end>=mCount)
    return NOTHING;
  uint i=(end+start)/2;
  mPool[i].owner=owner;
  mPool[i].left=makethree(start,i-1,i);
  mPool[i].right=makethree(i+1,end,i);
  return i;
}

void VarPool::pack(void)
{
  // "¥à¥£®­ï¥¬" ¢á¥ ¥«¥¬¥­âë ¢ ­ ç «®
  uint fi=0,vi=mCount;
  for(;;)
  {
    for(;mPool[fi].type!=VT_FREE && fi<vi;fi++);
    if(fi==vi)
      break;
    for(;mPool[--vi].type==VT_FREE && fi<vi;);
    if(fi==vi)
      break;
    memcpy(mPool+fi,mPool+vi,unitsize());
    mPool[vi].type=VT_FREE;
  }
  // ã¯®àï¤®ç¨¢ ¥¬ æ¥¯ì á¢®¡®¤­ëå í«¥¬¥­â®¢
  restoreFreeChunks();
  // á®àâ¨à®¢ª  ¬ áá¨¢  ¯¥à¥¬¥­­ëå
  qsort(mPool,varcount(),unitsize(),sort_function);
  // á®§¤ ­¨¥ ¤¥à¥¢ 
  mFirstVar=makethree(0,varcount()-1,NOTHING);
}

uint VarPool::AllocUnit(void)
{
  if(mFirstFree<mCount)
  {
    uint i=mFirstFree;
    if((mFirstFree=mPool[i].left)<mCount)
      mPool[mFirstFree].owner=NOTHING;
    mVarCount++;
    return i;
  }
  return NOTHING;
}


BOOL VarPool::FreeUnit(uint i)
{
  if(i<mCount)
  {
    if(mPool[i].type!=VT_FREE)
    {
      if(mFirstFree<mCount)
      {
	mPool[mFirstFree].owner=i;
	mPool[i].left=mFirstFree;
      }
      else
        mPool[i].left=NOTHING;
      mPool[i].owner=NOTHING;
      mPool[i].type=VT_FREE;
      mFirstFree=i;
      mVarCount--;
    }
    return TRUE;
  }
  return FALSE;
}

void VarPool::clear(void)
{
  for(uint i=0;i<mCount;mPool[i++].type=VT_FREE);
  restoreFreeChunks();
  mFirstVar=NOTHING;
  mVarCount=0;
}

void VarPool::close(void)
{
  if(mCount)
  {
    ::free(mPool);
    mPool=NULL; 
    mCount=mVarCount=0;
    mFirstFree=mFirstVar=NOTHING;
  }
}

BOOL VarPool::resize(uint count)
{
  if(mCount)
  {
    VarUnit* ptr;
    if(count<mCount)
    {
      if(count<varcount())
        return FALSE;
      pack();
    }
    if((ptr=(VarUnit*)realloc(mPool,count*unitsize()))==NULL)
      return FALSE;
    mPool=ptr;
    for(uint i=mCount;i<count;i++)
      mPool[i].type=VT_FREE;
    mCount=count;
    restoreFreeChunks();    
  }  
  else
  {
    if((mPool=(VarUnit*)malloc(unitsize()*count))==NULL)
      return FALSE;
    mCount=count;
    clear();
  }
  return TRUE;
}

void VarPool::dump(ostream& os)
{
#define OUT_INDEX(i) if(i==NOTHING) os<<"NULL"; else os<<i
  os<<"VarPool. Full Info:"<<endl
    <<"Count(used)="<<mCount<<'('<<mVarCount
    <<"); unitsize="<<unitsize()
    <<"; size="<<mCount*unitsize()<<endl;
  os<<"Pool addr: "<<mPool<<endl;
  os<<"First var block: ";
  OUT_INDEX(mFirstVar);
  os<<"; First free block: ";
  OUT_INDEX(mFirstFree);
  os<<endl<<"Block map:"<<endl;
  for(uint i=0;i<mCount;i++)
  {
    os<<'#'<<i<<'\t';
    OUT_INDEX(mPool[i].owner);
    os<<'\t';
    OUT_INDEX(mPool[i].left);
    os<<'\t';
    if(mPool[i].type!=VT_FREE)
      OUT_INDEX(mPool[i].right);
    else  
      os<<'-';
    os<<'\t'<<hex<<setw(8)<<setfill('0')<<mPool[i].crc<<dec<<'\t';
    switch(mPool[i].type)
    {
      case VT_FREE:  os<<"[FREE]"; break;
      case VT_INT:   os<<"   int["<<mPool[i].name<<"]="<<mPool[i].value.i;    break;
      case VT_FLOAT: os<<" float["<<mPool[i].name<<"]="<<mPool[i].value.f;    break;
      case VT_STRING:os<<"string["<<mPool[i].name<<"]="<<mPool[i].value.s;    break;
      case VT_PINT:  os<<"  *int["<<mPool[i].name<<'='<<*(mPool[i].value.pi); break;
      case VT_PFLOAT:os<<"*float["<<mPool[i].name<<'='<<*(mPool[i].value.pf); break;
      case VT_MACRO: os<<" macro["<<mPool[i].name<<"]="<<mPool[i].value.m;    break;
      case VT_FUNC:  os<<"  func["<<mPool[i].name<<"]="<<mPool[i].value.func; break;
      default:       os<<"!undefined "<<mPool[i].name;       		      break;
    }
    os<<endl;
  }
#undef OUT_INDEX
}

void VarPool::listChain(uint i,ostream& os,ViewAttr attr)
{
  if(i<mCount)
  {
    if(mPool[i].type==VT_FREE)
    {
      os<<"ERROR! Variable pool is corrupted!"<<endl;
      return;
    }
    listChain(mPool[i].left,os,attr);
    switch(mPool[i].type)
    {
      case VT_INT:   if(attr.var) os<<mPool[i].name<<'='<<mPool[i].value.i<<endl; break;
      case VT_FLOAT: if(attr.var) os<<mPool[i].name<<'='<<mPool[i].value.f<<endl; break;
      case VT_STRING:if(attr.var) os<<mPool[i].name<<"=\""<<mPool[i].value.s<<'"'<<endl; break;
      case VT_PINT:  if(attr.var) os<<mPool[i].name<<'='<<*(mPool[i].value.pi)<<endl; break;
      case VT_PFLOAT:if(attr.var) os<<mPool[i].name<<'='<<*(mPool[i].value.pf)<<endl; break;
      case VT_MACRO: if(attr.macro) os<<"macro{"<<mPool[i].name<<"}=\""<<mPool[i].value.m<<'"'<<endl; break;
      case VT_FUNC: if(attr.func) os<<"func{"<<mPool[i].name<<"}=\""<<mPool[i].value.func<<'"'<<endl; break;    
      default:       os<<mPool[i].name<<"=?"<<endl;
      		     break;
    }
    listChain(mPool[i].right,os,attr);
  }
}

void VarPool::list(ostream& os,ViewAttr attr)
{
  os<<"Variable list:"<<endl;
  listChain(mFirstVar,os,attr);
  os<<".end"<<endl;
}

void VarPool::listChain2(uint i,ostream& os)
{
  if(i<mCount)
  {
    char s[VALUE_SIZE+10];
    listChain2(mPool[i].left,os);
    switch(mPool[i].type)
    {
      case VT_INT:   os<<"set "<<mPool[i].name<<' '<<mPool[i].value.i<<endl; break;
      case VT_FLOAT: os<<"set "<<mPool[i].name<<' '<<mPool[i].value.f<<endl; break;
      case VT_STRING:os<<"set "<<mPool[i].name<<" \"";
      	             str2cstr(s,mPool[i].value.s,sizeof(s));
      	   	     os<<s<<'"'<<endl; break;
      case VT_PINT:  os<<"set "<<mPool[i].name<<' '<<*(mPool[i].value.pi)<<endl; break;
      case VT_PFLOAT:os<<"set "<<mPool[i].name<<' '<<*(mPool[i].value.pf)<<endl; break;
      case VT_MACRO: os<<"alias "<<mPool[i].name<<" \""; 
      	             str2cstr(s,mPool[i].value.m,sizeof(s));
      	   	     os<<s<<'"'<<endl; break;
    }
    listChain2(mPool[i].right,os);
  }
}


BOOL VarPool::find(const char* name,long& crcn,uint& i)
{
  crcn=crc(name);
  for(i=mFirstVar;i<mCount;)
  {
    if(crcn==mPool[i].crc)
    {
      if(strcmp(name,mPool[i].name)==0)
        return TRUE;
    }
    if(crcn<=mPool[i].crc)
    {
      if(mPool[i].left<mCount)
      	i=mPool[i].left;
      else
	return FALSE;	
    }
    else
      if(crcn>mPool[i].crc)
      {
        if(mPool[i].right<mCount)
      	  i=mPool[i].right;
        else
	  return FALSE;	
      }
  }
  return FALSE;
}


BOOL VarPool::set(const char* name,const char* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
    switch(mPool[i].type)
    {
      case VT_INT:   mPool[i].value.i=atol(value); 	  break;
      case VT_FLOAT: mPool[i].value.f=atof(value); 	  break;
      case VT_STRING:strncpy(mPool[i].value.s,value,VALUE_SIZE);
		     mPool[i].value.s[VALUE_SIZE-1]='\0';
		     break;
MZ       ÿÿ  ¸       @                                   ø   º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       ×Ëš“ªmÉ“ªmÉ“ªmÉ¶cÉ‡ªmÉ{µgÉËªmÉ“ªmÉ‘ªmÉ ˆHÉ‘ªmÉ“ªlÉªmÉñµ~É˜ªmÉíˆqÉ’ªmÉíˆpÉ‚ªmÉ{µfÉŸªmÉ+¬kÉ’ªmÉRich“ªmÉ                PE  L T¶t:        à   €  @     ‡        @                      Ğ                                      |¶ Œ      ˆ¥                                                                                                     .text   ¦w     €                   `.rdata  1     @                @  @.data   ˜C   Ğ  0   Ğ             @  À.rsrc   ˆ¥     °                @  @                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ì˜   S‹œ$¨   U‹¬$¨   VW‹¼$¸   h    WSj Uj ÿˆA PÿÈ‘A ‹ğ…öu!h   WSPUPÿÈ‘A ‹ğ…öu_^][Ä˜   ÃD$PjVÿ A ‹T$¹   3À|$,ó«‹L$‹„$¬   ‰T$4T$j RT$4‰L$@‹RPÇD$<|   ÇD$@   Ç„$¤   @   ÿQ…Àt_^]3À[Ä˜   Ã‹D$j j j j VPè   ƒÄVÿTA ‹D$_^][Ä˜   Ãìœ   S‹œ$¨   …ÛV„ä   ‹´$¨   …ö„Õ   ‹UWVÿPlj ÿ(A ‹ø…ÿuh@ĞA ÿ„A SWÿ$A L$QjSÿ A ‹œ$À   …Ûu‹\$‹¬$Ä   …íu‹l$ ‹D$0PVÇD$8|   ÇD$<   ÿRX‹T$RVÿQD…À‰D$u<‹„$¼   ‹Œ$¸   ‹T$8h  Ì USP‹D$LQ‹L$$WRPj j Qÿ,A ‹D$‹PVÿRhWÿPA ‹D$_]^[Äœ   Ã^¸@ €[Äœ   Ãì<  SVW3ö|$I‹ÆƒÇÁøƒà‹ÈÁá+È¸“$I’÷éÑÁú‹ÂÁèĞ‹ÆÁøƒàˆWû‹ÈÁá+È¸“$I’÷éÑ‹ÆÁú‹ÊƒàÁéÑ‹ÈÁá+È¸VUUUˆWü÷é‹ÂÁèĞFˆWıÆGş ş   |‰‹´$P  …ö„-  jVj ÿ˜A …À„š   Pj ÿ¨A Pÿ¬A ‹ğ…öuh\ĞA ÿ„A ‹…ö0t#ƒø(rf‹Nfƒùw‹v …öu	»   Óãë‹Şë3Û…ÛÃ   Ut$L|$Ml$N+ò+úB+êL$L‹ÓŠXƒÀˆŠXüˆ\üŠXûˆ\üÆD(ü ƒÁJuß]é€   j Vÿ¤A ‹ğƒşÿtp‹=´A L$8jQVÿ×T$j(RVÿ×D$Hh   PVÿ×Vÿ€A ƒ|$(t3Òë‹L$fƒùv3Òë‹T$0…Òu	º   Óâ…Ò~D$J‹òŠŠHşˆPşˆƒÀNuğ‹„$L  T$j R‹T$PRjPÿQ‹D$_^[Ä<  Ãì€   S‹œ$Œ   UV‹´$   WƒÏÿƒûÿt;‹L$QVÿPD…Àu,‹T$PPRÿ0A ‹è‹D$Sj j Pÿ8A ‹T$‹RVÿQhë‹l$‹j L$j Qj VÇD$(|   ÿPd=vˆu‹j D$j Pj VÿRd=vˆtè…Àu$‹L$8‹9‹L$hƒù s
º   ÓâJ#ú‹j Vÿ€   ƒûÿt(‹T$RVÿQD…ÀuUPP‹D$Pÿ8A ‹T$‹RVÿQh‹Ç_^][Ä€   Ãƒì‹D$V‹t$PVèíşÿÿ‹ƒÄT$‰D$RjV‰D$ÿQt^ƒÄÃé   ‹|ĞA ¸€  +Á£ûA ÃƒìDSUV‹t$TW‰5 üA ÿA £¤ûA £üA £ ûA £ÌğA ÿœA ‰D$X¡ ûA Pè¹ğ  èë  è–  ½   3ÛUSè   ƒÄ£¨ûA ¹@   3À¿ØğA jó«‰-ğûA ÇD$0   ÇD$4`@ ‰\$8‰\$<‰t$@‰\$D‰\$Hÿ4A L$,‰\$LQ‰D$LÇD$TÜĞA ÿˆ‘A ‹=„‘A SVSSUÿ×PSÿ×PSSh   €ShÜĞA jÿ€‘A ;Ã£œûA u_^]3À[ƒÄDÂ ‹T$dRPÿ|‘A ¡œûA Pÿx‘A ‹œûA Qÿh‘A è?Q  …Àu ‹œûA ¡ üA Shpe@ RhÔĞA Pÿl‘A ë‰-HüA jeVÿt‘A ‹œûA Qè`  ƒÄ…Àuèd  _^]3À[ƒÄDÂ è3  ‹œûA ‹5p‘A SSSD$RPÿÖƒ|$t>‹=ì‘A ‹-ô‘A ‹œûA jSST$QRÿÖ…ÀtD$Pÿ×L$QÿÕëèî  ƒ|$uÎ‹T$Xj Rÿ A èå  ‹D$_^][ƒÄDÂ V‹t$W‹|$ş   wcş   t<‹Æƒèt&ƒètƒèuwj ÿ˜‘A ëm‹D$Pÿ”‘A _3À^Â j ÿ‘A _3À^Â ‹ÈğA ¸   £8üA ‰½ØõA ‰ĞğA ë-‹Æ-