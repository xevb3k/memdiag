;       ������⥪� �ᯮ����⥫��� �㭪権 ��� ॠ�쭮�� ०���

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

;       ������� �����
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

;       �뢮� ᨬ���� � ����樨 ����� �।�⢠�� BIOS
;       ��᫥ �뢮�� ����� ᬥ頥��� �� 1 ᨬ���
;       AL - ASCII ��� ᨬ����
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

;       �뢮� ��ப� ⥪�� �।�⢠�� BIOS
;       ����� ��ॢ������ �� ��ப� ����
;       DS:SI - 㪠��⥫� �� ⥪��, �����稢��騩�� 0
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

;       �뢮� �����筮�� 32-��⭮�� �᫠ �� ��࠭
;       EAX - �᫮
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

;       �뢮� ��⭠����筮�� �᫠ �� ��࠭
;       EAX - �᫮
OutNumberHEX_16:
        push    cx eax edx
        mov     cx,8
        mov     edx,eax
.NextChar:
        rol     edx,4
        mov     eax,edx
        and     al,00001111b
        ; �������� ������ �८�ࠧ������ ��⭠����筮� ���� � ��� ᮮ�. ᨬ����
        cmp     al,10  ; ��ନ�㥬 �ࠢ���� 䫠� CF
        sbb     al,69h ; �᫠ 0-9 �ॢ������ � 96h-9Fh, � �᫠ 0Ah-0Fh � 0A1h-0A6h
        das            ; ���⠥� 66h �� ��ࢮ� ��㯯� �ᥫ, ��ॢ��� �� � 30h-39h (ASCII- ���� ᨬ����� '0'-'9'),
                       ; � 60h �� ��ன ��㯯� �ᥫ, ��ॢ��� �� � 41h-46h (ASCII-���� ᨬ����� 'A'-'F')
        call    OutSymbol_16
        loop    .NextChar
        pop     edx eax cx
        ret