#include <tables.h>
#include <math.h>
#include <debug.h>
#include <iostream.h>

void    main()
{
//        InitTables();
        cout<<"Angle: Testing sinus table"<<endl;
        int error = 0;
        Angle ang;
        for (long i=0;i<0xFFFF;i++)
        {
            ang = i;
            if (fabs(fixed2float(sin(ang))-sin(angle2float(ang)))>=0.001)
            {
                        D_String("Angle: Error in angle table");
                        cout<<"Angle: angle             = "<<angle2float(ang)<<endl;
                        cout<<"Angle: table  sin(angle) = "<<fixed2float(sin(ang))<<endl;
                        cout<<"Angle: double sin(angle) = "<<sin(angle2float(ang))<<endl<<endl;
                        error++;
            }
        }

        cout<<"Angle: Testing cosinus table"<<endl;
        for (i=0;i<0xFFFF;i++)
        {
            ang = i;
            if (fabs(fixed2float(cos(ang))-cos(angle2float(ang)))>=0.001)
            {
                        D_String("Angle: Error in angle table");
                        cout<<"Angle: angle             = "<<angle2float(ang)<<endl;
                        cout<<"Angle: table  cos(angle) = "<<fixed2float(cos(ang))<<endl;
                        cout<<"Angle: double cos(angle) = "<<cos(angle2float(ang))<<endl<<endl;
                        error++;
            }
        }

        cout<<"Angle: Testing tangens table"<<endl;
        for (i=0;i<0xFFFF;i++)
        {
            ang = i;
            if (fabs(fixed2float(tan(ang))/tan(angle2float(ang)))>=1.01)
            {
                        D_String("Angle: Error in angle table");
                        cout<<"Angle: angle             = "<<angle2float(ang)<<endl;
                        cout<<"Angle: table  tan(angle) = "<<fixed2float(tan(ang))<<endl;
                        cout<<"Angle: double tan(angle) = "<<tan(angle2float(ang))<<endl<<endl;
                        error++;
            }
        }

        cout<<"Angle: Testing inv sinus table"<<endl;
        for (i=0;i<0xFFFF;i++)
        {
            ang = i;
            if (fabs(fixed2float(isin(ang))/isin(angle2float(ang)))>=1.1)
            {
                        D_String("Angle: Error in angle table");
                        cout<<"Angle: angle             = "<<angle2float(ang)<<endl;
                        cout<<"Angle: table  inv sin(angle) = "<<fixed2float(isin(ang))<<endl;
                        cout<<"Angle: double inv sin(angle) = "<<1.0/sin(angle2float(ang))<<endl<<endl;
                        error++;
            }
        }

        cout<<"All tables are OK"<<endl;
        cout<<"Errors: "<<error<endl;
}