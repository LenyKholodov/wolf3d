#include <midasshl.h>
// from waveread.h from midas lib

#pragma pack(1)

typedef struct
{
    char  name[4];
    uint32 length;
} mWaveChunkHeader;

typedef struct
{
    char riff[4];                       /* "RIFF" */
    uint32 length;
} mWaveRiffChunk;


typedef struct
{
    uint16 formatTag,
           numChannels;
    uint32 samplesPerSec,
               avgBytesPerSec;
    uint16 blockSize;
} mWaveFmtData;


typedef struct
{
  uint16 bitsPerSample;
} mWavePcmFmtData;

#pragma pack()

// for SDevice
#define CALLING __cdecl
#define GLOBALVAR

#include "D:\projects\MIDAS\SRC\MIDAS\ERRORS.H"
// used for SoundDevice
#include "D:\projects\MIDAS\SRC\MIDAS\SDEVICE.H"

extern "C" SoundDevice *midasSD;


long MidasSoundShell::bfWaveFindChunk(BinFile* f, const char *name)
{
  for(mWaveChunkHeader chunk;;)
  {
    if(f->read(&chunk,sizeof(mWaveChunkHeader))!=sizeof(mWaveChunkHeader))
    {
      debug("bfWaveFindChunk: can't read chunk header\n");
      return -1;
    }
    if(tag2long(chunk.name)==tag2long(name))
      break;
    if(f->canseek())
      f->seekcur(chunk.length);
    else
      for(int i=0;i++<chunk.length;f->getc());
  }
  return chunk.length;
}

bfWaveState* MidasSoundShell::bfWaveOpen(BinFile *f)
{
  mWaveRiffChunk riff;
  debug("MidasSoundShell::bfWaveOpen: starting...\n");
  // Read and check initial "RIFF" chunk and WAVE id:
  if(f->read(&riff,sizeof(mWaveRiffChunk))!=sizeof(mWaveRiffChunk))
  {
    debug("bfWaveOpen: can't read WAVE chunk\n");
    return NULL;
  }

  if(tag2long(riff.riff)!=tag2long("RIFF"))
  {
    debug("bfWaveOpen: bad wave tag (no 'RIFF')\n");
    return NULL;
  }

  if(f->getl()!=tag2long("WAVE"))
  {
    debug("bfWaveOpen: bad wave format\n");
    return NULL;
  }

  long length;

  // Find the format chunk and read it:

  if((length=bfWaveFindChunk(f,"fmt "))<sizeof(mWaveFmtData))
  {
    debug("bfWaveOpen: bad wave format\n");
    return NULL;
  }

  mWaveFmtData fmt;

  if(f->read(&fmt,sizeof(mWaveFmtData))!=sizeof(mWaveFmtData))
  {
    debug("bfWaveOpen: error: end of chunk\n");
    return NULL;
  }
  length-=sizeof(mWaveFmtData);

  // Check that the WAVE file is a valid PCM file, and, if so, read the
  // PCM format data:

  if(fmt.formatTag!=1)
  {
    debug("bfWaveOpen: not supported wav format\n");
    return NULL;
  }

  if(length<sizeof(mWavePcmFmtData))
  {
    debug("bfWaveOpen: bad wav file\n");
    return NULL;
  }

  mWavePcmFmtData pcmfmt;

  if(f->read(&pcmfmt,sizeof(mWavePcmFmtData))!=sizeof(mWavePcmFmtData))
  {
    debug("bfWaveOpen: error: end of Pcm format tag\n");
    return NULL;
  }
  if((length-=sizeof(mWavePcmFmtData))>0)
  {
    if(f->canseek())
      f->seekcur(length);
    else
      for(int i=0;i++<length;f->getc());
  }

  // Finally, find the start of the data chunk:
  if((length=bfWaveFindChunk(f,"data"))<=0)
  {
    debug("bfWaveOpen: not found data chunk\n");
    return NULL;
  }

  // Allocate and build the state structure:
  bfWaveState *s;

  if((s=(bfWaveState*)__alloc_fn(sizeof(bfWaveState)))==NULL)
  {
    debug("bfWaveOpen: no memory\n");
    return NULL;
  }

  s->file=f;
  s->dataLength=length;
  s->dataLeft=length;
  s->sampleRate=fmt.samplesPerSec;
  s->sampleType=smpNone;
  s->dataStart=f->tell();

  // Determine the sample type:
  if((fmt.numChannels==1) && (pcmfmt.bitsPerSample==8))
    s->sampleType=smp8bitMono;
  if((fmt.numChannels==2) && (pcmfmt.bitsPerSample==8))
    s->sampleType=smp8bitStereo;
  if((fmt.numChannels==1) && (pcmfmt.bitsPerSample==16))
    s->sampleType=smp16bitMono;
  if((fmt.numChannels==2) && (pcmfmt.bitsPerSample==16))
    s->sampleType=smp16bitStereo;

  // Check that we got it:
  if(s->sampleType==smpNone)
  {
    __free_fn(s);
    debug("bfWaveOpen: unsupported sample format.\n");
    return NULL;
  }
  return s;
}

