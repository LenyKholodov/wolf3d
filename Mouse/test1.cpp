#include <vesa.h>
#include <mouse.h>

#include <iostream.h>

#include <debug.h>

Mouse           mouse;
VESAdevice      vesa;
Debug           debug;  //Сообщение в очередь DEBUG
int             mode = VESA1024x768x8;

void    main()
{

//  message_queue = 0xFFFF;

  if (vesa.open() != VESAdevice::OK)
  {
    cout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(mode) != VESAdevice::OK)
  {
    cout<<"Error open VESA mode"<<endl;
    return;
  }

  const Message*      message;

  MouseScreen      ms(vesa.CurrentScreen(),"mouse.slc");
  ms.ShowMouse();
  Mouse::SetBoundRect(0,0,2000,2000);
  while (!mouse.IsPress(Mouse::LEFT_BUTTON))
  {
    if (message_queue.IsEmpty())
       continue;
//      ms.MoveTo(Mouse::GetScanX(),Mouse::GetScanY());
    message = message_queue.GetMessage();
    if (message->id == Mouse::MOVE)
    {
      const MouseMoveMessage* mm = (MouseMoveMessage*)message;
      ms.MoveTo(mm->x,mm->y);
    }
  }
  vesa.close();
}