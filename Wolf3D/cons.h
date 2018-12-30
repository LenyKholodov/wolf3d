#ifndef __GAME_CONSOLE__
#define __GAME_CONSOLE__

#include <console/command.h>
#include "history.h"
#include "console.h"

#include <mscreen.h>

extern  ostream         grout;

class   Console
{
  public:
                Console(Screen*,tBinFont*);
        virtual ~Console();

        virtual void tick();
        virtual void putc(char,int key = 1);

                void open       (Screen*,tBinFont*);
                void show       ();
                void hide       ();
                void PutString  (const char*);
                void height     (int x) { grconsole.MaxHeight = x; }

        grConsole*      Graph() const { return (grConsole*)&grconsole; }
        oConsolestream& Stream()  { return conout; }

  protected:
        tHistory*       history;
        grConsole       grconsole;
        oConsolestream  conout;
        BOOL            createFlag;
        Screen*         frameScreen;
};

#endif