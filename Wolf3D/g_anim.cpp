#include "g_anim.h"
#include "g_types.h"

int     NextFace (char* faceTable,int& iterator,int& curFace)
{
  int  face = faceTable [iterator & ANIM_MASK];

  if (face==-1)
  {
     curFace = ANIM_DEFAULT_FACE;
     return -1;
  }

  if (iterator == ANIM_NUM-1 ||
      faceTable[(iterator&ANIM_MASK)+1] == END_OF_ANIM)
  {
      iterator = 0;
      curFace  = ANIM_DEFAULT_FACE;
  }
  else
      iterator++;

  return face;
}