BOOL MidasSoundShell::bfLoadWaveSample(BinFile *f,WaveSampleItem* wsi)
{
  debug("MidasSoundShell::bfLoadWaveSample - starting...\n");
  bfWaveState *s;
  if((s=bfWaveOpen(f))==NULL)
    return FALSE;
  void* buffer;

  // Allocate sample loading buffer:
  if((buffer=__alloc_fn(s->dataLength))==NULL)
  {
    bfWaveClose(s);
    debug("bfLoadWaveSample: no memory\n");
    return FALSE;
  }
  sdSample smp;
  s->file->read(smp.sample=(uchar*)buffer,smp.sampleLength=s->dataLength);
  // Build Sound Device sample structure for the sample:
  smp.samplePos=sdSmpConv;
  smp.sampleType=s->sampleType;
  smp.loop1Start=0;
  // No loop 2:
  smp.loop2Start=smp.loop2End=0;
  smp.loop2Type=loopNone;



  if(wsi->loop)
  { // Loop the whole sample:
    smp.loopMode=sdLoop1;
    smp.loop1End=smp.sampleLength;
    smp.loop1Type=loopUnidir;
  }
  else
  { // No loop:
    smp.loopMode=sdLoopNone;
    smp.loop1End=0;
    smp.loop1Type=loopNone;
  }

  wsi->rate=s->sampleRate;

  bfWaveClose(s);

  // Add the sample to the Sound Device:
  unsigned sampleHandle;
  debug("***\tSD="<<midasSD<<endl);
  int error=midasSD->AddSample(&smp,1,&sampleHandle);
  __free_fn(buffer);
  if(error!=OK)
  {
    debug("bfLoadWaveSample: midas error code: "<<error<<endl);
    return FALSE;
  }

  wsi->sample=sampleHandle;
  wsi->loaded=TRUE;
  return TRUE;
}

int MidasSoundShell::SeekSampleName(const char* name)
{
  for(int i=0;i<mWaveCountSamples;i++)
    if(!strncmp(name,mSamples[i].name,_MAX_PATH))
      return i;
  return -1;
}

BOOL MidasSoundShell::loadSample(int index)
{
  debug("MidasSoundShell::loadSample: starting\n");
  if(index>=mWaveCountSamples || index<0)
  {
    debug("MidasSoundShell::loadSample: index="<<index<<"(max="<<mWaveCountSamples<<")\n");
    return FALSE;
  }
  if(mSamples[index].loaded)
    return TRUE;
  BinFile* bf;
  BOOL result;
  debug("MidasSoundShell::loadSample: search in mdpak...");
  if(mdpak!=NULL && (bf=mdpak->open(mSamples[index].name))!=NULL)
  {
    debug("Ok\n");
  }
  else
  {
    debug("Failed\n");
    debug("MidasSoundShell::loadSample: try to load from stdBinFile("<<mSamples[index].name<<")\n");
    if(IsBad(bf=new stdBinFile(mSamples[index].name,BinFile::OPEN_RO)))
    {
      debug("MidasSoundShell::loadSample: no memory\n");
      delete bf;
      return FALSE;
    }
  }
  debug("MidasSoundShell::loadSample - mSamples(index)="<<mSamples<<'('<<index<<")\n");
  result=bfLoadWaveSample(bf,mSamples+index);
  delete bf;
  debug("MidasSoundShell::loadSample: Load result="<<result<<endl);
  return result;
}

