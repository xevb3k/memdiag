;       Библиотека вспомогательных функций для защищенного режима

        use32

;       очищает экран
ClearScreen_32:
        pushad
        mov     edi,VideoBuffer
        mov     eax,07000700h
        mov     ecx,1000
        cld
        rep     stosd
        popad
        ret

;       ЗАКРАШЕННЫЙ ПРЯМОУГОЛЬНИК
;       eax,ebx   - x1, y1
;       ecx,esi   - LenX, LenY
;       dl        - цвет
Bar_32:
        pushad
        mov     edi,VideoBuffer
        ;       вчисляем линейный адрес левого верхнего угла
        shl     ebx,1           ; ebx = ebx * 80
        lea     ebx,[ebx+ebx*4]
        shl     ebx,3
        add     ebx,eax
        shl     ebx,1
        ;       добавляем к смещению видеобуфера
        add     edi,ebx
        shl     dl,4
        mov     ah,dl
.NextLine:
        push    ecx
        mov     al,20h   ; пробел
        cld
        rep     stosw
        pop     ecx
        ;       вычисляем адрес следующей строки
        add     edi,80 * 2
        sub     edi,ecx
        sub     edi,ecx
        ;       переход на следующеую строку
        dec     esi
        jnz     .NextLine
        popad
        ret



;       Вывод текста с аттрибутами на экран
;       Аттрибуты должны содержаться в самом тексте
;       eax     - строка
;       ebx     - столбец
;       ds:esi  - данные
;       ecx     - длина
Write_32:
        pushad
        mov     edi,VideoBuffer
        mov     edx,80
        mul     edx
        add     eax,ebx
        shl     eax,1
        add     edi,eax
        cld
        rep     movsw   ; скачать CX байт из DS:SI в ES:DI
        popad
        ret

;       Вывод текста с аттрибутами на экран
;       ah      - столбец
;       al      - строка
;       bh      - аттрибут
;       ds:esi  - текст, заканчивающийся нулем
WriteWAttr_32:
        pushad
        ; вычисляем координаты строки
        mov     dl,80
        mov     dh,ah
        mul     dl
        shr     dx,8
        add     ax,dx
        movzx   eax,ax
        shl     eax,1
        mov     edi,VideoBuffer
        add     edi,eax
.NextChar:
        mov     al,[esi]
        test    al,al
        jz      .Exit
        mov     ah,bh
        mov     [edi],ax
        add     edi,2
        inc     esi
        jmp     .NextChar
.Exit:
        popad
        ret



WriteLn_32:
        ret