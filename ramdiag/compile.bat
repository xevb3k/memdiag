cls
@echo off

rem bootsector
echo ��������� kernel.asm...OK
..\fasm\fasm kernel.asm kernel.sys > Error!.tmp
IF ERRORLEVEL 1 GOTO kernel_error

copy kernel.sys G:\
move kernel.sys ..\iso


rem main
rem echo ��������� main.asm...OK
rem ..\fasm\fasm main.asm main.bin > Error!.tmp
rem IF ERRORLEVEL 1 GOTO Main_Error

rem ��襬 ����㧮��� ��᪥��
rem echo ������ ��ࠧ� �� ��᪥��..OK
rem bootdisk
GOTO OK

:kernel_error
echo �訡�� �� �������樨 kernel.asm
GOTO Exit

rem :Main_Error
rem echo �訡�� �� �������樨 main.asm
rem GOTO Exit


:OK
del Error!.tmp
echo ----------------------------
echo ��������� �ᯥ譮 �����襭�

cd ..
call make.bat
cd ramdiag

:Exit
