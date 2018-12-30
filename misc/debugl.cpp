#include <debug.h>

#ifndef __BORLANDC__
	ofstream dlogout("runlog.txt",ios::text|ios::out);
#else
	ofstream dlogout("runlog.txt");
#endif
