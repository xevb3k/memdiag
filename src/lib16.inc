;       Библиотека вспомогательных функций для реального режима

        use16


KbdWait_16:
        push    ax
.Wait:
        in      al,64h
        test    al,0010b
        jnz     .Wait
        pop     ax
        ret

KbdChangeLED_16:
        push    ax
        call    KbdWait_16
        mov     al,0EDh
        out     60h,al
        call    KbdWait_16
        mov     al,cl
        out     60h,al
        pop     ax
        ret

;       ОЧИЩАЕТ ЭКРАН
ClearScreen_16:
        pusha
        push    es

        mov     ax,0B800h
        mov     es,ax
        mov     ax,0700h
        xor     di,di
        mov     cx,2000
        cld
        rep     stosw

        xor     bx,bx
        xor     dx,dx
        mov     ah,02h
        int     10h

        pop     es
        popa
        ret

;       Вывод символа в позиции курсора средствами BIOS
;       После вывода курсор смещается на 1 символ
;       AL - ASCII код символа
OutSymbol_16:
        pusha
        mov     ah,0Eh
        xor     bh,bh
        int     10h
        popa
        ret

Out1013_16:
        push    ax
        mov     al,10
        call    OutSymbol_16
        add     al,3
        call    OutSymbol_16
        pop     ax
        ret

;       Вывод строки текста средствами BIOS
;       Курсор переводится на строку вниз
;       DS:SI - указатель на текст, заканчивающийся 0
OutText_16:
        push    ax si
.NextChar:
        mov     al,[si]
        cmp     al,0
        je      .TextEnd
        call    OutSymbol_16
        inc     si
        jmp     .NextChar
.TextEnd:
        mov     al,10
        call    OutSymbol_16
        add     al,3
        call    OutSymbol_16
        pop     si ax
        ret

;       Вывод десятичного 32-битного числа на экран
;       EAX - число
OutNumberDEC_16:
        push    eax ebx ecx edx
        mov     ebx,10
        xor     cx,cx
.NextDig:
        xor     edx,edx
        div     ebx
        add     dl,30h
        push    dx
        inc     cx
        test    eax,eax
        jnz     .NextDig
.NextOut:
        pop     dx
        mov     al,dl
        call    OutSymbol_16
        loop    .NextOut
        pop     edx ecx ebx eax
        ret

;       Вывод шестнадцатеричного числа на экран
;       EAX - число
OutNumberHEX_16:
        push    cx eax edx
        mov     cx,8
        mov     edx,eax
.NextChar:
        rol     edx,4
        mov     eax,edx
        and     al,00001111b
        ; компактный алгоритм преобразования шестнадцатеричной цифры в код соотв. символа
        cmp     al,10  ; сформируем правильный флаг CF
        sbb     al,69h ; числа 0-9 превращаются в 96h-9Fh, а числа 0Ah-0Fh в 0A1h-0A6h
        das            ; вычитает 66h из первой группы чисел, переводя их в 30h-39h (ASCII- коды символов '0'-'9'),
                       ; и 60h из второй группы чисел, переводя их в 41h-46h (ASCII-коды символов 'A'-'F')
        call    OutSymbol_16
        loop    .NextChar
        pop     edx eax cx
        ret