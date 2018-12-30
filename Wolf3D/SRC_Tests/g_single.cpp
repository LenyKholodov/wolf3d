void            SingleGame::CreateEnv (Map* map)
{
//  int soundId = level->GetSound() ?
//                level->GetSound()->GetIndex (DOOR_OPEN_SND)
//                : 0;

  for (int y=0;y<MAP_HEIGHT;y++)
      for (int x=0;x<MAP_WIDTH;x++)
          switch (map->GetBlock (Index(y,x))->type)
          {
/*            case Map::DOOR:
            {
              CreateDoor create;

              create.param().x       = x;
              create.param().y       = y;
              create.param().side    = NORTH;
              create.resourceClass.textId  = map->GetBlock(Index(y,x))->id.door.id;
              create.resourceClass.soundId = soundId;

              new (DOOR,ID(-1,-1),server) Door (&create);

              break;
            }       */
/*            case Map::MONSTR:
            {
                CreatePlayer        create;

                create.x            = int2fixed(x);
                create.y            = int2fixed(y);
                create.angle        = 0;
                create.spriteIndex  = server->GetBlock(Index(y,x))
                                            ->id.sprite.id;


                CreateObjectMessage<CreatePlayer> createObj (ID(-1,-1));

                createObj.type      = GAME_PLAYER;
                createObj.size      = sizeof(create);
                createObj.arg       = create;

                server->PutMessage (&createObj,sizeof(createObj));


                        //server
//                new (GAME_PLAYER,ID(-1,-1),server) Player (&create);

                break;
            }*/
          }
}

/*  char shotName [128];

  LoadTable  lTable (_screen,mesPoint,
                     "fonts/trip.chr",
                     LevelCompile::ScreenShotName(levelName,shotName));

  loadTable = &lTable;

  loadTable->SetMessage (LM_LEVEL_FILE,"LOADING    ",levelName);
  loadTable->SetMessage (LM_OPERATION_ID,"LOAD LEVEL");
  loadTable->SetMessage (LM_LEVEL_NAME,"Loading...  ");
  loadTable->SetMessage (LM_CURRENT_LOAD,"UNNAMED");

  level    = LevelCompile::LoadLevel (levelName);

  if (level == NULL)
  {
    dout<<"Single game: Error open level '"<<levelName<<"'"<<endl;
    grout<<"Error open level '"<<levelName<<"'"<<endl;

    loadTable = NULL;

    Error (LEVEL_NO_MEMORY,_screen);

//    input->OnlyConsole(TRUE);

    return ;
  }

  video   = new VideoSystem (level,dest);

  server  = new GameSystem  (level,MAX_OBJECTS,QUEUE_SIZE);

        //Создаём посредника

  client  = new (SINGLE_GAME,ID(-1,-1),server)
                 SingleGameClient (this);

  server->AddAttr (client->id(),OBJ_UNLOAD_PROTECT);

  isContinue = TRUE;

        //Создаём игрока

  CreateMainPlayer (DEFAULT_PLAYER);

        //Звук

  InitSound();

  dout<<"Single game: Create system OK"<<endl;

  loadTable = NULL;*/