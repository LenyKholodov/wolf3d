#include <tmpbfile.h>
#include <io.h>
#include <string.h>

void tmpBinFile::close(void)
{
	if(mode)
		if(::close(handle)==-1)
			SetErrorInfo(BFE_STD_CLOSE);
  	BinFile::close();
  	if(*fname)
    		unlink(fname);
  	*fname=0;	
}