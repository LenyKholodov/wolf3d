#ifndef BUTTONS_H
#define BUTTONS_H

class Button{
 protected:
  int  x,y,width,height;
  int  press;
  char *text;
 public:
  Button():x(0), y(0), width(30), height(15), text(NULL), press(0) {}
  Button(int xc,int yc):x(xc), y(yc), width(30), height(15),
 	text(NULL), press(0) {}
  Button(int xc,int yc,int w,int h):x(xc), y(yc), width(w), height(h),
	text(NULL), press(0) {}
  Button(int xc,int yc,int w,int h, const char *txt);
  Button(int xc,int yc,const char * txt);
  inline int Width() { return width; }
  inline int Height() { return height;}
  inline int GetX() { return x; }
  inline int GetY() { return y; }
  inline int IsPress() { return press; }
  virtual int IsRect(int xc,int yc)
   { return (xc>=x && yc>=y && xc<=x+width && yc<=y+height); }
  virtual int IsPress(int xc,int yc);
  virtual ~Button() { Clear(); }
  virtual void Clear();
  virtual void PressD();
  virtual void PressU();
  virtual void ReDraw();
  virtual void ChangeText(const char *);
  virtual void MoveTo(int xc,int yc) { x=xc; y=yc; ReDraw();}
};

class Button_Fast: public Button{
 protected:
  char fast;
  int  len,lenCh,x1,y1;
 public:
  Button_Fast(int x,int y,const char *txt):Button(x,y,txt), fast(0)
	{ width=80; height=20; SetFast(); ReDraw(); }
  Button_Fast(int x,int y,const char *txt,char ch):Button(x,y,txt),
	fast(ch) { width=80; height=20; SetFast(); ReDraw(); }
  Button_Fast(const char *txt):Button(0,0,txt), fast(0)
	{ width=80; height=20; SetFast(); ReDraw(); }
  virtual void SetFast();
  virtual void PressU();
  virtual void PressD();
  virtual void ChangeText(const char *) {}
  virtual int IsFast(const char s)
   { return s==fast; }
};

class Button_Char: public Button{
 public:
  Button_Char(int x,int y,char ch):Button(x,y,12,12," ") { text[0]=ch; ReDraw();}
};

class Button_Ask: public Button_Char{
 public:
  Button_Ask(int x,int y):Button_Char(x,y,'?') {}
};

class Button_Close: public Button_Char{
 public:
  Button_Close(int x,int y):Button_Char(x,y,'X') {}
};

class Button_Up: public Button{
 public:
  Button_Up(int x,int y):Button(x,y,16,40," ") { text[0]=24; ReDraw();}
};

class Button_Down: public Button{
 public:
  Button_Down(int x,int y):Button(x,y,16,40," ") { text[0]=25; ReDraw();}
};

class Button_Left: public Button{
 public:
  Button_Left(int x,int y):Button(x,y,40,16," ") { text[0]=27; ReDraw();}
};

class Button_Right: public Button{
 public:
  Button_Right(int x,int y):Button(x,y,40,16," ") { text[0]=26; ReDraw();}
};

class Button_Min: public Button_Char{
 public:
  Button_Min(int x,int y):Button_Char(x,y,'_') {}
};

class Button_Max: public Button_Char{
 public:
  Button_Max(int x,int y):Button_Char(x,y,'O') {}
};

class Button_Ok: public Button_Fast{
 public:
  Button_Ok(int x,int y):Button_Fast(x,y,"Ok",'k') {}
};

class Button_Cancel: public Button_Fast{
 public:
  Button_Cancel(int x,int y):Button_Fast(x,y,"Отмена",'О') {}
};

class Button_Exit: public Button_Fast{
 public:
  Button_Exit(int x,int y):Button_Fast(x,y,"Выход",'ы') {}
};

class Button_Yes: public Button_Fast{
 public:
  Button_Yes(int x,int y):Button_Fast(x,y,"Да",'Д') {}
};

class Button_No: public Button_Fast{
 public:
  Button_No(int x,int y):Button_Fast(x,y,"Нет",'Н') {}
};

class Button_Set: public Button_Fast{
 public:
  Button_Set(int x,int y):Button_Fast(x,y,"Применить",'П') {}
};

class Button_Change: public Button_Fast{
 public:
  Button_Change(int x,int y):Button_Fast(x,y,"Изменить",'И') {}
};

class Button_Browse: public Button_Fast{
 public:
  Button_Browse(int x,int y):Button_Fast(x,y,"Обзор",'з') {}
};

class Button_Param: public Button_Fast{
 public:
  Button_Param(int x,int y):Button_Fast(x,y,"Параметры",'а') {}
};

class Button_Search: public Button_Fast{
 public:
  Button_Search(int x,int y):Button_Fast(x,y,"Просмотр",'с') {}
};

class Button_Next: public Button_Fast{
 public:
  Button_Next(int x,int y):Button_Fast(x,y,"Далее",'Д') {}
};

class Button_Back: public Button_Fast{
 public:
  Button_Back(int x,int y):Button_Fast(x,y,"Назад",'Н') {}
};

#endif