@for %%a in ( *.bmp) do conv.exe %%a %%a
@for %%a in ( *.bmp) do makeflmp /p  %%a
@copy /b *.lmp %1 /y
@del *.lmp