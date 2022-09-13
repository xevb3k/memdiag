;       Обработчики исключений для LM

ExcWindowLength         =       44
ExcWindowHeight         =       6
ExcWindowColor          =       clGray
ExcWindowTitleBarColor  =       clRed
ExcWindowTitleTextColor =       clWhite

ExcNumberText           db      'Exception number:',0
ExcNameText             db      'Exception name:',0
ExcErrCodeText          db      'Error code:',0
ExcHaltText             db      'The system is halted!',0

Ext00_Name              db      'Divide by zero (#DE)',0
Ext01_Name              db      'Debug (#DB)',0
Ext02_Name              db      'NMI (#NMI)',0
Ext03_Name              db      'Breakpoint (#BP)',0
Ext04_Name              db      'Overflow (#OF)',0
Ext05_Name              db      'Bound range (#BR)',0
Ext06_Name              db      'Invalid opcode (#UD)',0
Ext07_Name              db      'Device not available (#NM)',0
Ext08_Name              db      'Double fault (#DF)',0
Ext0A_Name              db      'Invalid TSS (#TS)',0
Ext0B_Name              db      'Segment not present (#NP)',0
Ext0C_Name              db      'Stack error (#SS)',0
Ext0D_Name              db      'General protection (#GP)',0
Ext0E_Name              db      'Page fault (#PF)',0
Ext10_Name              db      'x87 error (#MF)',0
Ext11_Name              db      'Alignment check (#AC)',0
Ext12_Name              db      'Machine check (#MC)',0
Ext13_Name              db      'SIMD error (#XF)',0


Int00Handle_64:
        xor     r10,r10
        xor     r12,r12
        mov     rdi,Ext00_Name
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int01Handle_64:
        mov     rdi,Ext01_Name
        xor     r10,r10
        inc     r10
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int02Handle_64:
        mov     rdi,Ext02_Name
        mov     r10,2
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int03Handle_64:
        mov     rdi,Ext03_Name
        mov     r10,3
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int04Handle_64:
        mov     rdi,Ext04_Name
        mov     r10,4
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int05Handle_64:
        mov     rdi,Ext05_Name
        mov     r10,5
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int06Handle_64:
        mov     rdi,Ext06_Name
        mov     r10,6
        call    ExceptionWindowDraw
        cli
        hlt
        iretq


Int07Handle_64:
        mov     rdi,Ext07_Name
        mov     r10,7
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int08Handle_64:
        pop     r12
        mov     rdi,Ext08_Name
        mov     r10,8
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int0AHandle_64:
        pop     r12
        mov     rdi,Ext0A_Name
        mov     r10,0Ah
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int0BHandle_64:
        pop     r12
        mov     rdi,Ext0B_Name
        mov     r10,0Bh
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int0CHandle_64:
        pop     r12
        mov     rdi,Ext0C_Name
        mov     r10,0Ch
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int0DHandle_64:
        pop     r12
        mov     rdi,Ext0D_Name
        mov     r10,0Dh
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int0EHandle_64:
        pop     r12
        mov     rax,cr2
        mov     rdi,Ext0E_Name
        mov     r10,0Eh
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int10Handle_64:
        mov     rdi,Ext10_Name
        mov     r10,10h
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int11Handle_64:
        pop     r12
        mov     rdi,Ext11_Name
        mov     r10,11h
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int12Handle_64:
        mov     rdi,Ext12_Name
        mov     r10,12h
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


Int13Handle_64:
        mov     rdi,Ext13_Name
        mov     r10,13h
        call    ExceptionWindowDraw

        cli
        hlt
        iretq


;       r10     - номер исключения
;       r12     - номер ошибки
;       rdi     - название исключения
ExceptionWindowDraw:
WinX    equ     (80-ExcWindowLength)/2
WinY    equ     (25-ExcWindowHeight)/2
        ;       окно
        stdcall Bar_64, WinX, WinY, ExcWindowLength, ExcWindowHeight, ExcWindowColor, clBlack
        ;       панель заголовка
        stdcall Bar_64, WinX, WinY, ExcWindowLength, 1, ExcWindowTitleBarColor, ExcWindowTitleTextColor
        ;       заголовок
        stdcall Write_64, ExcRaisedMessage, WinX + 1, WinY, 0,0,0,0
        ;       надпись 1
        stdcall Write_64, ExcNumberText,       WinX + 1, WinY + 2, 0,0,0,0
        ;       надпись 2
        stdcall Write_64,ExcNameText, WinX + 1, WinY + 3, 0,0,0,0
        ;       надпись 3
        stdcall Write_64, ExcErrCodeText, WinX + 1, WinY + 4, 0,0,0,0
        ;       номер исключения
        stdcall WriteHEX_64, r10, WinX + 19, WinY + 2, 0,0,0,16, cFwd
        ;       название исключения
        stdcall Write_64, rdi, WinX + 19, WinY + 3, 0,0,0,0
        ;       номер ошибки
        stdcall WriteHEX_64, r12, WinX + 19, WinY + 4, 0,0,0,16, cFwd

;        stdcall WriteHEX_64, rax, WinX + 19, WinY + 5, 0,0,0,16, cFwd
        ret

ExcRaisedMessage     db      'Exception raised',0
