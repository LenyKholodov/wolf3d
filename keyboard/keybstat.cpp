#include <keyboard.h>

const SysKeyboardState  & kbState  =*(const SysKeyboardState*)  0x00000417;
const SysKeyboardStateEx& kbStateEx=*(const SysKeyboardStateEx*)0x00000418;

ostream& operator<<(ostream& os,const SysKeyboardState& ks)
{
  const char* names[] = {"rShift",
  	      	         "lShift",
			 "Ctrl  ",
  	      	         "Alt   ",
			 "ScrlLk",
			 "NumLk ",
			 "CapsLk",
			 "Insert"};
  int state=ks;
  for(int i=0;i<8;state>>=1)
    os<<(state&1?"[x]":"[ ]")<<names[i++];
  return os<<endl;
}

ostream& operator<<(ostream& os,const SysKeyboardStateEx& ks)
{
  const char* names[]={"lCtrl ",
  	      	       "lAlt  ",
		       "SysReq",
		       "Hold  ",
                       "ScLkDn",
		       "NmLkDn",
		       "CsLkDn",
		       "InsDwn"};
  int state=ks;
  for(int i=0;i<8;state>>=1)
    os<<(state&1?"[x]":"[ ]")<<names[i++];
  return os<<endl;
}
