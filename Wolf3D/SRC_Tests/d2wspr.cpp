tImage* drawSprite (const char* _name,int dir)
{
  tPic* pic              = wad->loadPic (_name);

  if (pic == NULL)
  {
     dout<<"Error load resource "<<_name<<endl;
     return NULL;
  }

  tImage* image = (tImage*)malloc (tImage::size(pic->width,pic->height,BPP_8bit));

  image->width  = pic->width;
  image->height = pic->height;

  memset (image->data,z_color,pic->width*pic->height);

  PicColumn* column     = (PicColumn*)(pic->data+pic->colOffsets[0]);

  for (int x=0;x<pic->width;x++)
  {
      column            = (PicColumn*)(pic->data+pic->colOffsets[x]);

      char*      src    = column->pixels+column->row;
      uchar*     dest   = dir == 1 ? image->data+column->row*image->width+x :
                          image->data+column->row*image->width+pic->width-x-1;

      while (column->row != 0xFF)
      {
        for (int y=0;y<column->nonTransparentPixels;y++,src++,dest+=image->width)
        {
           *dest = rgb(palette[*src].red,
                       palette[*src].green,
                       palette[*src].blue
                      );
        }

        column            = (PicColumn*)((char*)column +
                                         column->nonTransparentPixels + 4);
      }
  }

  return image;
}