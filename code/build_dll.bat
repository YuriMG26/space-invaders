@echo off

set i_p=c:\handmade\include\
set l_p=c:\handmade\lib\
set t_date=%time:~3,2%%time:~6,2%

pushd ..\build\
del *.pdb > NUL 2> NUL
cl -MT -nologo -Gm- -GR- -EHa- -O2 -FC -W2 -Z7 -I%i_p% ..\code\Game.c ..\code\Logger.c -Fmgame.map /LD /link /ignore:4098 -LIBPATH:%l_p% -incremental:no -opt:ref -PDB:game_%t_date%.pdb  raylib.lib gdi32.lib winmm.lib user32.lib shell32.lib opengl32.lib msvcrt.lib
popd
