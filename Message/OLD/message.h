#ifndef __MESSAGE__
#define __MESSAGE__

#include <types.h>

enum
{
  NULL_MESSAGE          = 0,
  MOUSE_MOVE            = 1,
  MOUSE_LBUTTON_DOWN    = 2,
  MOUSE_LBUTTON_UP      = 4,
  MOUSE_RBUTTON_DOWN    = 8,
  MOUSE_RBUTTON_UP      = 16,
  MOUSE_CBUTTON_DOWN    = 32,
  MOUSE_CBUTTON_UP      = 64,
  KEY_DOWN              = 128,
  KEY_UP                = 256,
  TIME_CLICK            = 512
};

////////////////////////////////////////////////////////////////////////////
////����饭��
////////////////////////////////////////////////////////////////////////////
struct  Message
{
  uint16          id;
  uint16          size;
};

////////////////////////////////////////////////////////////////////////////
////�������� ��।� ᮮ�饭��
////////////////////////////////////////////////////////////////////////////
class   MessageHandler
{
  public:
                        MessageHandler(long = 0xFFFF);
                        MessageHandler(void*,long = 0xFFFF);
          virtual       ~MessageHandler();

          BOOL          PutMessage(const Message*,int size);
   const  Message*      GetMessage(); //!!

          BOOL          IsEmpty() const;
          void          Clear();

          void          operator << ( const Message& );
          void          operator >> ( Message &);
  protected:
          void*         Alloc(long size);

  protected:
         uchar*       first;
         uchar*       last;

         uchar*       pool;
         long         pool_size;
};

inline void   MessageHandler::operator << ( const Message&  message)
{
  PutMessage(&message,sizeof(Message));
}

inline void   MessageHandler::operator >>( Message& message)
{
  message = *(Message*)GetMessage();
}

////////////////////////////////////////////////////////////////////////////
///�����窠 ��� �ᯮ�짮����� MessageHandler
////////////////////////////////////////////////////////////////////////////

class HandlerPtr
{
  private:
           MessageHandler* ptr;
  protected:
          HandlerPtr(const HandlerPtr* pointer) {}
  public:
          HandlerPtr();
          ~HandlerPtr ();

          operator = (int size);

          void      SetPool(void*,int);
          void      PutMessage (const Message* m,int size);
    const Message*  GetMessage ();

          BOOL          IsEmpty() const;
          void          Clear();
};


extern  HandlerPtr  message_queue;

#endif
