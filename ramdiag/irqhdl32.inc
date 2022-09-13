;       32-битные обработчики IRQ


;       IRQ от таймера
Int20Handle_32:
        pushad

        mov     eax,[Count_32]
        inc     [Count_32]

        mov     al,20h
        out     020h,al
        out     0A0h,al
        popad
        iretd

Count_32        dd      0



;       IRQ от клавиатуры
Int21Handle_32:
        pushad
        in      al,60h
        dec     al
        jnz     .next
        int     30h
.next:
        mov     al,[pos]
        inc     [pos]

        in      al,61h
        or      al,80h
        out     61h,al
        xor     al,80h
        out     61h,al

        mov     al,20h
        out     020h,al
        out     0A0h,al
        popad
        iretd

pos     db      0



;       обработчик для остальных IRQ
OtherIRQ_32:
        pushad
        mov     ah,0
        mov     al,20
        movcl   bh,0,4
        mov     esi,AnotherInt
        call    WriteWAttr_32

        mov     al,20h
        out     020h,al
        out     0A0h,al
        popad
        iretd
AnotherInt      db 'IRQ',0
