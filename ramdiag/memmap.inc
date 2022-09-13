;       вывод полной карты распределения памяти

        MapLen          equ     56
        MapHigh         equ     23
        MapX            equ     (80 - MapLen) / 2
        MapY            equ     (25 - MapHigh) / 2
ShowMemMapFull:
        call    ScreenSave
        stdcall Bar_64, MapX,    MapY,  MapLen, MapHigh,  clGray,  clBlack
        stdcall Bar_64, MapX,    MapY,  MapLen, 1,        clGreen, clBlack
        stdcall Write_64, txt_MemMapWindowTitle, MapX + 1,  MapY,     clNone,clNone,cNoAttr,cFwd
        stdcall Write_64, txt_Start,             MapX + 1,  MapY + 1, clNone,clRed,cSym,cFwd
        stdcall Write_64, txt_End,               MapX + 18, MapY + 1, clNone,clRed,cSym,cFwd
        stdcall Write_64, txt_Size,              MapX + 35, MapY + 1, clNone,clRed,cSym,cFwd
        stdcall Write_64, txt_Type,              MapX + 47, MapY + 1, clNone,clRed,cSym,cFwd
        stdcall Write_64, txt_ssMemShow, 0, 24,  clNone, clNone, cNoAttr, cFwd
        mov     rcx,[MemMapEntryCount]
        cmp     rcx,21
        jbe     .NoCut
        mov     rcx,21
.NoCut:
        xor     rsi,rsi
        mov     rax,MapY + 2
.NextEntry:
        mov     r8,qword [MemMap + rsi]
        mov     r9,qword [MemMap + rsi + 8]
        xor     r11,r11
        mov     r11d,dword [MemMap + rsi + 16]
        dec     r11
        mov     r12,[MemType1 + r11 * 8]
        mov     r11,[Mem1 + r11d * 8]
        stdcall WriteHEX_64, r8,        MapX + 1,  rax, clNone, r11, cSym, 16, cFwd
        lea     r10,[r8+r9-1]
        stdcall WriteHEX_64, r10,       MapX + 18, rax, clNone, r11, cSym, 16, cFwd
        stdcall Write_64,    txt_Razd,  MapX + 17, rax, clNone, r11, cSym,     cFwd
        stdcall Write_64,    txt_Kb,    MapX + 44, rax, clNone, r11, cSym, cFwd
        shr     r9,10
        stdcall WriteDEC_64, r9,        MapX + 42, rax, clNone, r11, cSym, cBack
        stdcall Write_64,    r12,       MapX + 47, rax, clNone, r11, cSym, cFwd
        inc     rax
.NoView:
        add     rsi,24
        dec     rcx
        jnz     .NextEntry
.NoKeyPressed:
        call    KeyPressed_64
        test    rax,rax
        jz      .NoKeyPressed
        call    KbdPop_64      ; если да - извлекаем скан из буфера
        cmp     rax,57h
        jne     .NoKeyPressed
        call    ScreenRestore
        stdcall Write_64, txt_ssProgStart, 0, 24,  clNone, clNone, cNoAttr, cFwd
        ret

Mem1            dq      clWhite
Mem2            dq      clBlack
Mem3            dq      clBlack
Mem4            dq      clBlack
Mem5            dq      clRed

MemType1        dq      txt_Type1
MemType2        dq      txt_Type2
MemType3        dq      txt_Type3
MemType4        dq      txt_Type4
MemType5        dq      txt_Type5
