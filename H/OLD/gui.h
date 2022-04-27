#if !defined (GUI_H)
#define GUI_H

class Box{
  protected:
    void *image;
    int  x,y,xsize,ysize;
    int  xold,yold;
    int  BoxColor;
    int  HeaderColor;
    int  BorderColor;
    char *header;
    char *message;
    void *fone;
    int  nowinit;
    int  visible;
    virtual void drawFirst();
  public:
    Box(int x0,int y0,int xs,int ys,char *hr,char *mss);
    virtual ~Box();
    virtual void draw();
    virtual void hide();
    virtual void move(int xnew,int ynew);
    int  isPress(int xc,int yc);
};

class Button: public Box{
  protected:
    int TextColor;
    int press;
    virtual void drawFirst();
  public:
    Button(int xv, int yv,char* textv);
    virtual int  isPress(int xc,int yc);
    virtual void drawPress();
};

class MessageBox{
  protected:
    Button * but;
    Box    * bx;
    int    xsize,ysize;
    virtual void drawFirst();
  public:
    MessageBox(int xv,int yv,int width,int height,
	       char * header,char * message);
    ~MessageBox();
    virtual void draw();
    virtual void hide();
    virtual void move(int xnew,int ynew);
    virtual int  isPress(int xc,int yc);
};
#endif