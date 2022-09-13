;       64-битные обработчики IRQ

align   8

;       IRQ от таймера
Int20Handle_64:
        push    rax rbx

        cmp     [StikDelay],2
        jne     .Exit

        mov     rbx,[CurStik]
        lea     rax,[rbx + Stiks]
        stdcall Write_64, rax, 79, 0, clNone, clNone, cNoAttr, cFwd
        add     rbx,2
        cmp     rbx,8
        jne     .NoFlush
        xor     rbx,rbx
.NoFlush:
        mov     qword [CurStik],rbx
        mov     qword [StikDelay],0
.Exit:
        inc     qword [StikDelay]


        ;       EOI
        mov     al,20h
        out     020h,al
        out     0A0h,al
        pop     rbx rax
        iretq

CurStik         dq      0
Stiks           db      '',0
                db      '',0
                db      '',0
                db      '',0
StikDelay       dq      0


BufferLength    =       16
WaitForGray     db      0

align   8

;       IRQ от клавиатуры
Int21Handle_64:
        push    rax rsi rcx r10 rbx r8
        ;       читаем скан-код
        xor     rax,rax
        in      al,60h
        ;       если ждем повторный код
        cmp     [WaitForGray],1
        jne      .NoWaitForGray
        cmp     al,80h
        jae     .NoPushLeaveGray
        call    KbdPush_64
.NoPushLeaveGray:
        mov     [WaitForGray],0
        jmp     .NoPush  ; и на выход
.NoWaitForGray:
        ;       иначе проверяем, если нажата серая
        cmp     al,0E0h
        jne     .NoGray
        mov     [WaitForGray],1   ; будем ждать следующего скана
        jmp     .NoPush           ; выходим, не трогая буфер
.NoGray:
        ;       иначе проверяем, если это нажатие обычной клавиши - в буффер
        cmp     al,80h
        jb      .Push
        jmp     .NoPush

.Push:
        call    KbdPush_64

.NoPush:
        ;       изменить бит состояния клавиатуры
        in      al,61h
        or      al,80h
        out     61h,al
        xor     al,80h
        out     61h,al

        ;       EOI обоим PIC
        mov     al,20h
        out     020h,al
        out     0A0h,al

        pop     r8 rbx r10
        pop     rcx rsi rax
        iretq

;       ПОМЕЩАЕТ ЭЛЕМЕНТ В ОЧЕРЕДЬ
;       al      - помещаемый элемент
KbdPush_64:
        push    rdi
        mov     rdi,[KbdEnd]
        mov     byte [rdi+KbdBuffer],al
        inc     rdi
        cmp     rdi,BufferLength
        jnz     .NoLoop
        xor     rdi,rdi
.NoLoop:
        mov     [KbdEnd],rdi
        pop     rdi
        ret

;       ИЗВЛЕКАЕТ СКАН ИЗ ОЧЕРЕДИ в al
KbdPop_64:
        push    rsi
        mov     rsi,[KbdHead]
        xor     rax,rax
        mov     al,byte [KbdBuffer + rsi]
        inc     rsi
        cmp     rsi,BufferLength
        jnz     .NoLoop
        xor     rsi,rsi
.NoLoop:
        mov     [KbdHead],rsi
        pop     rsi
        ret

;       RAX=1 ЕСЛИ БЫЛА НАЖАТА КЛАВИША
KeyPressed_64:
        push    rbx
        xor     rax,rax
        mov     rbx,qword [KbdHead]
        cmp     rbx,qword [KbdEnd]
        jz      .NoPressed
        inc     rax
.NoPressed:
        pop     rbx
        ret

KbdClear_64:
        mov     qword [KbdHead],0
        mov     qword [KbdEnd],0
        ret


;       буфер клавиатуры
KbdBuffer       db      BufferLength      dup(0)
KbdHead         dq      0
KbdEnd          dq      0



;       обработчик для остальных IRQ
OtherIRQ_64:
        push    rax
        mov     al,20h    ;       EOI
        out     020h,al
        out     0A0h,al
        pop     rax
        iretq

