@ECHO OFF

SET VcDir=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\
SET BuildDir=Y:\build\

SET SrcDir=Y:\win32\
SET SrcFile=win32_main.c
SET ObjFile=win32_main.obj
SET OutProgram=modc_loader.exe

SET Preprocessor=/DRENDERER_SOFTWARE
SET ClFlags=/FC /nologo /Zi
SET LinkFlags=/nologo /DEBUG

SET LinkLibs=User32.lib Gdi32.lib OpenGL32.lib

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

  link %LinkFlags% %ObjFile% %LinkLibs% /out:%OutProgram%
  ECHO.

  if errorlevel 1 (
  	ECHO Link FAILED
  	GOTO FAILED
  )
  
  ECHO Compilation finished at %time%
  ECHO.

:FAILED

POPD

EXIT /b %errorlevel%
