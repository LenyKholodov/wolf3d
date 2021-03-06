  REM ���e� �ᯮ�������� ������ ��஢�� ����ᮢ

  REM ������ ��㦨�

  INCLUDE "source/include/weapons/shotgun.inc"
  INCLUDE "source/include/weapons/automat.inc"
  INCLUDE "source/include/weapons/auto_pos.inc"
  INCLUDE "source/include/weapons/skmat.inc"
  INCLUDE "source/include/weapons/killer.inc"
  INCLUDE "source/include/weapons/gun.inc"
  INCLUDE "source/include/weapons/bazuka.inc"

  USE_LIST GAME_WEAPON_LIST GW
           "killer.wpn"
           "gun.wpn"
           "auto_pos.wpn"
           "duplet.wpn"
           "automat.wpn"
           "skmat.wpn"
           "bazuka.wpn"
  END

  REM   ������ ��ப�� (� ��᫥ skin'��)

  INCLUDE "source/include/skins/player.inc"
  INCLUDE "source/include/skins/monstr.inc"
  INCLUDE "source/include/skins/diablo.inc"
  INCLUDE "source/include/skins/intel.inc"
  INCLUDE "source/include/skins/niger.inc"
  INCLUDE "source/include/skins/sarg.inc"

  USE_LIST GAME_PLAYER_LIST PL
           "play.plr"
           "monstr.plr"
           "diablo.plr"
           "intel.plr"
           "niger.plr"
           "sarg.plr"
  END

  REM   ������ �ᥣ� ⮣�, �� ��⠫��� - � ᫮� ��� ���� ���

  INCLUDE "source/include/others/door1.inc"

  USE_LIST GAME_OTHERS_LIST OTH
           "door1.gdr"
  END

  REM   ������ ⮣�, �� �� ���� �������

  INCLUDE "source/include/subj/automat.inc"
  INCLUDE "source/include/subj/hpack.inc"
  INCLUDE "source/include/subj/duplet.inc"
  INCLUDE "source/include/subj/killer.inc"
  INCLUDE "source/include/subj/gun.inc"
  INCLUDE "source/include/subj/armor1.inc"
  INCLUDE "source/include/subj/bazuka.inc"

  USE_LIST GAME_SUBJECT_LIST SBJ
           "automat.box"
           "hpack.box"
           "duplet.box"
           "killer.box"
           "gun.box"
           "armor1.box"
           "bazuka.box"
  END

  REM   ������ ⥫����⮢ (*�� �� ���⠫��*)
  REM   ��������� - �� � �� ࠡ�⠥� � �࠭��� ������ �஢��
  REM   ���ࠫ� ����㦠�� ��㣮� �஢��� � ����� ������⠬� ���室� ����� �஢�ﬨ

  INCLUDE "source/include/portals/portal1.inc"
  INCLUDE "source/include/portals/telep1.inc"

  USE_LIST GAME_TELEPORT_LIST TLP
           "telep1.tlp"
           "portal1.tlp"
  END

  REM   ������ ��䥪⮢

  INCLUDE "source/include/effect/world.inc"
  INCLUDE "source/include/effect/light1.inc"

  USE_LIST GAME_EFFECT_LIST EF
           "fak.spr"
           "light1.lt"
           "tree1.spr"
           "chair.spr"
           "coat.spr"
  END
