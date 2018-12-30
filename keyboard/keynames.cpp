#include <keyboard.h>

const char* Keyboard::ScanNames[0x80]={
	"NULL",		"ESC",		"1",		"2",    
   	"3",		"4",    	"5",		"6",      //0x00-0x07
	"7",     	"8",    	"9",	   	"0",    
	"-",		"+",    	"BS",    	"TAB",    //0x08-0x0F
	"Q",     	"W",    	"E",	   	"R",    
	"T",    	"Y",       	"U",     	"I",      //0x10-0x17
	"O",     	"P",    	"[", 		"]",
	"ENTER",	"LCTRL",    	"A",     	"S",      //0x18-0x1F
   	"D",     	"F",    	"G", 	   	"H",    
	"J",    	"K",       	"L",     	":",  //0x20-0x27
   	"\"",  		"~", 		"LSHIFT",	"\\", 
	"Z",    	"X",       	"C",     	"V",      //0x28-0x2F
   	"B",     	"N",    	"M",	   	",",  
	".",   		"/",     	"RSHIFT",	"PRTSCR", //0x30-0x37
   	"LALT",   	"SPACE",	"CAPSLOCK",  	"F1",   
	"F2",   	"F3",	    	"F4",    	"F5",     //0x38-0x3F
   	"F6",    	"F7",   	"F8",	   	"F9",   
	"F10",  	"NUMLOCK",     	"SCROLL",	"HOME",   //0x40-0x47  
   	"UP",   	"PGUP", 	"NUM-",		"LEFT", 
	"NUM5",   	"RIGHT",	"NUM+", 	"END",    //0x48-0x4F
   	"DOWN", 	"PGDN", 	"INS",   	"DEL",  
	"SYSRQ",	"CursorINS",   	"MACRO", 	"F11",    //0x50-0x57  
   	"F12",   	"CursorDEL",	"PA1",   	"LWIN", 
	"RWIN", 	"MENU",    	"CursorHOME",  	"CursorEND",   //0x58-0x5F
	"CursorPGUP",  	"CursorPGDN", 	"Scan 0x62",  	"F16",  
	"F17",  	"F18",     	"F19",   	"F20",    //0x60-0x67
   	"F21",   	"F22",  	"F23",   	"F24",  
	"Scan 0x6C", 	"ERASEEOF",	"Scan 0x6E",  	"COPY",   //0x68-0x6F
	"Scan 0x70",	"Scan 0x71",	"CRSEL",      	"DELTA",
	"EXCEL",      	"Scan 0x75",	"CLEAR",  	"RCTRL",  //0x70-0x77
	"RALT",  	"NumENTER",     "CursorUP",	"CursorDOWN",
	"CursorLEFT",	"CursorRIGHT",	"Scan 0x7E",	"Scan 0x7F"
};