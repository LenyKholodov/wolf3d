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
  // "��ॣ��塞" �� �������� � ��砫�
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
  // 㯮�冷稢��� 楯� ᢮������ ������⮢
  restoreFreeChunks();
  // ���஢�� ���ᨢ� ��६�����
  qsort(mPool,varcount(),unitsize(),sort_function);
  // ᮧ����� ��ॢ�
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
MZ�       ��  �       @                                   �   � �	�!�L�!This program cannot be run in DOS mode.
$       �����mɓ�mɓ�m��cɇ�m�{�g�˪mɓ�mɑ�mɠ�Hɑ�mɓ�l��m��~ɘ�m��qɒ�m��pɂ�m�{�fɟ�m�+�kɒ�m�Rich��m�                PE  L T�t:        �   �  @     �     �   @                      �                                      |� �      ��                                                                          �                           .text   �w     �                   `.rdata  �1   �  @   �             @  @.data   �C   �  0   �             @  �.rsrc   ��     �                @  @                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ��   S��$�   U��$�   VW��$�   h    WSj Uj ���A P�ȑA ����u!h   WSPUP�ȑA ����u_^][�Ę   ÍD$PjV� �A �T$�   3��|$,�L$��$�   �T$4�T$j R�T$4�L$@�RP�D$<|   �D$@   Ǆ$�   @   �Q��t_^]3�[�Ę   ËD$j j j j VP�   ��V�T�A �D$_^][�Ę   Ð������   S��$�   ��V��   ��$�   ����   �UWV�Plj �(�A ����uh@�A ���A SW�$�A �L$QjS� �A ��$�   ��u�\$��$�   ��u�l$ ��D$0PV�D$8|   �D$<   �RX��T$RV�QD���D$u<��$�   ��$�   �T$8h  � USP�D$LQ�L$$WRPj j Q�,�A �D$�PV�RhW�P�A �D$_]^[�Ĝ   �^�@ �[�Ĝ   Ð��������<  SVW3��|$I�ƃ���������+ȸ�$I����������Ћ������W�����+ȸ�$I���ы����ʃ���ы���+ȸVUUU�W�������F�W��G� ��   |���$P  ���-  jVj ���A ����   Pj ���A P���A ����uh\�A ���A ����0t#��(rf�Nf��w�v ��u	�   ������3ۅ���   U�t$L�|$M�l$N+�+��B+�L$L�ӊX����X��\��X��\��D(� ��Ju�]�   j V���A �����tp�=��A �L$8jQV�׍T$j(RV�׍D$Hh   PV��V���A �|$(t3���L$f��v3���T$0��u	�   ���~�D$J���H��P����Nu���$L  �T$j R��T$PRjP�Q�D$_^[��<  Ð������   S��$�   UV��$�   W������t;��L$QV�PD��u,�T$PPR�0�A ��D$Sj j P�8�A �T$�RV�Qh��l$�j �L$j Qj V�D$(|   �Pd=v�u�j �D$j Pj V�Rd=v�t��u$�L$8�9�L$h�� s
�   ��J#��j V���   ���t(��T$RV�QD��uUPP�D$P�8�A �T$�RV�Qh��_^][�Ā   Ð���������D$V�t$PV���������T$�D$RjV�D$�Qt^����   ������������|�A ��  +���A Ð��������������DSUV�t$TW�5 �A ���A ���A ��A ���A ���A ���A �D$X���A P��  ��  �  �   3�US�   �����A �@   3����A j�-��A �D$0   �D$4`@ �\$8�\$<�t$@�\$D�\$H�4�A �L$,�\$LQ�D$L�D$T��A ���A �=��A SVSSU��PS��PSSh   �Sh��A j���A ;ã��A u_^]3�[��D� �T$dRP�|�A ���A P�x�A ���A Q�h�A �?Q  ��u ���A � �A Shpe@ Rh��A P�l�A ��-H�A jeV�t�A ���A Q�`  ����u�d  _^]3�[��D� �3  ���A �5p�A SSS�D$RP�փ|$t>�=�A �-��A ���A jSS�T$QR�օ�t�D$P�׍L$Q�����  �|$u΋T$Xj R���A ��  �D$_^][��D� �������V�t$W�|$��   wc��   t<�ƃ�t&��t��uwj ���A �m�D$P���A _3�^� j ���A _3�^� ���A �   �8�A ����A ���A �-��-