void MidasSoundShell::closeList(void)
{
  // освобождаем потоки
  for(int i=0;i<mChannelsCount;i++)
    if(mChannels[i].sample>=0
                     && mChannels[i].sample<mWaveCountSamples
                 && mSamples[mChannels[i].sample].loaded)
      stop(i);
  for(i=0;i<mWaveCountSamples;i++)
    if(mSamples[i].loaded)
      MIDASfreeSample(mSamples[i].sample);
  __free_fn(mSamples);
  mSamples=NULL;
  mWaveCountSamples=0;
}

MidasSoundShell::MidasSoundShell(tAllocFnPtr a,tFreeFnPtr f)
{
  mMidasInit=FALSE;
  mWaveCountSamples=0;
  mChannelsCount=0;
  mCurrentChannel=0;
  mSamples=NULL;
  mChannels=NULL;
  __alloc_fn=a;
  __free_fn=f;
  mdpak=NULL;
}

MidasSoundShell::MidasSoundShell(void)
{
  mMidasInit=FALSE;
  mWaveCountSamples=0;
  mChannelsCount=0;
  mCurrentChannel=0;
  mSamples=NULL;
  mChannels=NULL;
  __alloc_fn=malloc;
  __free_fn=free;
  mdpak=NULL;
}

BOOL MidasSoundShell::open(DirPak* dpak,int ccount,BOOL runConfig,const char* cfgname)
{
  close();
  mdpak=dpak;
  MIDASstartup();
  if(runConfig)
  {
    if(MIDASconfig())
      MIDASsaveConfig((char*)cfgname);
  }
  if(!MIDASloadConfig((char*)cfgname))
    MIDASdetectSoundCard();
  if(!MIDASinit())
  {
    debug("MidasSoundShell::open: error MIDASinit\n");
    MIDASclose();
    mdpak=NULL;
    return FALSE;
  }
  MIDASstartBackgroundPlay(0);
  if(!MIDASopenChannels(mChannelsCount=ccount))
  {
    debug("MidasSoundShell::open: error MIDASopenChannels\n");
    MIDASclose();
    mdpak=NULL;
    mChannelsCount=0;
    return FALSE;
  }
  mCurrentChannel=0;
  if((mChannels=(ChanelInfo*)__alloc_fn(sizeof(ChanelInfo)*mChannelsCount))==NULL)
  {
    debug("MidasSoundShell::open: no memory\n");
    MIDASclose();
    mdpak=NULL;
    mChannelsCount=0;
    return FALSE;
  }
  for(int i=0;i<mChannelsCount;i++)
  {
    mChannels[i].channel=MIDASallocateChannel();
    mChannels[i].playHandle=NULL;
    mChannels[i].volume=0;
    mChannels[i].sample=-1;
  }
  return mMidasInit=TRUE;
}

void MidasSoundShell::close(void)
{
  debug("MidasSoundShell::close - starting..\n");
  // stopped samples
  if(!mMidasInit)
    return;
  debug("MidasSoundShell::close - closeList...\n");
  closeList();
  debug("MidasSoundShell::close MIDASfreeChannels\n");
  for(int i=0;i<mChannelsCount;i++)
    MIDASfreeChannel(mChannels[i].channel);
  __free_fn(mChannels);
  mChannels=NULL;
  mCurrentChannel=0;
  mChannelsCount=0;
  mdpak=NULL;
  debug("MidasSoundShell::close MIDASclose\n");
  MIDASclose();
  mMidasInit=FALSE;
}

MidasSoundShell::~MidasSoundShell(void)
{
  close();
}

