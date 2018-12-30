#include <error.h>
#include <misc.h>
#include <iomanip.h>

ErrorPro *CurrentError=NULL;

void ErrorPro::dump(ostream& os) const
{
	if(ErrCode)
	{
		IOS_FMTFLAGS sflags=os.setf(ios::uppercase);
		os<<"Error code: "<<hex<<setfill('0')<<setw(INT_HEX_DIGIT_WIDTH)<<ErrCode;
		os.flags(sflags);
	}
}

const char* DosErrStr(int DosErrCode)
{
	switch(lobyte(DosErrCode))
	{
		case 0x01: return "Invalid function number";
		case 0x02: return "File not found";
		case 0x03: return "Path not found";
		case 0x04: return "Too many open files";
		case 0x05: return "Access denied";
		case 0x06: return "Invalid handle";
		case 0x07: return "Memory ctrl blocks destroyed";
		case 0x08: return "Insufficient memory";
		case 0x09: return "Invalid memory block address";
		case 0x0A: return "Invalid environment";
		case 0x0B: return "Invalid format";
		case 0x0C: return "Invalid access code";
		case 0x0D: return "Invalid data";
		case 0x0F: return "Invalid drive specified";
		case 0x10: return "Can't remove current dir";
		case 0x11: return "Not same device";
		case 0x12: return "No more matching files";
	}
	return "unknown error";
}