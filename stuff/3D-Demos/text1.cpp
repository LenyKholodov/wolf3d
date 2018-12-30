#include <vesa.h>
#include <grtypes.h>
#include <picture.h>
#include <mscreen.h>

#include <vector3d.h>   //my
#include <matrix.h>     //my

#include <debug.h>
#include <keyboard.h>

cPicture        picture;
tImage*         image;
Screen*         screen;
VESAdevice      vesa;

Keyboard        kb;

DoubleVector3D        v[4];
DoubleVector3D        n,m,l;

uchar*          texture;
uint            width,
                height;

void    DrawScene();
void    CalcScene(DoubleVector3D*);

void    main(int argc,char* argv[])
{
  if (argc<2)
  {
    dout<<"Picture.bmp"<<endl;
    return;
  }
  if (vesa.open() != VESAdevice::OK)
  {
    dout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(VESA640x480x8) != VESAdevice::OK)
  {
    dout<<"Error open VESA mode"<<endl;
    return;
  }

  screen = GimmeMirror(vesa.CurrentScreen());

  picture.load (argv[1]);
  image   = picture.GetImage();
  texture = image->data;
  width   = image->width;
  height  = image->height;

  ShiftPalette(256,(RGBStruct*)picture.GetPalette(),-2);
  vesa.CurrentScreen()->PutPalette((RGBStruct*)picture.GetPalette());

  v[0]  = DoubleVector3D(100,100,1);
  v[1]  = DoubleVector3D(200,100,1);
  v[2]  = DoubleVector3D(200,200,1);
  v[3]  = DoubleVector3D(100,200,1);

  DoubleMatrix m = DoubleMatrix::rotateX (0);
  DoubleMatrix mStep = DoubleMatrix::rotateX (0.00001)*
                       DoubleMatrix::rotateY (0.00001)*
                       DoubleMatrix::rotateZ (0.00001);

  double       step  = 0.0001;
//                       DoubleMatrix::translate   (DoubleVector3D(-0.5,0.5,-0.01));
  DoubleVector3D vect[4];

  while (!kb.IsPress(KEY_ESC))
  {
//    if (!kb.IsPress())
//       continue;

    if (kb.IsPress(KEY_UPARROW))
       m = m * DoubleMatrix::rotateY (-step);
    if (kb.IsPress(KEY_DOWNARROW))
       m = m * DoubleMatrix::rotateY (step);
    if (kb.IsPress(KEY_LEFTARROW))
       m = m * DoubleMatrix::rotateX (-step);
    if (kb.IsPress(KEY_RIGHTARROW))
       m = m * DoubleMatrix::rotateX (step);
    if (kb.IsPress(KEY_INS))
       m = m * DoubleMatrix::rotateZ (step*1000);
    if (kb.IsPress(KEY_DEL))
       m = m * DoubleMatrix::rotateZ (-step*1000);
    if (kb.IsPress(KEY_HOME))
       m = m * DoubleMatrix::translate (DoubleVector3D(0,0,0.01));
    if (kb.IsPress(KEY_END))
       m = m * DoubleMatrix::translate (DoubleVector3D(0,0,-0.01));

    screen->cls();
    for (int i=0;i<4;i++)
        vect[i] = v[i] * m;

    CalcScene(vect);
    DrawScene();

    screen->refresh();
  }

  delete screen;
  vesa.close();
}

void    CalcScene(DoubleVector3D* vertex)
{
  DoubleVector3D e1 = vertex [1] - vertex [0];      //1-0
  DoubleVector3D e2 = vertex [3] - vertex [0];      //2-1
  n = e1 ^ e2;
  m = e2 ^ vertex [0];
  l = vertex[0] ^ e1;
}

void    DrawScene ()
{
  double        d,du,dv;
  uint          u,v;

  for (int row = 0;row<screen->height();row++)
      for (int col = 0;col<screen->width();col++)
      {
        d  = n.z + n.y*row + n.x*col;
        du = m.z + m.y*row + m.x*col;
        dv = l.z + l.y*row + l.x*col;

        u  = ((double)width)*(du/d);
        v  = ((double)height)*(dv/d);

        if (u>=width || v>=height)
           continue;

        screen->PutPixel(col,row,texture[v*width+u]);
      }
}

