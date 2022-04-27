#include <conio.h>
const
  true=1;
  false=0;
class  box{
  private:
    int active;
  public:
    int  x,y,xsize,ysize;
    char header[40];
    char texter[1000];
    int  colortext;
    void drawbox(void);
	 box(void);
	~box(void);
   void  writemessage(void);
   int   getx;
   int    gety;
   int    getxsize;
   int    getysize;
   int    isactive;
}

box::box(void)
{
  active=true;
  x=10;
  y=10;
  xsize=20;
  ysize=10;
  colortext=15;
  header[xsize-2]=0;
}

void box::drawbox(void)
{
  wind(x+1,y+1,x+xsize+1,y+ysize+1,7,'±');
  wind(x,y,x+xsize,y+ysize,15,' ');
  doubleramka(x,y,x+xsize,y+ysize,15);
  gotoxy(x+(xsize-length(header)) div 2+1,y);
  textcolor(15);
  Write(header);
  active:=true;
}

void box::~box;
{
  active:=false;
  GotoXY(x+(xsize-length(header)) div 2+1,y);
  TextColor(1);
  Write(header);
end;

procedure Box.WriteMessage;
var xc,yc:byte;
begin
  textcolor(colortext);
  for yc:=y+1 to y+ysize-1 do
    for xc:=x+1 to x+xsize-1 do
    begin
      GotoXY(xc,yc);
      Write(texter[(yc-ysize)*xsize+xc-xsize]);
    end;
end;

int Box.GetX;
begin
  getx:=x;
end;

int Box.GetY;
begin
  gety:=y;
end;

int Box.GetXSize;
begin
  getxsize:=xsize;
end;

int Box.GetYSize;
begin
  getysize:=ysize;
end;

int Box.IsActive;
begin
  isactive:=active;
end;


end.
