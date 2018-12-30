#include <pakbfile.h>

tAllocFnPtr pakBinFile::m_alloc_fn=malloc;
tFreeFnPtr  pakBinFile::m_free_fn=free;

tAllocFnPtr unpakBinFile::m_alloc_fn=malloc;
tFreeFnPtr  unpakBinFile::m_free_fn=free;

tAllocFnPtr unpakBinLump::m_alloc_fn=malloc;
tFreeFnPtr  unpakBinLump::m_free_fn=free;

/****************************************************************************
*			Prototype					    *
****************************************************************************/

const int NIL=N;		// index for root of binary search trees

BOOL compressedBinFile::reset(void)
{
	if(mode)
	{
		low=0;
		m_r=N-F;
		for(int i=0;i<m_r;text_buf[i++]=' ');
		high=Q4;
		value=0;
		shifts=0;
		filelen=filepos=0;
		StartModel();
		return f->reset();
	}
	return FALSE;
}

void compressedBinFile::StartModel(void)  
{
	int i;
	for(sym_cum[i=N_CHAR]=0;i>=1;i--)
	{
		int ch=i-1;
		char_to_sym[ch]=i;
		sym_to_char[i]=ch;
		sym_freq[i]=1;
		sym_cum[i-1]=sym_cum[i]+sym_freq[i];
	}
// empirical distribution function (quite tentative). 
// Please devise a better mechanism!
	for(sym_freq[0]=position_cum[i=N]=0;i>=1;i--)
		position_cum[i-1]=position_cum[i]+10000/(i+200);
}

void compressedBinFile::UpdateModel(int sym)
{
	int i;
	if(sym_cum[0]>=MAX_CUM)
	{
		int c=0;
		for(i=N_CHAR;i>0;i--)
		{
			sym_cum[i]=c;
			c+=(sym_freq[i]=(sym_freq[i]+1)>>1);
		}
		sym_cum[0]=c;
	}
	for(i=sym;sym_freq[i]==sym_freq[i-1];i--);
	if(i<sym)
	{
		int ch_i, ch_sym;
		ch_i=sym_to_char[i];
		ch_sym=sym_to_char[sym];
		sym_to_char[i]=ch_sym;
		sym_to_char[sym]=ch_i;
		char_to_sym[ch_i]=sym;
		char_to_sym[ch_sym]=i;
	}
	for(sym_freq[i]++;--i>=0;sym_cum[i]++);
}

/****************************************************************************
*			pakBinFile					    *
****************************************************************************/

pakBinFile::~pakBinFile(void)
{
	close();
}

BOOL pakBinFile::reset(void)
{
	if(compressedBinFile::reset()==FALSE)
		return FALSE;
	codesize=0;
	bitbuffer=0;
	mask=128;
	fillBufMode=TRUE;
	m_len=0;
	InitTree();
	f->resize(0);
	f->reset();
	return TRUE;
}

void pakBinFile::InitTree(void)  // Initialize trees
{
// For i = 0 to N - 1, rson[i] and lson[i] will be the right and left children 
// of node i.  These nodes need not be initialized. Also, dad[i] is the parent 
// of node i.  These are initialized to NIL, which stands for 'not used.'
// For i = 0 to 255, rson[N + i + 1] is the root of the tree for strings that 
// begin with character i.  These are initialized to NIL.  Note there are 256 trees. 
	for(int i=0;i<N;dad[i++]=NIL);	      // node
	for(i=N+1;i<=N+256;rson[i++]=NIL);   // root
}

