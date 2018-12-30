#include <vesa.h>
#include <mscreen.h>
#include <stdlib.h>

#include <types.h>
#include <matrix.h>
#include <vector3d.h>

#include <iostream.h>

const   int     N1        = 10;
const   int     N2        = 10;
const   int     scrScale  = 64;
//const   double  FL_PI   = 3.1415926;

const   int     grMode  = VESA1024x768x8;

int             halfWidth,
                halfHeight;

VESAdevice      vesa;
Screen*         frameScreen;
Screen*         screen;

DoubleVector3D          prj (0,0,1);
DoubleVector3D          vertex[N1*N2];
DoubleMatrix            trans      = DoubleMatrix::rotateX(FL_PI);
DoubleMatrix            transStep  = DoubleMatrix::rotateY(FL_PI/80)*
                                    DoubleMatrix::rotateX(FL_PI/80);

typedef tPoint  Point;

struct  Facet
{
  int                   index[3];       //graph of vertex
  DoubleVector3D        n;
  float                 depth;
  float                 coeff;
}*torus,*tmp;

void   InitPalette();

int    FacetCmp(const Facet*,const Facet*);
void   InitTorus();
void   DrawTorus();

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

  screen      = vesa.CurrentScreen();
  frameScreen = GimmeMirror(screen);

  halfWidth  = screen->width() /2;
  halfHeight = screen->height()/2;
  InitPalette();

  torus = new Facet [N1*N2*2];
  tmp   = new Facet [N1*N2*2];

  frameScreen->cls();
  InitTorus();

  while (!kbhit())
  {
    frameScreen->cls();

    InitTorus();
    DrawTorus();

    trans       *= transStep;

    WaitForVsync();
    frameScreen->refresh();
  }

  vesa.close();

  delete        tmp;
  delete        torus;
}

int    FacetCmp(const void* v1,const void* v2)
{
  Facet* f1     = (Facet*)v1;
  Facet* f2     = (Facet*)v2;

  if (f1->depth < f2->depth)
     return  -1;
  else if (f1->depth > f2->depth)
     return 1;
  else
     return 0;
}

void   InitTorus ()
{
  float         r1 = 3;         //radius low
  float         r2 = 1;         //radius high

  int   k = 0;
  for (int i=0;i<N1;i++)
  {
    float       phi = i*2*FL_PI/N1;

    for (int j=0;j<N2;j++,k++)
    {
      float     psi = j*2*FL_PI/N2;

      vertex [k].x = (r1+r2*cos(psi)) * cos(phi);
      vertex [k].y = (r1+r2*cos(psi)) * sin(phi);
      vertex [k].z = r2 * sin(psi);

      vertex [k]   = trans * vertex [k];        //rotate torus
      vertex [k].z+= 10;   //??
    }
  }

  for (i=k=0;i<N1;i++)
      for (int j=0;j<N2;j++,k+=2)
      {
        torus [k].index [0] = i*N2 + j;
        torus [k].index [1] = ((i+1) % N1) * N2 +j;
        torus [k].index [2] = ((i+1) % N1) * N2 + (j+1) % N2;

        torus [k+1].index [0] = i*N2 + j;
        torus [k+1].index [1] = ((i+1) % N1) * N2 + (j+1) % N2;
        torus [k+1].index [2] = i * N2 + (j+1) % N2;
      }
}

void    DrawTorus ()
{
  int count = 0;

  for (int i=0;i<N1*N2*2;i++)
  {
    torus [i].n = (vertex [torus[i].index[1]] - vertex [torus[i].index[0]]) ^
                  (vertex [torus[i].index[2]] - vertex [torus[i].index[1]]);

    torus [i].coeff  = (torus[i].n & prj)/!torus[i].n;
    torus [i].depth = vertex[torus[i].index[0]] & prj;

    for (int j=1;j<3;j++)       //vertex loop
    {
      float     d = vertex [torus[i].index[j]] & prj;

      if (d<torus [i].depth)
         tmp [i].depth = d;
    }
    if (torus [i].coeff > 0)    //отсечение не лицевых граней
       tmp [count++] = torus [i];
  }

  qsort(tmp,count,sizeof(Facet),FacetCmp);

  Point edges [3];

  for (i=0;i<count;i++)
  {
    for (int k=0;k<3;k++)       //vertex loop
    {
                //parallel projection
      DoubleVector3D v = vertex [tmp[i].index[k]];    //rotate torus

      edges [k].x = halfWidth  + scrScale*v.x;
      edges [k].y = halfHeight + scrScale*v.y;
    }
    int color = 80+tmp[i].coeff*43;

    frameScreen->polygone(3,edges,color);
  }
}

void    InitPalette ()
{
  for (int i=0;i<256;i++)
      screen->PutPalette(i,0,0,i>>2);
  screen->PutPalette(0,0,32,32);
}

