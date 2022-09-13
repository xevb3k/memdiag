;       ������⥪� �ᯮ����⥫��� �㭪権 ��� ���饭���� ०���

        use32

;       ��頥� ��࠭
ClearScreen_32:
        pushad
        mov     edi,VideoBuffer
        mov     eax,07000700h
        mov     ecx,1000
        cld
        rep     stosd
        popad
        ret

;       ����������� �������������
;       eax,ebx   - x1, y1
;       ecx,esi   - LenX, LenY
;       dl        - 梥�
Bar_32:
        pushad
        mov     edi,VideoBuffer
        ;       ���塞 ������� ���� ������ ���孥�� 㣫�
        shl     ebx,1           ; ebx = ebx * 80
        lea     ebx,[ebx+ebx*4]
        shl     ebx,3
        add     ebx,eax
        shl     ebx,1
        ;       ������塞 � ᬥ饭�� ���������
        add     edi,ebx
        shl     dl,4
        mov     ah,dl
.NextLine:
        push    ecx
        mov     al,20h   ; �஡��
        cld
        rep     stosw
        pop     ecx
        ;       ����塞 ���� ᫥���饩 ��ப�
        add     edi,80 * 2
        sub     edi,ecx
        sub     edi,ecx
        ;       ���室 �� ᫥������ ��ப�
        dec     esi
        jnz     .NextLine
        popad
        ret



;       �뢮� ⥪�� � ���ਡ�⠬� �� ��࠭
;       ���ਡ��� ������ ᮤ�ঠ���� � ᠬ�� ⥪��
;       eax     - ��ப�
;       ebx     - �⮫���
;       ds:esi  - �����
;       ecx     - �����
Write_32:
        pushad
        mov     edi,VideoBuffer
        mov     edx,80
        mul     edx
        add     eax,ebx
        shl     eax,1
        add     edi,eax
        cld
        rep     movsw   ; ᪠��� CX ���� �� DS:SI � ES:DI
        popad
        ret

;       �뢮� ⥪�� � ���ਡ�⠬� �� ��࠭
;       ah      - �⮫���
;       al      - ��ப�
;       bh      - ���ਡ��
;       ds:esi  - ⥪��, �����稢��騩�� �㫥�
WriteWAttr_32:
        pushad
        ; ����塞 ���न���� ��ப�
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