void pakBinFile::InsertNode(int r)
	/* Inserts string of length F, text_buf[r..r+F-1], into one of the
	   trees (text_buf[r]'th tree) and returns the longest-match position
	   and length via the global variables match_position and match_length.
	   If match_length = F, then removes the old node in favor of the new
	   one, because the old one will be deleted sooner.
	   Note r plays double role, as tree node and position in buffer. */
{
	int i,p,cmp=1, temp;
	uchar *key;

	key=&text_buf[r];
	p=N+1+key[0];
	rson[r]=lson[r]=NIL;
	for(match_length=0;;)
	{
		if(cmp>=0)
		{
			if(rson[p]!=NIL)
				p=rson[p];
			else
			{
				rson[p]=r;
				dad[r]=p;
				return;
			}
		}
		else
		{
			if(lson[p]!=NIL)
				p=lson[p];
			else
			{
				lson[p]=r;
				dad[r]=p;
				return;
			}
		}
		for(i=1;i<F;i++)
			if((cmp=key[i]-text_buf[p+i])!=0)
				break;
		if(i>THRESHOLD)
		{
			if(i>match_length)
			{
				match_position=(r-p)&(N-1);
				if((match_length=i)>=F)
					break;
			}
			else
				if(i==match_length)
				{
					if((temp=(r-p)&(N-1))<match_position)
						match_position=temp;
				}
		}
	}
	dad[r]=dad[p];
	lson[r]=lson[p];
	rson[r]=rson[p];
	dad[lson[p]]=dad[rson[p]]=r;
	if(rson[dad[p]]==p)
		rson[dad[p]]=r;
	else
		lson[dad[p]]=r;
	dad[p]=NIL;  /* remove p */
}

void pakBinFile::DeleteNode(int p)  /* Delete node p from tree */
{
	int  q;
	if(dad[p]==NIL) 
		return;  /* not in tree */
	if(rson[p]==NIL) 
		q=lson[p];
	else 
		if(lson[p]==NIL)
			q=rson[p];
		else 
		{
			q=lson[p];
			if(rson[q]!=NIL) 
			{
				do 
					q=rson[q];
				while(rson[q]!=NIL);
				rson[dad[q]]=lson[q];
				dad[lson[q]]=dad[q];
				lson[q]=lson[p]; 
				dad[lson[p]]=q;
			}
			rson[q]=rson[p];  
			dad[rson[p]]=q;
		}
	dad[q]=dad[p];
	if(rson[dad[p]]==p) 
		rson[dad[p]]=q;
	else                   
		lson[dad[p]]=q;
	dad[p] = NIL;
}

void pakBinFile::Output(int bit)
{
	for(PutBit(bit);shifts>0;shifts--) 
		PutBit(!bit); 
}

void pakBinFile::EncodePosition(int position)
{
	ulong range=high-low;
	high=low+(range*position_cum[position])/position_cum[0];
	for(low+=(range*position_cum[position+1])/position_cum[0];;high+=high)
	{
		if(high<=Q2)
			Output(0);
		else
			if(low>=Q2)
			{
				Output(1);
				low-=Q2;
				high-=Q2;
			}
			else
				if(low>=Q1 && high<=Q3)
				{
					shifts++;
					low-=Q1;
					high-=Q1;
				}
					else
						break;
		low+=low;
	}
}

void pakBinFile::EncodeChar(int ch)
{
	int sym=char_to_sym[ch];
	ulong range=high-low;
	high=low+(range*sym_cum[sym-1])/sym_cum[0];
	for(low+=(range*sym_cum[sym])/sym_cum[0];;high+=high)
	{
		if(high<=Q2)
			Output(0);
		else
			if(low>=Q2)
			{
				Output(1);
				low-=Q2;
				high-=Q2;
			}
			else
				if(low>=Q1 && high<=Q3)
				{
					shifts++;
					low-=Q1;
					high-=Q1;
				}
				else
					break;
		low+=low;
	}
	UpdateModel(sym);
}

BOOL pakBinFile::open(BinFile* fil)
{
	close();
	if(fil==NULL)
		return FALSE;
	if(fil->canseek() && fil->canwrite())
	{
		mode=can_write|can_resize;
		f=fil;
		reset();
		return TRUE;
	}
	debug("pakBinFile::open: error: not found requered access\n");
	return FALSE;
}

