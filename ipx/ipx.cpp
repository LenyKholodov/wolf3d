#include "ipx.h"
#include <iostream.h>
#include <dpmi32gw.h>

#include <string.h>
#include <debug/debug.h>

dos_ptr    IPX::mDOSPtr [8];
uint16     IPX::mDOSSel, IPX::mDOSSeg;

void far*          IPX::mPool           = NULL;
PacketPool*        IPX::mHeaderWrite    = NULL;
PacketPool*        IPX::mWrite          = NULL;
HeaderList*        IPX::mWriteUse       = NULL;
PacketPool*        IPX::mHeaderRead     = NULL;
PacketPool*        IPX::mRead           = NULL;
HeaderList*        IPX::mReadUse        = NULL;
localadr_t   far*  IPX::mLocalAddr      = NULL;

static       int   count                = 0;

int      XCHNG  (int x) { return (x>>8)+((x&255)<<8); }

void    IPX::Process         (RMREGS* rmRegs)
{
  rmRegs->w.cs = mDOSPtr [IPX_DRIVER].segment;
  rmRegs->w.ip = mDOSPtr [IPX_DRIVER].offset;

  DPMIRMCall(rmRegs);
}

void    IPX::ProcessIPXDrv   ()
{
  cout<<"Get IPX driver addr"<<endl;

  static RMREGS      regs;
  static RMREGS far* regsPtr = &regs;

  regs.w.ax  =  0x7a00;

  DPMIRMInt(0x2F,&regs,&regsPtr);

  if (regsPtr->h.al != 0xFF)
    cout<<"IPX not detected"<<endl;
  else
    cout<<"IPX detected"<<endl;

  mDOSPtr [IPX_DRIVER].segment = regsPtr->h.es;
  mDOSPtr [IPX_DRIVER].offset  = regsPtr->w.di;
}

void    IPX::ResetTables ()
{
  mPool=DPMIDOSAlloc(DOS_BUFFER_SIZE,&mDOSSel,&mDOSSeg);

  if (mPool == NULL)
  {
    cout<<"IPX no free DOS memory"<<endl;
    return;
  }

        //Инициализация пулов  для записи

  char far* mWriteHeaderPtr  = (char far*)mPool;
  char far* mReadHeaderPtr   = mWriteHeaderPtr + MAX_WRITE_PACKETS*(sizeof(IPXRMHeader)+sizeof(lump_t));
  char far* mWritePtr        = mReadHeaderPtr  + MAX_READ_PACKETS*(sizeof(IPXRMHeader)+sizeof(lump_t));
  char far* mReadPtr         = mWritePtr       + MAX_WRITE_PACKETS*(BUFFER_SIZE + sizeof(lump_t));

  if (mHeaderWrite) delete mHeaderWrite;
  if (mWrite)       delete mWrite;
  if (mRead)        delete mRead;
  if (mHeaderRead)  delete mHeaderRead;

  mHeaderWrite = new PacketPool (MAX_WRITE_PACKETS,sizeof(IPXRMHeader),mWriteHeaderPtr);
  mWrite       = new PacketPool (MAX_WRITE_PACKETS,BUFFER_SIZE,mWritePtr);
  mHeaderRead  = new PacketPool (MAX_READ_PACKETS,sizeof(IPXRMHeader),mReadHeaderPtr);
  mRead        = new PacketPool (MAX_READ_PACKETS,BUFFER_SIZE,mReadPtr);

        //Получение локального адреса

  mLocalAddr               = (localadr_t far*)(mReadPtr + MAX_READ_PACKETS*(BUFFER_SIZE+sizeof(lump_t)));

  GetLocalAddr  (mDOSSeg,(ulong)mLocalAddr);
}

IPXRMHeader far*        IPX::Alloc (PacketPool* headerPool,PacketPool* pool)
{
  Flag  flag ("ALLOC");

  IPXRMHeader far* header = (IPXRMHeader far*)headerPool->Alloc ();

  if (header == NULL)
  {
     return NULL;
  }

  header->pm_ptr = pool->Alloc();

  if (header->pm_ptr == NULL)
  {
    mHeaderWrite->Free (header);
    return NULL;
  }

  header->data_ptr.segment = mDOSSeg;
  header->ecb_ptr.segment  = mDOSSeg;
  header->ipx_ptr.segment  = mDOSSeg;

  header->data_ptr.offset  = (ulong)header->pm_ptr;
  header->ecb_ptr.offset   = (ulong)(char*)&header->ecb;
  header->ipx_ptr.offset   = (ulong)(char*)&header->ipxHeader;

  return header;
}

