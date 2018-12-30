@echo off
echo make COMPACT
wmake /f make_bc MODEL_MEMORY=c
if errorlevel 1 goto ErrEnd
echo make MEDIUM
wmake /f make_bc MODEL_MEMORY=m
if errorlevel 1 goto ErrEnd
echo make SMALL
wmake /f make_bc MODEL_MEMORY=s
if errorlevel 1 goto ErrEnd
echo make LARGE
wmake /f make_bc MODEL_MEMORY=l
if errorlevel 1 goto ErrEnd
echo make HUGE
wmake /f make_bc MODEL_MEMORY=h
if errorlevel 1 goto ErrEnd
goto finish
:ErrEnd
echo error!
del *.obj
:finish