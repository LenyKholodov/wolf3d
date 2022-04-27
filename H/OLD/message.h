//Message.h
//Message.h
//Keyboard ASCII codes only in English
//Коды символов ASCII только на английском

#ifndef MESSAGE_H
#define MESSAGE_H

const int MS_MOVE=1;
const int MS_LEFT_DOWN=2;
const int MS_LEFT_UP=4;
const int MS_RIGHT_DOWN=8;
const int MS_RIGHT_UP=0x10;
const int MS_CENTER_DOWN=0x20;
const int MS_CENTER_UP=0x40;
const int KB_PRESS=0x80;

const int EVENT_DONE=0xFFFF;
const int EVENT_NOT_DONE=0;
const int EVENT_NO_HANDLER=0xAAAA;

struct Message{
 unsigned int type;
 struct MouseType{
   char leftbutton,rightbutton,centerbutton;
   char move;
   int x,y;
 }Mouse;
 struct KeyboardType{
   char  press;
   char scancode,asciicode;
   char  insert,scrolllock,numlock,capslock;
   char  alt,ctrl,shift;
   char  rightshift,leftshift,rightalt,leftalt,rightctrl,leftctrl;
 }Keyboard;
};

class Event
{
 protected:
  int (*ms_LeftDown)(Message m);
  int (*ms_RightDown)(Message m);
  int (*ms_CenterDown)(Message m);
  int (*ms_LeftUp)(Message m);
  int (*ms_RightUp)(Message m);
  int (*ms_CenterUp)(Message m);
  int (*ms_Move)(Message m);
  int (*kb_Press)(Message m);
  int eventmask;
 public:
  Event();
  ~Event();
  int  IsEventDone();
  void DoneEvent();
  int  GetMessage(Message &);
  int  DispatchMessage(Message);
  int  SetEvent(int evtype,int (*f)(Message));
  int  ClearEvent(int);
};
#endif