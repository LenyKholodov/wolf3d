#include <iostream.h>
#include <dos.h>
#include <dpmi32gw.h>

int mouse_event  = 0;
int mouse_code   = 0;
int mouse_cx     = 0;
int mouse_dx     = 0;
int right_button =0;

#pragma off (check_stack)
void _loadds far click_handler (int max, int mcx, int mdx)
{
#pragma aux click_handler parm [EAX] [ECX] [EDX]
        mouse_event = 1;
        mouse_code = max;
        mouse_cx = mcx;
        mouse_dx = mdx;
        right_button = 1;
}
#pragma on (check_stack)


void    main()
{
         REGS   regs;
         SREGS  sregs;

         regs.w.ax      = 0xC;
         regs.w.cx      = 0xFF;
         regs.x.edx     = FP_OFF( click_handler );
         sregs.es       = FP_SEG( click_handler );

         int386x( 0x33, &regs, &regs, &sregs );

         while (!kbhit())
         {
//             if (!mouse_event)
//                continue;
             if (mouse_code & 1)
              cout<< "Mouse move"<<endl;
             if (mouse_code & 2)
                 cout<< "Left button down"<<endl;
             if (mouse_code & 4)
                 cout<< "Left button up"<<endl;
             if (mouse_code & 8)
                 cout<< "Right button down"<<endl;
             if (mouse_code & 16)
                 cout<< "Right button up"<<endl;
             if (mouse_code & 32)
                 cout<< "Center button down"<<endl;
             if (mouse_code & 64)
                 cout<< "Center button up"<<endl;
             mouse_code = 0;
         }
         cout<<right_button<<endl;

         regs.w.ax      = 0xC;
         regs.w.cx      = 0;
         regs.x.edx     = FP_OFF( click_handler );
         sregs.es       = FP_SEG( click_handler );

         int386x( 0x33, &regs, &regs, &sregs );
}