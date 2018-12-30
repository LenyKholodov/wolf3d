#include <mouse.h>
#include <keyboard.h>
#include <timer.h>

const Message* message;
Keyboard       kb;
Mouse          mouse;
SystemTimer    timer;

void    main()
{
  message_queue = 0xFFF;

  timer.SetRefreshTime(CLICK_256);

  Mouse::MoveTo(0,0);

  int   flag = 1;

  while (flag)
  {
    if (!message_queue.IsEmpty())
    {
        message = message_queue.GetMessage();

        switch (message->id)
        {
           case Mouse::MOVE:
           {
                MouseMoveMessage* mes = (MouseMoveMessage*)message;
                cout<<"Event: Mouse Move";
                cout<<"\t\tX   = "<<mes->x<<" Y    = "<<mes->y
                    <<" _X "<<mes->_x<<" _Y "<<mes->_y<<endl;
                break;
           }

           case Mouse::LBUTTON_DOWN:
           {
                ButtonDownMessage* mes = (ButtonDownMessage*)message;
                cout<<"Event: Left Button Down";
                cout<<"\t\tX   = \t"<<mes->x<<" \tY    = \t"<<mes->y<<endl;
                break;
           }
           case Mouse::LBUTTON_UP:
           {
                ButtonUpMessage* mes = (ButtonUpMessage*)message;
                cout<<"Event: Left Button Up";
                cout<<"\t\tX   = \t"<<mes->x<<" \tY    = \t"<<mes->y<<endl;
                break;
           }
           case Mouse::RBUTTON_DOWN:
           {
                ButtonDownMessage* mes = (ButtonDownMessage*)message;
                cout<<"Event: Right Button Down";
                cout<<"\tX   = \t"<<mes->x<<" \tY    = \t"<<mes->y<<endl;
                break;
           }
           case Mouse::RBUTTON_UP:
           {
                ButtonUpMessage* mes = (ButtonUpMessage*)message;
                cout<<"Event: Right Button Up";
                cout<<"\t\tX   = \t"<<mes->x<<" \tY    = \t"<<mes->y<<endl;
                break;
            }
           case Mouse::CBUTTON_DOWN:
            {
                ButtonDownMessage* mes = (ButtonDownMessage*)message;
                cout<<"Event: Center Button Down";
                cout<<"\tX   = \t"<<mes->x<<" \tY    = \t"<<mes->y<<endl;
                break;
            }
           case Mouse::CBUTTON_UP:
           {
                ButtonUpMessage* mes = (ButtonUpMessage*)message;
                cout<<"Event: Center Button Up";
                cout<<"\tX   = \t"<<mes->x<<" \tY    = \t"<<mes->y<<endl;
                break;
           }
           case KEY_DOWN:
           {
                KeyDown*         mes = (KeyDown*)message;
                cout<<"Event: Key down";
                cout<<"\t\t\tkey =  \t"<<kb.GetKeyName(mes->scan);
                cout<<"\tscan =\t"<<mes->scan<<endl;
                break;
           }
           case KEY_UP:
           {
                KeyUp*         mes = (KeyUp*)message;
                cout<<"Event: Key up";
                cout<<"\t\t\tkey =  \t"<<kb.GetKeyName(mes->scan);
                cout<<"\tscan =\t"<<mes->scan<<endl;

                if (mes->scan == KEY_ESC)
                   flag = 0;

                break;
           }
           case TIME_CLICK:
           {
                TickMessage*   mes = (TickMessage*)message;
                cout<<"Event: Timer click";
                cout<<"\t\t\ttick = "<<mes->num<<endl;
           }
        }

    }
  }
}