void pakBinFile::close(void)
{
	if(!mode)
		return;
	if(fillBufMode)
	{
		InitPrimary();
		StepFirst();
	}
	for(;;StepFirst())
	{
	       StepLast();
	       if(m_len<=0)
			break;
	}
	EncodeEnd();
	f->putl(filelen);
	BinFile::close();
}

long pakBinFile::_close(void)
{
	if(!mode)
		return 0;
	if(fillBufMode)
	{
		InitPrimary();
		StepFirst();
	}
	for(;;StepFirst())
	{
	       StepLast();
	       if(m_len<=0)
			break;
	}
	EncodeEnd();
	long flen=filelen;
	BinFile::close();
	return flen;
}

void pakBinFile::InitPrimary(void)
{
	filelen=m_len;
	for(int i=1;i<=F;i++)
		InsertNode(m_r-i);
	InsertNode(m_r);
	m_s=0;
}

void pakBinFile::StepFirst(void)
{
	if(match_length>m_len)
		match_length=m_len;
	if(match_length<=THRESHOLD)
	{
		match_length=1;
		EncodeChar(text_buf[m_r]);
	}
	else
	{
		EncodeChar(255-THRESHOLD+match_length);
		EncodePosition(match_position-1);
	}
	last_match_length=match_length;
	m_i=0;
}

void pakBinFile::StepLast(void)
{
	for(filelen+=m_i;m_i++<last_match_length;)
	{
		DeleteNode(m_s);
		m_s=(m_s+1)&(N-1);
		m_r=(m_r+1)&(N-1);
		if(--m_len)
			InsertNode(m_r);
	}
}

void pakBinFile::_putc(char c)
{
	if(fillBufMode)
	{
		text_buf[m_r+m_len]=c;
		m_len++;
		if(m_len>=F)
		{
			fillBufMode=FALSE;
			InitPrimary();
			StepFirst();
		}
	}
	else
	{
		for(;m_i>=last_match_length;StepFirst())
			StepLast();
		DeleteNode(m_s);
		text_buf[m_s]=c;
		if(m_s<F-1)
			text_buf[m_s+N]=c;
		m_s=(m_s+1)&(N-1);
		m_r=(m_r+1)&(N-1);
		InsertNode(m_r);
		m_i++;
	}
}

BinFile& pakBinFile::putc(char c)
{
	if(!mode)
		return BinFile::putc(c);
	_putc(c);
	return *this;
}

long pakBinFile::write(const void *buf, long len)
{
	if(mode)
	{
		const char* ptr=(const char*)buf;
		for(long i=0;i++<len;ptr++)
			 _putc(*ptr);
		return len; 
	}
	return BinFile::write(buf,len);
}

BinFile& pakBinFile::puts(int16 s)
{	
	if(!mode)
		return BinFile::puts(s);
	_putc(lobyte(s));
	_putc(hibyte(s));	
	return *this;
}

BinFile& pakBinFile::putl(long l)
{
	if(!mode)
		return BinFile::putl(l);
	_putc(lobyte(l));
	_putc(lobyte(l>>8));	
	_putc(lobyte(l>>16));
	_putc(lobyte(l>>24));	
	return *this;
}

/****************************************************************************
*			unpakBinFile					    *
****************************************************************************/
int unpakBinFile::BinarySearchSym(uint x)
{
	int i=1,j;
	for(j=N_CHAR;i<j;)
	{
		int k=(i+j)/2;
		if(sym_cum[k]>x)
			i=k+1;
		else 
			j=k;
	}
	return i;
}

int unpakBinFile::BinarySearchPos(uint x)
{
	int i=1;
	for(int j=N;i<j;)
	{
		int k=(i+j)/2;
		if(position_cum[k]>x)
			i=k+1;
		else
			j=k;
	}
	return i-1;
}

