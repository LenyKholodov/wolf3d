ID      TypeTable::Include (GameObjects type,PoolIndex index)
{
  int   iIndex = -1;
  ID    id = Include (mTypes[type],index);

  if (id.id >= 0 && id.id<MAX_SLOTS)
  {
     dout<<"ID = ("<<id.type<<","<<id.id<<")"<<endl;
     return id;
  }
  else
  {
    int slot;

    if ((slot = RegisterNewType (type))==-1)
    {
      id.type = -1;
      id.id   = -1;

      return id;
    }
    else
    {
        dout<<"!!!!!!!!!!!!!!!!"<<endl;

        mTypes   [type]               = slot;

        iIndex                        = mTable[slot][0].next;

        mTable   [slot][iIndex].data  = index;
        mTable   [slot][0].next       = mTable[slot][iIndex].next;

        mSize    [slot]               = mSize [slot] + 1;

        id.type = slot;
        id.id   = iIndex;

        return id;
    }

    id.type = -1;
    id.id   = -1;

    return id;
  }
}

ID       TypeTable::Include (int iTable,PoolIndex index)
{
  int iIndex = -1;

  if (iTable >=0 && iTable < MAX_TYPES)
  {
    if (mSize [iTable] < MAX_SLOTS && mSize [iTable] > 0)
    {
//       if ( (iIndex = mTable[iTable][0].next) != -1)
//       {
          iIndex = mTable[iTable][0].next;

          mTable   [iTable][iIndex].data = index;
          mTable   [iTable][0].next      = mTable[iTable][iIndex].next;

          mSize    [iTable]              = mSize [iTable] + 1;

          ID      id;

          id.type = iTable;
          id.id   = iIndex;

          return id;
//       }
    }
  }

  ID  id;

  id.type = -1;
  id.id   = -1;

  return id;
}
