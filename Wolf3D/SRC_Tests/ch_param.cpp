        case KEY_INS:
          draw->FloorPtr()->SetParam(Floor::D0,floorHeight-=FLOOR_HEIGHT_STEP);
          draw->FloorPtr()->Reset();
          PrintFloor(floorHeight,floorC);
          break;
        case KEY_DEL:
          draw->FloorPtr()->SetParam(Floor::D0,floorHeight+=FLOOR_HEIGHT_STEP);
          draw->FloorPtr()->Reset();
          PrintFloor(floorHeight,floorC);
          break;
        case KEY_O:
          draw->FloorPtr()->SetParam(Floor::C,floorC-=FLOOR_C_STEP);
          draw->FloorPtr()->Reset();
          PrintFloor(floorHeight,floorC);
          break;
        case KEY_L:
          draw->FloorPtr()->SetParam(Floor::C,floorC+=FLOOR_C_STEP);
          draw->FloorPtr()->Reset();
          PrintFloor(floorHeight,floorC);
          break;
        case KEY_END:
          draw->StagePtr()->SetParam(RayCast::FOCUS,rcFocus-=RC_FOCUS_STEP);
          draw->StagePtr()->Reset();
          break;
        case KEY_HOME:
          draw->StagePtr()->SetParam(RayCast::FOCUS,rcFocus+=RC_FOCUS_STEP);
          draw->StagePtr()->Reset();
          break;
        case KEY_MINUS:
          draw->StagePtr()->SetParam(RayCast::VIEW_ANGLE,rcVAngle-=RC_VANGLE_STEP);
          draw->StagePtr()->Reset();
          break;
        case KEY_PLUS:
          draw->StagePtr()->SetParam(RayCast::VIEW_ANGLE,rcVAngle+=RC_VANGLE_STEP);
          draw->StagePtr()->Reset();
          break;