void    IPX::Free (PacketPool* headerPool,PacketPool* pool,IPXRMHeader far* header)
{
  if (header == NULL || header->pm_ptr == NULL)
     return;

  pool->      Free (header->pm_ptr);
  headerPool->Free (header);
}

HeaderList*     IPX::AddHeader  (IPXRMHeader far* hdr,HeaderList** first)
{
  Flag flag ("ADD_HEADER");

  if (hdr == NULL || first == NULL)
     return NULL;

  if (*first)
  {
    HeaderList* tmp = new HeaderList;

    tmp->next       = *first;
    tmp->prev       = NULL;
    tmp->header     = hdr;
    (*first)->prev  = tmp;
    *first          = tmp;
  }
  else
  {
    *first = new HeaderList;

    (*first)->next    = NULL;
    (*first)->prev    = NULL;
    (*first)->header  = hdr;
  }

  return *first;
}

void    IPX::SubHeader  (HeaderList** list)
{
  if (list == NULL || *list == NULL)
     return;

  HeaderList* tmp = *list;

  if ((*list)->next) (*list)->next->prev = (*list)->prev;
  if ((*list)->prev) (*list)->prev->next = (*list)->next;
  else                *list              =  NULL;

  delete tmp;
}

void    IPX::GetLocalAddr (ulong seg,ulong offs)
{
  static RMREGS regs;

  regs.w.si = offs;
  regs.h.es = seg;
  regs.w.bx = GET_LOCAL_ADDR;

  Process (&regs);
}

BOOL    IPX::OpenSocket  (socket_t& soc)
{
  static RMREGS regs;

  regs.w.dx = XCHNG(soc);
  regs.w.bx = OPEN_SOCKET;
  regs.h.al = 0;

  Process (&regs);

  soc = XCHNG (regs.w.dx);

  return !regs.h.al;
}

void   IPX::CloseSocket (socket_t soc)
{
  static RMREGS regs;

  regs.w.dx = XCHNG(soc);
  regs.w.bx = CLOSE_SOCKET;

  Process (&regs);
}

void  IPX::SendPacket (const IPXRMHeader far* header)
{
  static RMREGS regs;

  regs.h.es = header->ecb_ptr.segment;//mDOSSeg;
  regs.w.si = header->ecb_ptr.offset;//offs;
  regs.w.bx = SEND_PACKET;

  Process(&regs);
}

void  IPX::ListenForPacket (IPXRMHeader far * header)
{
  static RMREGS regs;

  regs.h.es = header->ecb_ptr.segment;//mDOSSeg;
  regs.w.si = header->ecb_ptr.offset;//offs;
  regs.w.bx = LISTEN_FOR_PACKET;

  Process(&regs);
}

void    IPX::RelinquishControl ()
{
  static RMREGS regs;

  regs.w.bx = RELINQUISH_CONTROL;

  Process(&regs);
}

addrnode_t&     IPX::local  (addrnode_t& node) volatile
{
  _fmemcpy (node.node,mLocalAddr->node,6);

  return node;
}

size_t IPX::write (const void* dest,size_t size,socket_t socket,const addrnode_t& addr)
{
  IPXRMHeader  far*  header  = Alloc     (mHeaderWrite,mWrite);
  HeaderList*        list    = AddHeader (header,&mWriteUse);

  if (list == NULL || header == NULL || header->pm_ptr == NULL)
     return 0;

  if (size > BUFFER_SIZE)
     size = BUFFER_SIZE;

  _fmemset(&header->ecb, 0, sizeof(ECB));

  header->ecb.Socket            = XCHNG(socket);
  header->ecb.FragmentCnt       = 2;
  header->ecb.Packet[0].Address = MakeDword (mDOSSeg,header->ipx_ptr.offset);
  header->ecb.Packet[0].Size    = sizeof (IPX_HEADER);
  header->ecb.Packet[1].Address = MakeDword (mDOSSeg,header->data_ptr.offset);
  header->ecb.Packet[1].Size    = size;

  lump_t far* tmp = (lump_t far*)((char far*)header-sizeof(lump_t));

//  _fmemset(header->ecb.ImmAddress, 0xff, 6);
  _fmemcpy (header->ecb.ImmAddress,addr.node,6);
  _fmemcpy (header->pm_ptr,dest,size);

  header->ipxHeader.PacketType = 4;
  _fmemset(header->ipxHeader.DestNetwork, 0, 4);
//  _fmemset(header->ipxHeader.DestNode, 0xff, 6);
  _fmemcpy (header->ipxHeader.DestNode,addr.node,6);
  header->ipxHeader.DestSocket = XCHNG(socket);

  SendPacket (header);

  return size;
}

