;       Библиотека вспомогательных функций для LM

        use64

cFwd            =       0
cBack           =       1



;       ЗАЛИВКА ЭКРАНА С ЗАДАННЫМИ АТТРИБУТАМИ
;       вход:   FonColor, SymColor
;       выход:  нет
ClearScreen_64:
        push    rbp
        mov     rbp,rsp
        push    rax rcx rdi

        mov     rax,[rbp+16]
        mov     rcx,[rbp+24]

        shl     rax,12
        shl     rcx,8
        or      rax,rcx
        shl     rax,48
        shr     rax,48
        mov     rcx,rax

        shl     rax,16
        or      rax,rcx
        shl     rax,16
        or      rax,rcx
        shl     rax,16
        or      rax,rcx

        mov     rdi,VideoBuffer

        mov     rcx,500
        cld
        rep     stosq

        pop     rdi rcx rax
        pop     rbp
        ret     16

;       ЗАКРАШЕННЫЙ ПРЯМОУГОЛЬНИК
;       вход:   X, Y, LenX, LenY, FonColor, SymColor
;       выход:  нет
Bar_64:
        push    rbp
        mov     rbp,rsp
        push    rax rcx rdi r8 r9 r10 r11
        ;       извлечение параметров
        mov     r8, [rbp+16]
        mov     r9, [rbp+24]
        mov     r10,[rbp+32]
        mov     r11,[rbp+40]
        mov     rax,[rbp+48]
        mov     rcx,[rbp+56]

        shl     rax,12
        shl     rcx,8
        or      rax,rcx

        mov     rdi,VideoBuffer
        ;       вчисляем линейный адрес левого верхнего угла
        shl     r9,1           ; y = y * 80
        lea     r9,[r9+r9*4]
        shl     r9,3
        add     r9,r8          ; y = y + x
        shl     r9,1           ; y = y * 2
        ;       добавляем к смещению видеобуфера
        add     rdi,r9
        ;       цвет должен быть в старших 4х битах ah
        cld
.NextLine:
        mov     rcx,r10
        mov     al,20h   ; пробел
        rep     stosw
        ;       вычисляем адрес следующей строки
        add     rdi,80 * 2
        sub     rdi,r10
        sub     rdi,r10
        ;       переход на следующеую строку
        dec     r11
        jnz     .NextLine
        pop     r11 r10 r9 r8 rdi rcx rax
        pop     rbp
        ret     48



;       ВВЫВОД ТЕКСТА С АТТРИБУТАМИ НА ЭКРАН
;       вход:   x, y, ^Text, Way
;       выход:  нет
;       аттрибуты должны содержаться в самом тексте
;       строка должна заканчиваться нулем
WriteDirectCopy_un_64:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdx rdi rsi r8

        mov     rbx,[rbp+16]
        mov     rax,[rbp+24]
        mov     rsi,[rbp+32]
        mov     r8, [rbp+40]

        stdcall GetStringLength,rsi
        shr     rcx,1

        mov     rdx,80
        mul     rdx      ; RDX:RAX := RDX * RAX
        add     rax,rbx

        test    r8,r8
        jz      .Forward
        sub     rax,rcx
        inc     rax
.Forward:

        shl     rax,1

        mov     rdi,VideoBuffer
        add     rdi,rax

.NextChar:
        mov     ax,[rsi]
        test    al,al
        jz      .Exit

        mov     [rdi],ax

        add     rdi,2
        add     rsi,2
        jmp     .NextChar
.Exit:
        pop     r8 rsi rdi rdx rcx rbx rax
        pop     rbp
        ret     32


;       ВЫВОД ТЕКСТА С АТТРИБУТАМИ НА ЭКРАН
;       вход:   ^Text, x, y, FonColor, SymColor, UseAttr, Way
;       выход:  нет
Write_64:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdx rdi rsi r8 r9 r10

        mov     rsi,[rbp+16] ; rsi
        mov     rbx,[rbp+24] ; rbx
        mov     rax,[rbp+32] ; rax
        mov     rcx,[rbp+40]
        mov     r8, [rbp+48]
        mov     r10,[rbp+56]
        mov     r9, [rbp+64]

        push    rcx
        stdcall GetStringLength,rsi
        ; вычисляем координаты строки
        mov     rdx,80
        mul     rdx      ; RDX:RAX := RDX * RAX
        add     rax,rbx

        test    r9,r9
        jz      .Forward
        sub     rax,rcx
        inc     rax
.Forward:
        pop     rcx

        shl     rax,1

        shl     rcx,12
        shl     r8,8
        or      rcx,r8

        mov     rdi,VideoBuffer
        add     rdi,rax
.NextChar:
        mov     al,[rsi]
        test    al,al
        jz      .Exit

        mov     cl,ch
        mov     ah,[rdi+1]
        bt      r10,0
        jnc     .m2
        and     ah,11110000b
        and     cl,00001111b
        or      ah,cl
.m2:
        mov     cl,ch
        bt      r10,1
        jnc     .m1
        and     ah,00001111b
        and     cl,11110000b
        or      ah,cl
.m1:
        mov     [rdi],ax
        add     rdi,2
        inc     rsi
        jmp     .NextChar
.Exit:
        pop     r10 r9 r8 rsi rdi rdx rcx rbx rax
        pop     rbp
        ret     56

