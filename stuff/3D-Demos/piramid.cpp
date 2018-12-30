#include <vector3D.h>
#include <matrix.h>
#include <facet.h>

#include <iostream.h>
#include <conio.h>

#include <vesa.h>
#include <mscreen.h>

const   int     grMode = VESA1024x768x8;

DoubleMatrix            prj (1.0);    //v3d
DoubleVector3D          eye (0,0,-10);
Vertex                  vertex [4];
Facet*                  facet  [4] = {NULL,NULL,NULL,NULL};

Screen*                 screen;
VESAdevice              vesa;

void            InitPiramid ();
void            DrawPiramid ();

void    main()
{
  if (vesa.open() != VESAdevice::OK)
  {
    cout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(grMode) != VESAdevice::OK)
  {
    cout<<"Error open VESA mode"<<endl;
    return;
  }

  screen      = GimmeMirror(vesa.CurrentScreen());

//  prj  = prj * DoubleMatrix::translate(DoubleVector3D(50,50,20));
//  prj                    = prj * DoubleMatrix::rotateX(FL_PI/4);
  DoubleMatrix transStep = DoubleMatrix::rotateZ(FL_PI/400);
//                           DoubleMatrix::rotateX(FL_PI/150);
//                           DoubleMatrix::rotateY(FL_PI/500);

  while (!kbhit())
  {
    screen->cls();

    prj = prj * transStep;

    InitPiramid();
    DrawPiramid();
    screen->refresh();

//    prj = prj * DoubleMatrix::translate(DoubleVector3D(2,2,-1));
  }

  getch();
  delete screen;
  vesa.close();
}

void            InitPiramid()
{
//  vertex[0] = Vertex (-10,-10,-10);
//  vertex[1] = Vertex (-30,-10,-10);
//  vertex[2] = Vertex (-30,-30,-30);
//  vertex[3] = Vertex (-30,-20,-10);

  vertex [0] = Vertex (10,10,-10);
  vertex [1] = Vertex (20,20,-10);
  vertex [2] = Vertex (5,20,-10);
  vertex [3] = Vertex (15,15,-20);

  for (int j = 0;j<4;j++)
      vertex[j] = vertex[j] * prj;

  v_cache   = vertex;

  int _line[4][3] =
  {{1,2,3},{0,1,3},{0,2,3},{0,2,1}};

  for (int i=0;i<4;i++)
  {
    if (facet[i]) delete facet[i];
    facet[i] = new (3) Facet(3,_line[i]);
  }
}

void    DrawPiramid ()
{
  for (int i=0;i<4;i++)
  {
    Facet*    f = facet[i];

    f->ComputeNormal();

    if (!f->IsFront(eye))
       continue;

    tPoint  v [3];

    for (int j=0;j<3;j++) //3-1
    {
      int       start  = j;

      v[j].x = 300 + 120*v_cache [f->vertex[start]].x/(v_cache [f->vertex[start]].z + 0.5);
      v[j].y = 200 + 120*v_cache [f->vertex[start]].y/(v_cache [f->vertex[start]].z + 0.5);
    }

    screen->polygone(3,v,i+1);
  }
}