size_t  IPX::read   (void* dest,size_t size,socket_t socket,addrnode_t& addr)
{
  IPXRMHeader far* header  = Alloc (mHeaderRead,mRead);

  _fmemset(&header->ecb, 0, sizeof(ECB));

  header->ecb.Socket            = XCHNG(socket);
  header->ecb.FragmentCnt       = 2;
  header->ecb.Packet[0].Address = MakeDword (mDOSSeg,header->ipx_ptr.offset);
  header->ecb.Packet[0].Size    = sizeof (IPX_HEADER);
  header->ecb.Packet[1].Address = MakeDword (mDOSSeg,header->data_ptr.offset);
  header->ecb.Packet[1].Size    = BUFFER_SIZE;

  ListenForPacket (header);

  while (header->ecb.InUse) ;

  _fmemcpy (addr.node,header->ecb.ImmAddress,6);

  if (size > header->ecb.Packet[1].Size) size = BUFFER_SIZE;
  if (dest) _fmemcpy (dest,header->pm_ptr,size);

  Free (mHeaderRead,mRead,header);

  return size;
}

IPXRMHeader far* IPX::Listen (socket_t socket)
{
  IPXRMHeader far* header  = Alloc (mHeaderRead,mRead);
  HeaderList*      list    = AddHeader (header,&mReadUse);

  _fmemset(&header->ecb, 0, sizeof(ECB));

  header->ecb.Socket            = XCHNG(socket);
  header->ecb.FragmentCnt       = 2;
  header->ecb.Packet[0].Address = MakeDword (mDOSSeg,header->ipx_ptr.offset);
  header->ecb.Packet[0].Size    = sizeof (IPX_HEADER);
  header->ecb.Packet[1].Address = MakeDword (mDOSSeg,header->data_ptr.offset);
  header->ecb.Packet[1].Size    = BUFFER_SIZE;

  ListenForPacket (header);

  return header;
}

void    IPX::Clear ()
{
  Flag  flag ("CLEAR");

  if (mWriteUse == NULL)
     return;

  HeaderList* next = mWriteUse->next;
  for (HeaderList* list=mWriteUse;list;list = next,next = next ? next->next : NULL)
      if (list->header->ecb.InUse == 0)
      {
        Free (mHeaderWrite,mWrite,list->header);
        SubHeader (&list);
        if (list == NULL)
        {
          mWriteUse = NULL;
          return ;
        }
      }
}

void    IPX::Dispatch ()
{
  if (mReadUse == NULL)
     return;

  HeaderList* next = mReadUse->next;
  for (HeaderList* list=mReadUse;list;list = next,next=next?next->next:NULL)
      if (list->header->ecb.InUse == 0)
      {
         Message far*    message = (Message far*)list->header->pm_ptr;
         int             size    = message->size > list->header->ecb.Packet[1].Size ?
                                   list->header->ecb.Packet[1].Size : message->size;
         message_queue.PutMessage (message,size);
         ListenForPacket (list->header);
      }
}

void    IPX::tick    ()
{
  Flag  ("IPX_TICK");

  Clear    ();
  Dispatch ();
               //Опрос IPX процессора
}

IPX::IPX ()
{
  if (count++ == 0)
  {
    ProcessIPXDrv();
    ResetTables();
  }
}

IPX::~IPX ()
{
  if (--count)
     return;

  cout<<"IPX destruct"<<endl;

        //Очистка очереди пакетов

  if (mWriteUse)
  {
    HeaderList* next = mWriteUse->next;
    for (HeaderList* list=mWriteUse;list;list = next,next = next ? next->next : NULL)
    {
        if (list)
           delete list;
    }
  }

  if (mReadUse)
  {
    HeaderList* next = mReadUse->next;
    for (HeaderList* list=mReadUse;list;list=next,next = next ? next->next : NULL)
    {
        if (list)
           delete list;
    }
  }

        //Освобождение DOS памяти

  DPMIDOSFree (mDOSSel);

        //Освобождение пулов

  if (mHeaderWrite) delete mHeaderWrite;
  if (mWrite)       delete mWrite;
}

ostream& operator << (ostream& os,const addrnode_t& addr)
{
  for (int i=0;i<6;i++)
      os<<(unsigned)addr.node [i]<<".";
  os<<char(8);

  return os;
}