BOOL unpakBinFile::reset(void)
{
	if(compressedBinFile::reset()==FALSE)
		return FALSE;
	bitbuffer=mask=0;
	m_i=m_j=0;
	filelen=ReallySize>=0?ReallySize:(*f)[codesize-4].getl();
	f->reset();
	// StartDecode
	for(int i=0;i<M+2;i++)
		GetBit();
	return TRUE;
}

long unpakBinFile::seek(long pos)
{
	if(reset()==FALSE)
		return FALSE;
	if(pos<size())
		while(pos--)
			_getc();
	return tell();
}

BOOL unpakBinFile::open(BinFile* fil,long rs)
{
	close();
	if(fil==NULL || fil->size()==0 || !fil->canread() || !fil->canseek())
		return FALSE;
	mode=can_read|can_seek;
	f=fil;
	ReallySize=rs;
	codesize=f->size();
	reset();
	return TRUE;
}

int unpakBinFile::DecodePosition(void)
{
	ulong range=high-low;
	uint pos_cum0=position_cum[0];
	int position=BinarySearchPos(uint(((value-low+1)*pos_cum0-1)/range));
	high=low+(range*position_cum[position])/pos_cum0;
	for(low+=(range*position_cum[position+1])/pos_cum0;;GetBit())
	{
		if(low>=Q2)
		{
			value-=Q2;
			low-=Q2;
			high-=Q2;
		}
		else
			if(low>=Q1 && high<=Q3)
			{
				value-=Q1;
				low-=Q1;
				high-=Q1;
			}
			else
				if(high>Q2)
					break;
		low+=low;
		high+=high;
	}
	return position;
}

int unpakBinFile::DecodeChar(void)
{
	int sym, ch;
	ulong range=high-low;
	uint sym_cum0=sym_cum[0];
	sym=BinarySearchSym(uint(((value-low+1)*sym_cum0-1)/range));
	high=low+(range*sym_cum[sym-1])/sym_cum0;
	for(low+=(range*sym_cum[sym])/sym_cum0;;GetBit())
	{
		if(low>=Q2)
		{
			value-=Q2;
			low-=Q2;
			high-=Q2;
		}
		else
			if(low>=Q1 && high<=Q3)
			{
				value-=Q1;
				low-=Q1;
				high-=Q1;
			}
			else
				if(high>Q2)
					break;
		low+=low;
		high+=high;
	}
	ch=sym_to_char[sym];
	UpdateModel(sym);
	return ch;
}

int unpakBinFile::_getc(void)
{
	int c;
	if(m_j)
	{
		c=text_buf[m_r++]=text_buf[m_i&(N-1)];
		m_i++;
		m_j--;
	}
	else
	{
		if((c=DecodeChar())<256)
			text_buf[m_r++]=c;
		else
		{
			m_i=(m_r-DecodePosition()-1)&(N-1);
			m_j=c-255+THRESHOLD;
			c=text_buf[m_r++]=text_buf[m_i&(N-1)];
			m_i++;
			m_j--;
		}
	}
	m_r&=(N-1);
	filepos++;
	return c;
}

int unpakBinFile::getc(void)
{
	if(!mode)
		return BinFile::getc();
	if(filepos>=filelen)
		return EOF;
	return _getc();
}

int unpakBinFile::gets(void)
{
	if(!mode)
		return BinFile::getc();
	switch(filelen-filepos)
	{
		case 0: return EOF;
			case 1: return _getc();
	}
	int lo=_getc();
	return word(_getc(),lo);
}

long unpakBinFile::getl(void)
{
	if(!mode)
		return BinFile::getc();
	long data=0;
	char* x=(char*)&data;
	switch(filelen-filepos)
	{
		default: *x++=_getc();
		case 3:  *x++=_getc();
		case 2:  *x++=_getc();
		case 1:  *x++=_getc();
	}
	return data;	
}

long unpakBinFile::read(void *buf, long len)
{
	if(!mode)
		return BinFile::read(buf,len);
	if(len+filepos>filelen)
		len=filelen-filepos;
	for(int i=0;i<len;i++)
		((char*)buf)[i]=_getc();
	return len;
}