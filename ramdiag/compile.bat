cls
@echo off

rem bootsector
echo Компиляция kernel.asm...OK
..\fasm\fasm kernel.asm kernel.sys > Error!.tmp
IF ERRORLEVEL 1 GOTO kernel_error

copy kernel.sys G:\
move kernel.sys ..\iso


rem main
rem echo Компиляция main.asm...OK
rem ..\fasm\fasm main.asm main.bin > Error!.tmp
rem IF ERRORLEVEL 1 GOTO Main_Error

rem Пишем загрузочную дискету
rem echo Запись образа на дискету..OK
rem bootdisk
GOTO OK

:kernel_error
echo Ошибка при компиляции kernel.asm
GOTO Exit

rem :Main_Error
rem echo Ошибка при компиляции main.asm
rem GOTO Exit


:OK
del Error!.tmp
echo ----------------------------
echo Компиляция успешно завершена

cd ..
call make.bat
cd ramdiag

:Exit
