@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

set opts=-FC -GR- -EHa- -nologo -Zi -std:c17 -O2 -Oi
set code=%cd%
pushd bin
cl %opts% %code%\main.c -Fefcc.exe
popd
