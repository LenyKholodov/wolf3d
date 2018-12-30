#include <facet.h>

#include <string.h>
#include <sbinfile.h>
#include <debug.h>

Vertex* v_cache = NULL;

////////////////////////implementation of class Facet///////////////////////

void Facet::ComputeNormal(Vertex* v)
{                   //1-0 2-1
    n = ((v[vertex[1]] - v[vertex [0]]) ^
        ( v[vertex[2]] - v[vertex [0]]));
}

void Facet::ComputeLight(const DoubleVector3D& prj)
{
  d = (n & prj)/(float)!n;
}

void*   Facet::operator new (size_t bytes,size_t x)
{
  return ::operator new (bytes-sizeof(int)+x*sizeof(int));
}

Facet::Facet(int c,int* index): count(c), d(0)
{
#ifndef NDEBUG
  if (count<3)
     D_Error ("Facet: Wrong vertex count");
#endif

  memcpy(vertex,index,sizeof(int)*c);
}

Facet::Facet(const Facet& facet):count(facet.count), d(facet.d),n(facet.n)
{
  memcpy(vertex,facet.vertex,sizeof(int)*facet.count);
}

Facet&  Facet::operator = (const Facet& facet)
{
  count = facet.count;
  n     = facet.n;
  d     = facet.d;

  memcpy(vertex,facet.vertex,sizeof(int)*facet.count);

  return *this;
}

BOOL    Facet::IsFront() const
{
  return n.x*n.y*n.z<0;
}

BOOL    Facet::IsFront(const DoubleVector3D& v,Vertex* vrtx) const
{
  DoubleVector3D eye = vrtx[vertex[0]] - v;
  return (eye & n)<=0;
}

///////////////////implementation of class FacetNode/////////////////////////

FacetNode::FacetNode  ()
   : vertex(NULL), vrtx_size(0), facet_count(0) {}

FacetNode::FacetNode  (Vertex* v,int size)
    : vrtx_size(size), facet_count(0)
{
  if (v == NULL)
     dout<<"Facet node: Error load vertex"<<endl;
  else
  {
    vertex = new Vertex [vrtx_size];
    memcpy (vertex,v,size*sizeof(Vertex));
  }
}

FacetNode::~FacetNode () { if (vertex) delete vertex; }

BOOL       FacetNode::Load(const char* path)
{
  stdBinFile file;
  if (!file.open(path,BinFile::OPEN_RO))
  {
     dout<<"Facet node: Error open resource file"<<endl;
     return FALSE;
  }
  BOOL status = Load(&file);
  file.close();
  return status;
}

void       FacetNode::Save(const char* path)
{
  stdBinFile file;
  file.open(path,BinFile::OPEN_RW);
  Save(&file);
  file.close();
}

////////////////////implememntation of class FacetArray//////////////////////

FacetArray::FacetArray   (): FacetNode(), facet(NULL), facet_size(0) {}

FacetArray::FacetArray   (const FacetArray& f)
    :FacetNode (f.vertex,f.vrtx_count)
{
  facet_size = f.facet_size;


  memcpy (facet,f.facet,sizeof(Facet)*facet_size);      //no!!!
  memcpy (offset,f.offset,f.facet_count*sizeof(ulong));
}

FacetArray::FacetArray   (const FacetList& list)
      :FacetNode (list.vertex,list.vrtx_count)
{
  facet_count = list.facet_count;
  offset = new ulong [facet_count];

  ulong    pos  = 0;

  FacetListNode* cur = list.node;
  for (int i=0;i<facet_count && cur;i++,cur=cur->next)
  {
      offset [i] = pos;

      pos       += cur->facet->size();
  }

  facet  = (Facet*)(new uchar [pos]);

  cur = list.node;
  for (i=0;i<facet_count && cur;i++,cur=cur->next)
      (*this)[i] = *cur->facet;
}

FacetArray&      FacetArray::operator = (const FacetArray& f)
{
  facet_size = f.facet_size;
  vrtx_size  = f.vrtx_size;

  memcpy(vertex,f.vertex,sizeof(Vertex)*vrtx_size);
  memcpy(facet,f.facet,sizeof(Facet)*facet_size);

  return *this;
}

//FacetArray::operator FacetList* () {}

BOOL       FacetArray::Load(BinFile* file)
{
  file->reset();

  FNFHeader fnf;

  file->read(&fnf,sizeof(FNFHeader));

  if (memcmp(fnf.id,"FNF "))
  {
    dout<<"Wrong format"<<endl;
    return FALSE
  }

  file->read

  return TRUE;
}


void       FacetArray::Save(BinFile*) {}

//////////////////////implementation of class FacetList/////////////////////

struct FacetNode
{
        Facet*          facet;
        FacetListNode*  next;
};

FacetList::FacetList  (): node(NULL), count(0) {}
//FacetList::FacetList  (const FacetArray&) {}

FacetList::~FacetList ()
{
        //??
  FacetListNode* next = node->next;
  for (FacetListNode* tmp = node;tmp;tmp=next,next=next->next)
      delete tmp;
}

//operator       FacetList::FacetArray* ();

void       FacetList::PutFacet (Facet* f)
{
  if (last == NULL)
     last = node = new FacetListNode;
  else
     last        = last->next  = new FacetListNode;

  last->facet = f;
}

Facet*   FacetList::GetFacet ()
{
  if (node == NULL)
     return NULL;

  Facet* f           = node->facet;
  FacetListNode* del = node;
  node               = node->next;

  delete del;

  return f;
}

BOOL       FacetList::Load(BinFile*){ return FALSE;}
void       FacetList::Save(BinFile*) {}




