#ifdef  __VIDEO_MEM_H__
#define __VIDEO_MEM_H__

#include <videopro.h>

const MEM_PALETTE_ID=PALETTE_ID+0x80;

class MemPalette:public Palette
{
protected:
	int upStart,upSize; 
public:
	Palette* shadow;
	virtual void update(void);
	void updateBounds(int,int);
	int  GetUpdateStart(void) const 	{ return upStart; }
	int  GetUpdateEnd(void) const 		{ return upStart+upSize-1; }
	int  GetUpdateSize(void) const 		{ return upSize; }
};

typedef struct
{
	long  size;
	int16 start,
	      count;
	RGBStruct palette[1];	
} RGBtag,*RGBtag_ptr;

typedef struct
{
	long  size;
	int16 start,
	      count;
	COLOR32 palette[1];	
} COLOR32tag,*COLOR32tag_ptr;

const int RGB_MEM_PALETTE_ID=MEM_PALETTE_ID+1,
      	  COLOR32_MEM_PALETTE_ID=RGB_MEM_PALETTE_ID+1;

class RGBMemPalette:public MemPalette
{
	boolean owner;
	RGBStruct *palette;
public:
	RGBMemPalette(void):Palette(0,0,RGB_MEM_PALETTE_ID) 	{ owner=TRUE; palette=NULL; }
	RGBMemPalette(MemPalette&
	~RGBMemPAlette(void)					{ free(); }	

	virtual void update(void);
	Palette(void):mObject(PALETTE_ID),Start(0),Size(0)	{}
	Palette(int st,int sz,int t):mObject(t),Start(st),Size(sz)	{}	
	virtual ostream& about(ostream&) const;
	int	start(void) const	 		{ return Start; }
	virtual	int start(int);
	int 	size(void)  const			{ return Size;	}
	virtual int size(int);
	virtual int load(int st,int sz,istream&,int rgbquad=TRUE);
	virtual int save(int st,int sz,ostream&,int rgbquad=TRUE);
	virtual int set(int index,RGBStruct const* ptr);
	virtual int get(int index,RGBStruct* ptr) const;
	virtual int set(int index,long rgbq);
	virtual long get(int index) const;
	virtual int set(int index,int count,RGBStruct const* ptr);
	virtual int get(int index,int count,RGBStruct* ptr) const;
	int set(RGBStruct const* ptr)     { return set(Start,Size,ptr); }
	int get(RGBStruct* ptr) const	  { return get(Start,Size,ptr); }
	virtual int set(int index,int count,COLOR32 const* ptr);
	virtual int get(int index,int count,COLOR32* ptr) const;
	int set(COLOR32 const* ptr)     { return set(Start,Size,ptr); }
	int get(COLOR32* ptr) const	  { return get(Start,Size,ptr); }
	virtual int set(int index,int count,Palette const& pal);
	virtual int get(int index,int count,Palette& pal) const;
	int set(Palette const& ptr)     { return set(Start,Size,ptr); }
	int get(Palette& ptr) const     { return get(Start,Size,ptr); }
	Palette& operator=(Palette const& pal)	     { this->set(pal); return *this; }
};

#endif