;       ВЫВОД ДЕСЯТИЧНОГО ЧИСЛА НА ЭКРАН
;       вход:   Number, x, y, FonColor, SymColor, UseAttr, Way
;       выход:  нет
WriteDEC_64:
        push    rbp
        mov     rbp,rsp

        push    rax rbx rcx rdx rdi r8 r9 r10 r11 r15

        mov     rax,[rbp+16]
        mov     r9, [rbp+24]
        mov     r8, [rbp+32]
        mov     rbx,[rbp+40]
        mov     r10,[rbp+48]
        mov     r11,[rbp+56]
        mov     r15,[rbp+64]

        shl     rbx,12
        shl     r10,8
        or      rbx,r10   ; bh - аттр

        mov     rdi,VideoBuffer

        shl     r8,1           ; r8 = r8 * 80
        lea     r8,[r8+r8*4]
        shl     r8,3

        add     r8,r9
        shl     r8,1
        add     rdi,r8

        mov     r8,10
        xor     rcx,rcx
.NextDig:
        xor     rdx,rdx
        div     r8
        add     dl,30h
        push    rdx
        inc     rcx
        test    rax,rax
        jnz     .NextDig

        test    r15,r15
        jz      .Forward
        sub     rdi,rcx
        sub     rdi,rcx
        add     rdi,2
.Forward:

        mov     bl,cl
        mov     ch,bh
.NextOut:
        pop     rdx
        mov     al,dl

        mov     cl,ch
        mov     ah,[rdi+1]
        bt      r11,0
        jnc     .m2
        and     ah,11110000b
        and     cl,00001111b
        or      ah,cl
.m2:
        mov     cl,ch
        bt      r11,1
        jnc     .m1
        and     ah,00001111b
        and     cl,11110000b
        or      ah,cl
.m1:

        mov     [rdi],ax
        add     rdi,2
        dec     bl
        jnz     .NextOut

        pop     r15 r11 r10 r9 r8 rdi rdx rcx rbx rax
        pop     rbp
        ret     56


;       ВЫВОД ШЕСТНАДЦАТЕРИЧНОГО ЧИСЛА НА ЭКРАН
;       вход:   Number, x, y, FonColor, SymColor, UseAttr, Length, Way
;       выход:  нет
WriteHEX_64:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdx rdi r8 r9 r10 r11 r12 r13

        mov     r10,[rbp+16]
        mov     rbx,[rbp+24]
        mov     rax,[rbp+32]
        mov     rcx,[rbp+40]
        mov     r11,[rbp+48]
        mov     r13,[rbp+56]  ; useattr
        mov     r8, [rbp+64]
        mov     r12,[rbp+72]

        shl     rax,1           ; rax = rax * 80
        lea     rax,[rax+rax*4]
        shl     rax,3

        add     rax,rbx

        test    r12,r12
        jz      .Forward
        sub     rax,r8
        inc     rax
.Forward:

        shl     rax,1

        mov     rdi,VideoBuffer
        add     rdi,rax

        push    rcx
        mov     r9,16
        sub     r9,r8
        shl     r9,2
        mov     cl,r9l
        shl     r10,cl
        pop     rcx

        shl     rcx,12
        shl     r11,8
        or      rcx,r11  ; ch - attr

        mov     rdx,r10
        mov     rbx,HexTable
.NextChar:
        rol     rdx,4
        mov     rax,rdx
        and     al,00001111b

        xlatb

        mov     cl,ch
        mov     ah,[rdi+1]
        bt      r13,0
        jnc     .m2
        and     ah,11110000b
        and     cl,00001111b
        or      ah,cl
.m2:
        mov     cl,ch
        bt      r13,1
        jnc     .m1
        and     ah,00001111b
        and     cl,11110000b
        or      ah,cl
.m1:

        mov     [rdi],ax
        add     rdi,2
        dec     r8
        jnz     .NextChar

        pop     r13 r12 r11 r10 r9 r8 rdi rdx rcx rbx rax
        pop     rbp
        ret     64


HexTable        db '0123456789ABCDEF'


;       ВОЗВРАЩАЕТ ДЛИНУ СТРОКИ
;       вход:   ^Text
;       выход:  RCX - длина строки
GetStringLength:
        push    rbp
        mov     rbp,rsp
        push    rsi
        mov     rsi,[rbp+16]

        xor     rcx,rcx
.NextSym:
        cmp     byte [rsi],0
        jz      .Stop
        inc     rcx
        inc     rsi
        jmp     .NextSym
.Stop:

        pop     rsi
        pop     rbp
        ret     8



KbdWait_64:
        push    rax
.Wait:
        in      al,64h
        test    al,10b
        jnz     .Wait
        pop     rax
        ret



ScreenSave:
        push    rcx rsi rdi
        mov     rcx,4000 / 8
        mov     rsi,VideoBuffer
        mov     rdi,ScreenBuffer
        cld
        rep     movsq
        pop     rdi rsi rcx
        ret

ScreenRestore:
        push    rcx rsi rdi
        mov     rcx,4000 / 8
        mov     rsi,ScreenBuffer
        mov     rdi,VideoBuffer
        cld
        rep     movsq
        pop     rdi rsi rcx
        ret


ScreenBuffer    db      4000    dup(0)