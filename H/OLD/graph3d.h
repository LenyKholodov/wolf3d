#ifndef VIDEO_H
#define VIDEO_H
void isometric_matrix();
void dimetric_matrix(float alpha);
void oblique_matrix(float alpha);
void project(float x,float y, float z,int *u,int *v);
void init_center(unsigned u,unsigned v);
void line_3d(float,float,float,float,float,float,char);
void put_pixel_3d(float,float,float,int);
#endif