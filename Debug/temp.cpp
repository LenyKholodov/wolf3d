    void          far* _old_exc00h; void  far* _old_exc01h;
    void          far* _old_exc02h; void  far* _old_exc03h;
    void          far* _old_exc04h; void  far* _old_exc05h;
    void          far* _old_exc06h; void  far* _old_exc07h;
    void          far* _old_exc08h; void  far* _old_exc09h;
    void          far* _old_exc0Ah; void  far* _old_exc0Bh;
    void          far* _old_exc0Ch; void  far* _old_exc0Dh;
    void          far* _old_exc0Eh; void  far* _old_exc0Fh;
    void          far* _old_exc10h; void  far* _old_exc11h;
    void          far* _old_exc12h;

    static void  far     exc00h(); static  void  far     exc01h();
    static void  far     exc02h(); static  void  far     exc03h();
    static void  far     exc04h(); static  void  far     exc05h();
    static void  far     exc06h(); static  void  far     exc07h();
    static void  far     exc08h(); static  void  far     exc09h();
    static void  far     exc0Ah(); static  void  far     exc0Bh();
    static void  far     exc0Ch(); static  void  far     exc0Dh();
    static void  far     exc0Eh(); static  void  far     exc0Fh();
    static void  far     exc10h(); static  void  far     exc11h();
    static void  far     exc12h();

void  far  Debug::ResetNames()
{
  for (int i=0;i<0x13;i++)
  {
      if (excNames[i])
         delete excNames[i];
      SetExceptionName(i,ExcName[i]);
  }
}

void  far  Debug::DeleteNames()
{
  for (int i=0;i<0x13;i++)
      delete excNames[i];
}

void  far   Debug::SetExceptionName(int i,char* name)
{
  excNames[i] = new char[strlen(name)+1];
  strcpy(excNames[i],name);
}

const char far*  far   Debug::GetExceptionName(int i)
{
  return excNames[i];
}

  _old_exc00h = DPMIGetExceptionHandler(0x00);
  _old_exc01h = DPMIGetExceptionHandler(0x01);
  _old_exc02h = DPMIGetExceptionHandler(0x02);
  _old_exc03h = DPMIGetExceptionHandler(0x03);
  _old_exc04h = DPMIGetExceptionHandler(0x04);
  _old_exc05h = DPMIGetExceptionHandler(0x05);
  _old_exc06h = DPMIGetExceptionHandler(0x06);
  _old_exc07h = DPMIGetExceptionHandler(0x07);
  _old_exc08h = DPMIGetExceptionHandler(0x08);
  _old_exc09h = DPMIGetExceptionHandler(0x09);
  _old_exc0Ah = DPMIGetExceptionHandler(0x0A);
  _old_exc0Bh = DPMIGetExceptionHandler(0x0B);
  _old_exc0Ch = DPMIGetExceptionHandler(0x0C);
  _old_exc0Dh = DPMIGetExceptionHandler(0x0D);
  _old_exc0Eh = DPMIGetExceptionHandler(0x0E);
  _old_exc0Fh = DPMIGetExceptionHandler(0x0F);
  _old_exc10h = DPMIGetExceptionHandler(0x10);
  _old_exc11h = DPMIGetExceptionHandler(0x11);
  _old_exc12h = DPMIGetExceptionHandler(0x12);

  DPMISetExceptionHandler(0x00,_old_exc00h);
  DPMISetExceptionHandler(0x01,_old_exc01h);
  DPMISetExceptionHandler(0x02,_old_exc02h);
  DPMISetExceptionHandler(0x03,_old_exc03h);
  DPMISetExceptionHandler(0x04,_old_exc04h);
  DPMISetExceptionHandler(0x05,_old_exc05h);
  DPMISetExceptionHandler(0x06,_old_exc06h);
  DPMISetExceptionHandler(0x07,_old_exc07h);
  DPMISetExceptionHandler(0x08,_old_exc08h);
  DPMISetExceptionHandler(0x09,_old_exc09h);
  DPMISetExceptionHandler(0x0A,_old_exc0Ah);
  DPMISetExceptionHandler(0x0B,_old_exc0Bh);
  DPMISetExceptionHandler(0x0C,_old_exc0Ch);
  DPMISetExceptionHandler(0x0D,_old_exc0Dh);
  DPMISetExceptionHandler(0x0E,_old_exc0Eh);
  DPMISetExceptionHandler(0x0F,_old_exc0Fh);
  DPMISetExceptionHandler(0x10,_old_exc10h);
  DPMISetExceptionHandler(0x11,_old_exc11h);
  DPMISetExceptionHandler(0x12,_old_exc12h);  