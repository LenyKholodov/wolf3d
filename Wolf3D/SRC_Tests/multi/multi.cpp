/*
      level    = LevelCompile::LoadLevel (levelName);

      if (level == NULL)
      {
        dout<<"Multi game: Error open level '"<<levelName<<"'"<<endl;
        grout<<"Error open level '"<<levelName<<"'"<<endl;

        delete fin;
      }

      farClient = message->client;
      player    = message->mainPlayer;

      net       = new Net (MAX_NET_SEQUENCE,message->addr,CLIENT_WRITE,CLIENT_READ);

      server    = new GameClient  (this,message->addr,level);
      video     = new VideoSystem (level,_screen);
      input     = new (farClient,farClient,(GameSystem*)server) InputTranslator (player);
      view      = new (farClient,farClient,(GameSystem*)server) GameView        (video,player);

      InitSound();

      isNet       = TRUE;
      readAnswer  = clock();
      writeAnswer = clock();

        if (video)  delete video;
        if (server) delete server;
        if (level)  delete level;
        if (net)    delete net;

        video      = NULL;
        view       = NULL;
        server     = NULL;
        input      = NULL;
        net        = NULL;
        level      = NULL;
        farClient  = ID(-1,-1);
        player     = ID(-1,-1);

        addrnode_t addr ;

        server  = new GameClient  (this,addr,level);
        console = new Console     (_screen,tBinFont::load("fonts/font.fnt"));
        console->height           (_screen->height());
        console->show();

        input   = new (farClient,farClient,(GameSystem*)server) InputTranslator (player);
        input->OnlyConsole (TRUE);

        isNet      = FALSE;

  addrnode_t addr ;

  server  = new GameClient  (this,addr,level);
  console = new Console     (dest,tBinFont::load("fonts/font.fnt"));
  console->height (dest->height());
  console->show();

  input   = new (farClient,farClient,(GameSystem*)server) InputTranslator (player);
  input->OnlyConsole (TRUE);

*/