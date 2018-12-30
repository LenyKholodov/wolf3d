#ifndef __CONSOLE_MACROS__
#define __CONSOLE_MACROS__

#include "cons.h"
#include "g_types.h"

#include <cd.h>

extern          addrnode_t    servers [8];

void            InitCommand     ();
void            DefaultCommand  ();
void            LoadAutoexec    (const char* = "autoexec.cfg");

extern void     Send            (GameMessage*,int size,GameObjects);
extern void     Send            (GameMessage*,int size,ID id);
extern void     Send            (GameMessage*,int size);
extern void     ChangeScreen    (int);

extern cdromDevice* GetCDROM    ();

char*           NextWord        (char*,int&);

void            CmdBind        ( VarPool&, ParmStack&, ostream&);

void            add_GoForward   ( VarPool&, ParmStack&, ostream&);
void            sub_GoForward   ( VarPool&, ParmStack&, ostream&);
void            add_GoBack      ( VarPool&, ParmStack&, ostream&);
void            sub_GoBack      ( VarPool&, ParmStack&, ostream&);
void            add_TurnLeft    ( VarPool&, ParmStack&, ostream&);
void            sub_TurnLeft    ( VarPool&, ParmStack&, ostream&);
void            add_TurnRight   ( VarPool&, ParmStack&, ostream&);
void            sub_TurnRight   ( VarPool&, ParmStack&, ostream&);
void            add_StrafeLeft  ( VarPool&, ParmStack&, ostream&);
void            sub_StrafeLeft  ( VarPool&, ParmStack&, ostream&);
void            add_StrafeRight ( VarPool&, ParmStack&, ostream&);
void            sub_StrafeRight ( VarPool&, ParmStack&, ostream&);

void            MSMove          ( VarPool&, ParmStack&, ostream&);

void            add_Fire        ( VarPool&, ParmStack&, ostream&);
void            sub_Fire        ( VarPool&, ParmStack&, ostream&);

void            PressObject     ( VarPool&, ParmStack&, ostream&);
void            NextWeapon      ( VarPool&, ParmStack&, ostream&);
void            PrevWeapon      ( VarPool&, ParmStack&, ostream&);
void            SelectWeapon    ( VarPool&, ParmStack&, ostream&);

void            SfxVolume       ( VarPool&, ParmStack&, ostream&);
void            MusicVolume     ( VarPool&, ParmStack&, ostream&);

void            QuickSave       ( VarPool&, ParmStack&, ostream&);
void            QuickLoad       ( VarPool&, ParmStack&, ostream&);
void            Save            ( VarPool&, ParmStack&, ostream&);
void            Load            ( VarPool&, ParmStack&, ostream&);
void            Pause           ( VarPool&, ParmStack&, ostream&);
void            sub_Pause       ( VarPool&, ParmStack&, ostream&);
void            add_Pause       ( VarPool&, ParmStack&, ostream&);
void            Exit            ( VarPool&, ParmStack&, ostream&);

void            LoadLevel       ( VarPool&, ParmStack&, ostream&);

void            VID_ChangeScreen ( VarPool&, ParmStack&, ostream&);
void            VID_PicMip       ( VarPool&, ParmStack&, ostream&);

void            PresentSight     ( VarPool&, ParmStack&, ostream&);
void            CDCmd            ( VarPool&, ParmStack&, ostream&);

void            Request          ( VarPool&, ParmStack&, ostream&);
void            ConnectCmd       ( VarPool&, ParmStack&, ostream&);
void            DisconnectCmd    ( VarPool&, ParmStack&, ostream&);

#endif