BOOL MidasSoundShell::load(const LoadSampleInfo* SampleList,int count,BOOL clearList)
{
  if(!mMidasInit)
    if(!open())
    {
      debug("MidasSoundShell::load: can't initialized midas system\n");
      return FALSE;
    }
  // Грузим новый список
  WaveSampleItem* newItems=(WaveSampleItem*)__alloc_fn(count*sizeof(WaveSampleItem));
  if(newItems==NULL)
  {
    debug("MidasSoundShell::load: no memory for new playlist\n");
    return FALSE;
  }
  if(clearList) // выбираем вариант (чистый или "грязный")
    closeList();
  for(int i=0;i<count;i++)
  {
    int j=SeekSampleName(SampleList[i].name);
    if(j!=-1) // if found - copy info struct
    {
      memcpy((newItems+i),(mSamples+j),sizeof(WaveSampleItem));
      mSamples[j].loaded=FALSE;
      for(int k=0;k<mChannelsCount;k++)
        if(mChannels[k].sample==j)
          mChannels[k].sample=i;
    }
    else
    {
      strncpy(newItems[i].name,SampleList[i].name,_MAX_PATH);
      newItems[i].sample=0;
      newItems[i].rate=0;
      newItems[i].loaded=FALSE;
      newItems[i].loop=SampleList[i].loop;
    }
  }
  // удаляем старый список
  closeList();
  // переназначаем список
  mSamples=newItems;
  mWaveCountSamples=count;
  return TRUE;
}

void MidasSoundShell::download(void)
{
  if(!mMidasInit || mSamples==NULL || mWaveCountSamples<=0)
    return;
  for(int i=0;i<mWaveCountSamples;i++)
    loadSample(i);
}

int MidasSoundShell::play(int index,int vol,int pan,BOOL wait)
{
  debug("MidasSoundShell::play - starting\n");
  // Проверяем на корректность сэмпла
  if(index<0 || index>=mWaveCountSamples || !mMidasInit)
  {
    debug("MidasSoundShell::play: incorrect index: "<<index<<'('<<mWaveCountSamples<<")\n");
    return -1;
  }
  if(!mSamples[index].loaded)
    if(!loadSample(index))
      return -1;
  if(mSamples[index].loop)
    wait=FALSE;
  // ищем пустое место начиная с 1го
  int channel=-1,j=mCurrentChannel;

  for(int i=0;i<mChannelsCount;i++)
    if(!playing(i))
    {
      channel=i;
      break;
    }
  if(channel<0)
    for(i=0;i<mChannelsCount;i++)
    {
      if(!mSamples[mChannels[j].sample].loop)
      {
        channel=j;
        stop(j);
        break;
      }
      if(++j==mChannelsCount)
        j=0;
  }
  if(channel==-1)
    return channel;

// run play!
  if((mChannels[channel].playHandle=MIDASplaySample(mSamples[index].sample,
                                               mChannels[channel].channel,
                                             0,mSamples[index].rate,
                                             mChannels[channel].volume=vol,
                                             pan))==NULL)
  {
    mChannels[channel].volume=0;
    mChannels[channel].sample=-1;
    return -1;
  }
  mChannels[channel].sample=index;
  if((mCurrentChannel=channel+1)>=mChannelsCount)
    mCurrentChannel=0;
  if(wait)
    while(playing(channel));
  return channel;
}

void MidasSoundShell::stop(int index)
{
  if(playing(index))
  {
    MIDASstopSample(mChannels[index].playHandle); // stopped sample
    mChannels[index].playHandle=NULL;
    mChannels[index].sample=-1;// this chanel is free
    mChannels[index].volume=0;
    mCurrentChannel=index;     // and used for next time
  }
}

void MidasSoundShell::stop(void)
{
  for(int i=0;i<mChannelsCount;i++)
    stop(i);
}

BOOL MidasSoundShell::playing(int index)
{
  if(index<0 || index>=mChannelsCount || mChannels[index].playHandle==NULL)
    return FALSE;
  if(MIDASgetSamplePlayStatus(mChannels[index].playHandle)==MIDAS_SAMPLE_STOPPED)
  {
    MIDASstopSample(mChannels[index].playHandle); // stopped sample
    mChannels[index].playHandle=NULL;
    mChannels[index].sample=-1;// this chanel is free
    mChannels[index].volume=0;
    mCurrentChannel=index;     // and used for next time
    return FALSE;
  }
  return TRUE;
}

BOOL MidasSoundShell::volume(int index,int vol)
{
  if(playing(index))
    return MIDASsetSampleVolume(mChannels[index].playHandle,mChannels[index].volume=vol);
  return FALSE;
}

BOOL MidasSoundShell::panning(int index,int pan)
{
  if(playing(index))
    return MIDASsetSamplePanning(mChannels[index].playHandle,pan);
  return FALSE;
}