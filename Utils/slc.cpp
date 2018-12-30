#include <picture.h>
#include <fstream.h>

#define MAX_SPRITE_COUNT 1000


cRect rects[MAX_SPRITE_COUNT];
int colors[MAX_SPRITE_COUNT];
tSprite* sprites[MAX_SPRITE_COUNT];

void main(int argc,char* argv[])
{
        cout<<"Sprite list maker. ver. 0.1."<<endl;
        int warning=0,ln=0;
        if(argc<3)
        {
                cout<<"USAGE: slc <picture> <list.txt> [result.iml]"<<endl;
                return;
        }
        cPicture picture;
        cout<<"Loading picture...";
        if(!picture.load(argv[1]))
        {
                cout<<"Error: can't open picture file "<<argv[1]<<endl;
                return;
        }
        cout<<"Ok"<<endl;
        cout<<"Picture info:"<<endl
            <<"width  ="<<picture.width()<<endl
            <<"height ="<<picture.height()<<endl
            <<"bpp    ="<<picture.bpp()<<endl
            <<"Palsize="<<picture.GetPaletteSize()<<endl<<endl;
        cout<<"open list...";
        ifstream is(argv[2],ios::text);
        if(is.bad())
        {
                cout<<"Error: can't open list file "<<argv[2]<<endl;
                return;
        }
        cout<<"Ok"<<endl<<"Read:"<<endl;
        for(int i=0;is;)
        {
                ln++;
                if(i>=MAX_SPRITE_COUNT)
                {
                        cout<<"Warning:Array overflow"<<endl;
                        warning++;
                        break;
                }
                int x,y,w,h;
                is>>x>>y>>w>>h>>colors[i];
                rects[i].setup(x,y,x+w-1,y+h-1);
                if(rects[i].empty())
                {
                        cout<<"Warning: empty set at line"<<ln<<endl;
                        warning++;
                        continue;
                }
                rects[i++].dump(cout);
        }
        if(warning)
                cout<<"Warnings: "<<warning<<endl;
        else
                cout<<"Ok"<<endl;
        cout<<"List loaded"<<endl<<"Create image list...";
        tImageList* ilist=tImageList::create(i,picture.bpp(),
                                                   rects,
                                               picture.GetPaletteSize());
        if(ilist==NULL)
        {
                cout<<"failed. Program terminated"<<endl;
                return;
        }
        cout<<"Ok"<<endl;
        // palette
        RGBPalette* palette=ilist->GetPalette();
        ShiftPalette(256,(RGBStruct*)picture.GetPalette(),-2);
        if(palette!=NULL)
        {
                memcpy(palette->palette,picture.GetPalette(),palette->count*sizeof(RGBStruct));
                cout<<"Palette loaded"<<endl;
        }
        // images
        cout<<"Load images";
        tImage* p=picture.GetImage();
        for(i=0;i<ilist->count;i++)
        {
                cout<<'.';
                tImage* image=ilist->GetImage(i);
                for(int x=0;x<image->width;x++)
                        for(int y=0;y<image->height;y++)
                                image->data[x+y*image->width]=
                                        p->data[x+rects[i].left+(y+rects[i].top)*p->width];
                if((sprites[i]=(tSprite*)malloc(image->size(1)*2))==NULL)
                {
                        cout<<"Error: too low memory"<<endl;
                        return;
                }
                sprites[i]->width=image->width;
                sprites[i]->height=image->height;
                sprites[i]->x=0;
                sprites[i]->y=0;
                for(int j=0;j<sprites[i]->height;j++)
                {
                        uint16* data=(uint16*)(sprites[i]->data+j*(sprites[i]->width+5));
                        data[0]=image->width+5;
                        data[1]=image->width|0x8000;
                        uchar* dc=(uchar*)data;
                        dc[4]=colors[i];
                        memcpy(dc+5,image->data+image->width*j,image->width);
                }
        }
        tSpriteList* slist=tSpriteList::create(ilist->count,8,sprites,ilist->GetPalette());
        if(slist!=NULL)
                cout<<"Ok"<<endl;
        else
                cout<<"Failed"<<endl;
        if(slist->save(argc==4?argv[3]:"default.spl"))
                cout<<"Ok"<<endl;
        else
                cout<<"Failed"<<endl;
}