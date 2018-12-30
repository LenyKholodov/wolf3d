  REM Далeе располагаются классы игровых ресурсов

  REM Классы оружия

  USE_LIST GAME_WEAPON_LIST GW
           "killer.wpn"
           "gun.wpn"
           "auto_pos.wpn"
           "duplet.wpn"
           "automat.wpn"
           "skmat.wpn"
           "bazuka.wpn"
  END

  REM   Классы того, что на полу валяется

  USE_LIST GAME_SUBJECT_LIST SBJ
           "automat.box"
           "hpack.box"
           "killer.box"
           "gun.box"
           "armor1.box"
           "bazuka.box"
  END

  REM   Классы всего того, что осталось - и слов для него нет

  USE_LIST GAME_OTHERS_LIST OTH
           "door1.gdr"
  END

  REM   Классы игроков (в смысле skin'ов)

  USE_LIST GAME_PLAYER_LIST PL
           "play.plr"
           "monstr.plr"
           "diablo.plr"
           "intel.plr"
           "niger.plr"
           "sarg.plr"
  END

  REM   Классы телепортов (*но не порталов*)
  REM   Телепорты - это то что работает в границах одного уровня
  REM   Потралы загружают другой уровень и являются элементами перехода между уровнями

  USE_LIST GAME_TELEPORT_LIST TLP
           "telep1.tlp"
           "portal1.tlp"
  END

  REM   Классы эффектов

  USE_LIST GAME_EFFECT_LIST EF
           "fak.spr"
           "light1.lt"
           "tree1.spr"
           "chair.spr"
           "coat.spr"
  END

