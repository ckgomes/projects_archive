@ECHO OFF

SET VcDir=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\
SET BuildDir=Y:\build\

SET SrcDir=Y:\game\
SET SrcFile=game.c
SET ObjFile=game.obj
SET DefFile=game.def
SET OutProgram=modc_game.dll

SET Preprocessor=/DGLEW_STATIC /DRENDERER_SOFTWARE
SET ClFlags=/FC /nologo /Zi
SET LinkFlags=/nologo /DEBUG

SET LinkLibs=Opengl32.lib glew32s.lib

if not defined DevEnvDir (
  PUSHD "%VcDir%"
    CALL vcvarsall.bat x86
  POPD
)

if not exist "%BuildDir%" mkdir "%BuildDir%"

PUSHD "%BuildDir%"

  ECHO Compiling...
  
  cl %Preprocessor% %ClFlags% -c "%SrcDir%%SrcFile%" /Fo%ObjFile%

  if errorlevel 1 (
    ECHO Compilation FAILED
    GOTO FAILED
  )

  link %LinkFlags% /DLL /def:"%SrcDir%%DefFile%" %ObjFile% %LinkLibs% /out:%OutProgram%
  ECHO.

  if errorlevel 1 (
  	ECHO Link FAILED
  	GOTO FAILED
  )
  
  ECHO Compilation finished at %time%

:FAILED

POPD

EXIT /b %errorlevel%
