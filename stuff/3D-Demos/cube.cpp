#include <conio.h>
#include <vesa.h>
#include <vector3d.h>
#include <matrix.h>
#include <stdlib.h>
#include <iostream.h>

const float focus = 1.5f;
Screen*     screen;

struct Point
{
        int x,y;
};

struct Edge
{
        int v1,v2;
        int f1,f2;
        void SetEdge(int _v1,int _v2,int _f1,int _f2)
        {
                v1=_v1;
                v2=_v2;
                f1=_f1;
                f2=_f2;
        }
};

struct Facet
{
        int                                         v[4];
        Vector3D<double>         n;
        int                                    flags;
        void SetFacet(int v0,int v1,int v2,int v3)
        {
                v[0]=v0;
                v[1]=v1;
                v[2]=v2;
                v[3]=v3;
        }
};

class Cube
{
        public:
                Vector3D<double>        vertex[8];
                Edge                                        edge[12];
                Facet                           facet[6];

                Cube();
                void InitEdge (int i,int v1,int v2,int f1,int f2)
                        {        edge[i].SetEdge(v1,v2,f1,f2); }
                void InitFacet(int i,int v0,int v1,int v2,int v3)
                        { facet[i].SetFacet(v0,v1,v2,v3); }
                void ComputeNormals();

                int        IsFrontFacing(int i,Vector3D<double>& v)
                {
                        return ((vertex[facet[i].v[0]]-v) & facet[i].n)<0;
                }
                void apply(Matrix<double>&);
                void draw();
};

//Matrix<double>          prj = DoubleMatrix::rotateX(FL_PI)*
//                              DoubleMatrix::rotateY(FL_PI)*
//                              DoubleMatrix::rotateZ(FL_PI);
DoubleMatrix              prj(1);

Vector3D<double> eye(0,0,0);
Vector3D<double> light(10.2,0.5,1.2);

Cube::Cube()
{
        for (int i=0;i<8;i++)
        {
                vertex[i].x = i & 1 ? 1.0 : 0.0;
                vertex[i].y = i & 2 ? 1.0 : 0.0;
                vertex[i].z = i & 4 ? 1.0 : 0.0;
        }
        InitEdge(0,0,1,2,4);
        InitEdge(1,1,3,1,4);
        InitEdge(3,2,0,0,4);
        InitEdge(4,4,5,2,5);
        InitEdge(5,5,7,1,5);
        InitEdge(6,7,6,3,5);
        InitEdge(7,6,4,0,5);
        InitEdge(8,0,4,0,2);
        InitEdge(9,1,5,1,2);
        InitEdge(10,3,7,1,3);
        InitEdge(11,2,6,0,3);

        InitFacet(0,4,6,2,0);
        InitFacet(1,1,3,7,5);
        InitFacet(2,0,1,5,4);
        InitFacet(3,6,7,3,2);
        InitFacet(4,2,3,1,0);
        InitFacet(5,4,5,7,6);

}

void Cube::ComputeNormals()
{
                        //1-0 2-1
        for (int i=0;i<6;i++)
                facet[i].n = (vertex[facet[i].v[2]] - vertex[facet[i].v[1]]) ^
                             (vertex[facet[i].v[0]] - vertex[facet[i].v[1]]);
}

void Cube::apply(Matrix<double>& m)
{
        for (int i=0;i<8;i++)
                vertex[i] = m*vertex[i];
}

void Cube::draw()
{
        Point                                         p[8];
        Point                                         contour[4];
        Vector3D<double>    v;
        int                                                 color;

        for (int i=0;i<8;i++)
        {
                v = prj*vertex[i];
                p[i].x=v.x;
                p[i].y=v.y;
        }
        ComputeNormals();
        for (i=0;i<6;i++)       //out
                facet[i].flags = IsFrontFacing(i,eye);
        for (i=0;i<6;i++)
                if (facet[i].flags)
                {
                        int color = -(facet[i].n & light)*20+180;
                        for (int j=0;j<4;j++)
                                contour[j] = p[facet[i].v[j]];
                        screen->polygone(4,(tPoint*)contour,color);
                }
}

void main()
{
 Cube                                         cube;
 DoubleMatrix   trans;
 VESAdevice     vesa;
 int            mode = VESA1024x768x8;
 if (vesa.open()!=VESAdevice::OK)
 {
   cout<<"Error open VESA"<<endl;
   return;
 }
 if (vesa.SetMode(mode)!=VESAdevice::OK)
 {
   cout<<"Error open VESA mode"<<endl;
   return;
 }
 screen=vesa.CurrentScreen();
 prj[2][2] = 0;
 prj[3][2] = 1;
 prj = Matrix<double>::scale (Vector3D<double>(640,350,0))*prj;
 cube.apply(Matrix<double>::translate(Vector3D<double>(4,4,4)));

 for (int i=0;i<256;i++)
        screen->PutPalette(i,i>>2,i>>2,i>>2);

 DoubleMatrix transStep = DoubleMatrix::rotateX (FL_PI/50)*
                          DoubleMatrix::rotateY (FL_PI/180);

// while (!kbhit())
// {
       screen->cls();
       cube.apply(transStep);
       cube.draw();

//       prj = prj * transStep;
// }

 getch();
 vesa.close();
}
