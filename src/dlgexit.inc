        ExitLen         equ     20
        ExitHigh        equ     6
        ExitX           equ     (80 - ExitLen) /2
        ExitY           equ     (25 - ExitHigh) /2
        ActiveBtn       equ     clRed
        PassiveBtn      equ     clGray
        CurChoice        dq      0
        YesBtnColor      dq      0
        NoBtnColor       dq      0
Exit_Dialog:
        call    ScreenSave
        stdcall Bar_64, ExitX,    ExitY,  ExitLen, ExitHigh,  PassiveBtn,  clBlack
        stdcall Bar_64, ExitX,    ExitY,  ExitLen, 1,        clGreen, clBlack
        stdcall Write_64, txt_ExitDialogTitle,   ExitX + 1,  ExitY,     clNone,clNone,cNoAttr,cFwd
        stdcall Write_64, txt_ExitMessage,   ExitX + 1,  ExitY + 2,     clNone,clNone,cNoAttr,cFwd
        stdcall Write_64, txt_ssExitDlg, 0, 24,  clNone, clNone, cNoAttr, cFwd
.NoKeyPressed:
        mov     [YesBtnColor],ActiveBtn
        mov     [NoBtnColor],PassiveBtn
        cmp     [CurChoice],1
        je      .SkipColor
        mov     [YesBtnColor],PassiveBtn
        mov     [NoBtnColor],ActiveBtn
.SkipColor:
        stdcall Write_64, txt_Yes,   ExitX + 1,            ExitY + 4,     [YesBtnColor],clNone,cFon,cFwd
        stdcall Write_64, txt_No,    ExitX + ExitLen - 2,  ExitY + 4,     [NoBtnColor],clNone,cFon,cBack
        call    KeyPressed_64
        test    rax,rax
        jz      .NoKeyPressed
        call    KbdPop_64
        cmp     rax,4Bh
        jne     .NoLeft
        xor     [CurChoice],1
.NoLeft:
        cmp     rax,4Dh
        jne     .NoRight
        xor     [CurChoice],1
.NoRight:
        cmp     rax,1Ch
        jne     .NoKeyPressed
        mov     rax,[CurChoice]
        call    ScreenRestore
        stdcall Write_64, txt_ssProgStart, 0, 24,  clNone, clNone, cNoAttr, cFwd
        ret

txt_ExitDialogTitle     db      'Exit',0
txt_ExitMessage         db      'PC will reboot',0
txt_Yes                 db      ' Yes ',0
txt_No                  db      ' No ',0
