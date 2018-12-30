void     InputTranslator::Translate()
{
                //На первых порах сойдёт
  while (!message_queue.IsEmpty())
  {
    Message*        message = (Message*)message_queue.GetMessage();
    PlayerMessage   playerMessage (PLAYER_MOVE);

    if (message == NULL)
        return;

    switch (message->id)
    {
      case TIME_CLICK:  break; ///Сделать
      case KEY_DOWN:
      {
        KeyPress*       k_message = (KeyPress*)message;

        switch (k_message->scan)
        {
          case KEY_W:
          case KEY_CUP:     playerMessage.id   = ADD_MOVE_FORWARD;  break;
          case KEY_S:
          case KEY_CDOWN:   playerMessage.id   = ADD_MOVE_BACK;     break;
          case KEY_CLEFT:   playerMessage.id   = ADD_TURN_LEFT;     break;
          case KEY_CRIGHT:  playerMessage.id   = ADD_TURN_RIGHT;    break;
          case KEY_A:       playerMessage.id   = ADD_STRAFE_LEFT;   break;
          case KEY_D:       playerMessage.id   = ADD_STRAFE_RIGHT;  break;
          case KEY_RCTRL:
          {
            PlayerShotOn  message;

            owner->PutMessage (&message,mainPlayer,sizeof(PlayerShotOn));

            continue;
          }

          case KEY_SPACE:
            playerMessage.id   = PRESS;
            break;

          case KEY_F6:
          {
            GameSaveMessage message ("save/default.sav");

            owner->PutMessage (&message,callBack,sizeof(message));

            continue;
          }
          case KEY_F7:
          {
            GameLoadMessage message ("save/default.sav");

            owner->PutMessage (&message,callBack,sizeof(message));

            continue;
          }
          case KEY_E:
          {
            UserNextWeaponMessage message;

            owner->PutMessage (&message,mainPlayer,sizeof(message));

            continue;
          }
          case KEY_Q:
          {
            UserPrevWeaponMessage message;

            owner->PutMessage (&message,mainPlayer,sizeof(message));

            continue;
          }

          default:
          {
            KeyDownMessage      key (k_message->scan,k_message->chr);

            owner->PutMessage (&key,callBack,sizeof(key));

            continue;
          }
        }

        owner->PutMessage (&playerMessage,mainPlayer,sizeof(playerMessage));

        break;
      }

      case KEY_UP:
      {
        KeyPress*       k_message = (KeyPress*)message;

        switch (k_message->scan)
        {
          case KEY_W:
          case KEY_CUP:     playerMessage.id   = SUB_MOVE_FORWARD;    break;
          case KEY_S:
          case KEY_CDOWN:   playerMessage.id   = SUB_MOVE_BACK;       break;
          case KEY_CLEFT:   playerMessage.id   = SUB_TURN_LEFT;       break;
          case KEY_CRIGHT:  playerMessage.id   = SUB_TURN_RIGHT;      break;
          case KEY_A:       playerMessage.id   = SUB_STRAFE_LEFT;     break;
          case KEY_D:       playerMessage.id   = SUB_STRAFE_RIGHT;    break;

          case KEY_E:
          case KEY_Q:
          case KEY_SPACE:       continue;
          case KEY_F6:          continue;
          case KEY_F7:          continue;
          case KEY_RCTRL:
          {
            PlayerShotOff  message;

            owner->PutMessage (&message,mainPlayer,sizeof(PlayerShotOff));

            continue;
          }

          default:
          {
            KeyUpMessage      key (k_message->scan);

            owner->PutMessage (&key,callBack,sizeof(key));

            continue;
          }
        }

        owner->PutMessage (&playerMessage,mainPlayer,sizeof(playerMessage));

        break;
      }
      case MOUSE_MOVE:
      {
           MouseMoveMessage* mes = (MouseMoveMessage*)message;

           if (mouseX == -1 && mouseY == -1)
           {
             mouseX = mes->x;
             mouseY = mes->y;

             break;
           }
           else
           {
             int dx = mes->_x - mouseX,
                 dy = mes->_y - mouseY;

             mouseX = mes->_x;
             mouseY = mes->_y;

                        //Мышь в центре

             playerMessage.id    = PLAYER_MOVE;
             playerMessage.dist  = 0;//-int2fixed(dy)>>8;
             playerMessage.angle = dx<<4;

             owner->PutMessage (&playerMessage,
                                mainPlayer,
                                sizeof(playerMessage));

             break;
           }
      }
      case MOUSE_LBUTTON_DOWN:
      {
            PlayerShotOn  shot;

            owner->PutMessage (&shot,mainPlayer,sizeof(PlayerShotOn));

            break;
      }
      case MOUSE_LBUTTON_UP:
      {
            PlayerShotOff shot;

            owner->PutMessage (&shot,mainPlayer,sizeof(PlayerShotOff));

            break;
      }
    }
  }
}