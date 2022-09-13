;       Обработчики исключений


Int00Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext00_Name
        call    WriteWAttr_32
        popad
        iretd

Int01Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext01_Name
        call    WriteWAttr_32
        popad
        iretd


Int02Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext02_Name
        call    WriteWAttr_32
        popad
        iretd

Int03Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext03_Name
        call    WriteWAttr_32
        popad
        iretd


Int04Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext04_Name
        call    WriteWAttr_32
        popad
        iretd

Int05Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext05_Name
        call    WriteWAttr_32
        popad
        iretd

Int06Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext06_Name
        call    WriteWAttr_32
        popad
        iretd

Int07Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext07_Name
        call    WriteWAttr_32
        popad
        iretd

Int08Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext08_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int0AHandle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext0A_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int0BHandle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext0B_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int0CHandle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext0C_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int0DHandle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext0D_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int0EHandle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext0E_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int10Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext10_Name
        call    WriteWAttr_32
        popad
        iretd

Int11Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext11_Name
        call    WriteWAttr_32
        pop     eax
        popad
        iretd

Int12Handle_32:
        pushad
        mov     ah,0
        mov     al,24
        movcl   bh,0,10
        mov     esi,Ext12_Name
        call    WriteWAttr_32
        popad
        iretd
