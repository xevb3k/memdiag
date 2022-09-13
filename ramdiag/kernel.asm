VideoBuffer     equ     0B8000h

clNone          =       0Fh
clBlack         =       00h
clDarkBlue      =       01h
clGreen         =       02h
clBlue2         =       03h
clRed           =       04h
clPurple        =       05h
clBrown         =       06h
clGray          =       07h
clDarkGray      =       08h
clLightBlue     =       09h
clLightGreen    =       0Ah
clLightBlue2    =       0Bh
clLightRed      =       0Ch
clPink          =       0Dh
clYellow        =       0Eh
clWhite         =       0Fh

CODE_SELECTOR_32        equ     001000b         ; ᥫ���� ᥣ���� ���� ��� PM
DATA_SELECTOR_32        equ     010000b         ; ᥫ���� ᥣ���� ������ ��� PM

CODE_SELECTOR_64        equ     011000b         ; ᥫ���� ᥣ���� ���� ��� LM
DATA_SELECTOR_64        equ     100000b         ; ᥫ���� ᥣ���� ������ ��� LM

TSS_SELECTOR_64         equ     0101000b        ; ᥫ���� TSS64

PIC1Base                equ     20h             ; ������ ���� ��� PIC
PIC2Base                equ     28h

struc MEM_STRUC EntryCnt           ; ����⥫� ������ �����
{
times EntryCnt          dd      0  ; ��砫�� ���� ���������, ����訥 32 ���
times EntryCnt          dd      0  ; ��砫�� ���� ���������, ���訥 32 ���
times EntryCnt          dd      0  ; ����� ���������, ����訥 32 ���
times EntryCnt          dd      0  ; ����� ���������, ���訥 32 ���
times EntryCnt          dd      0  ; ⨯ ���������
times EntryCnt          dd      0  ; ��ਡ��� ���������
}

include 'macros.inc'

        org     600h

        use16

        jmp     0:REAL_MODE_ENTRY_POINT

        ;       �⥪ 4 �� (��᫥ ���室� � PM/LM ����� �ᯮ�짮���� 5.5 ��)
        times   4096-5  db     0

STACK_TOP:

REAL_MODE_ENTRY_POINT:

        cli
        xor     ax,ax
        mov     ds,ax
        mov     es,ax
        mov     ss,ax
        mov     sp,STACK_TOP
        sti

        ;       ����ன�� �����०���
        ;       80x25x16
        mov     ax,3
        int     10h
        ;       ࠧ�蠥� 16 梥⮢
        mov     ax,1003h
        xor     bl,bl
        int     10h
        ;       ��ᨬ �����
        mov     ah,01h
        mov     ch,20h
        int     10h

        ;       ��ᨬ ����� ����������
        xor     cl,cl
        call    KbdChangeLED_16

        ;       ��頥� �࠭
        call    ClearScreen_16

        ;       �ਢ���⢥���� �������
        mov     si,WelcomeText
        call    OutText_16

        ;       �஢��塞 �����প� ������樨 cpuid
        pushfd
        pop     eax
        mov     ebx,eax
        xor     eax,200000h
        push    eax
        popfd
        pushfd
        pop     eax
        push    ebx
        popfd
        xor     eax,ebx
        jnz     CPUSupported   ; ������ 486 ��� ���
        mov     si,CPUNotSupportedText
        call    OutText_16
        jmp     ResetPC

CPUSupported:
        ;       ��।������ �����প� x86-64
        mov     eax,80000000h
        cpuid
        cmp     eax,80000001h       ; �᫨ �㭪�� 80000001h �� �����ন������
        jb      No_LM_Supported     ; � � LM �� �����ন������

        ;       ������塞 ���� � ����� ������
        mov     si,CPUVendorName
        mov     di,CPUName
        call    GetCPUinfo

        mov     eax,80000001h
        cpuid
        bt      edx,29            ; �᫨ ��� 29 ��⠭����� - LM �����ন������
        jc      LM_Supported

No_LM_Supported:
        mov     byte [x64Support],0 ; LM �� �����ন������

LM_Supported:
        ;       �ନ஢���� ����� ��।������ �����
        call    GetMemoryMap
        jnc     .MemMapDetectOK
        mov     si,ACPINotSupportedText
        call    OutText_16
        jmp     ResetPC

.MemMapDetectOK:
        ; �����⮢�� � ���室� � PM

        ; ������� �� IRQ
        cli
        in      al,70h
        or      al,80h
        times   5 nop
        out     70h,al

        ; ����祭�� A20
        in      al,92h
        or      al,02h
        times   5 nop
        out     92h,al

        ;       ���樠������ ����஫��஢ ���뢠���,
        ;       IRQ0 - IRQ7 �⮡ࠦ����� �� PIC1Base - PIC1Base+7,
        ;       a IRQ8 - IRQ15 �� PIC2Base - PIC2Base+7
        mov     al,00010001b         ; ICW1
        out     20h,al
        out     0A0h,al
        mov     al,PIC1Base          ; ICW2 ��� ��ࢮ�� ����஫���
        out     21h,al
        mov     al,PIC2Base          ; ICW2 ��� ��ண� ����஫���
        out     0A1h,al
        mov     al,04h               ; ICW3 ��� ��ࢮ�� ����஫���
        out     21h,al
        mov     al,02h               ; ICW3 ��� ��ண� ����஫���
        out     0A1h,al
        mov     al,00000001b         ; ICW4 ��� ��ࢮ�� ����஫���
        out     21h,al
        mov     al,00000001b         ; ICW4 ��� ��ண� ����஫���
        out     0A1h,al

        ;       ��᪨�㥬 �� IRQ, �஬� ���������� � ⠩���
        ;       1-� ����஫���
        in      al,21h
        or      al,11111100b
        out     21h,al
        ;       2-� ����஫���
        in      al,0A1h
        or      al,11111111b
        out     0A1h,al

        ;       ����㧪� GDTR_32 � IDTR_32
        lgdt    fword [GDTR_32]
        lidt    fword [IDTR_32]

        ;       ���室 PM
        mov     eax,cr0
        or      al,1
        mov     cr0,eax

        ;       ���室 �� ��� �室� � PM ��� ��� ��।� ������
        ;       � ����㧪� � CS ॠ�쭮�� ᥫ����
        db      66h              ; ॣ���� CS �� 16-����
        db      0EAh
        dd      PROTECTED_MODE_ENRTY_POINT
        dw      CODE_SELECTOR_32


GetMemoryMap:
        pushad
        xor     ebx,ebx
        xor     bp,bp
        mov     di,MemMap

        mov     eax,0E820h
        mov     ecx,24
        mov     edx,'PAMS'
        mov     dword [di+20],1
        int     15h
        jc      .Failed     ;   �㭪�� �� �����ন������
        mov     edx,'PAMS'
        cmp     eax,edx
        jne     .Failed     ;   �㭪�� �訡�筠
        test    ebx,ebx
        je      .Failed     ;   �ᥣ� 1 ������ - �訡��
        jmp     .JmpIn
.NextEntry:
        mov     eax,0E820h
        mov     ecx,24
        mov     dword [di+20],1
        int     15h
        jc      .End
        mov     edx,'PAMS'
.JmpIn:
        jcxz    .SkipEntry
        cmp     cl,20
        jbe     .NoText
        test    byte [di+20],1
        je      .SkipEntry
.NoText:
        mov     ecx,[di+8]
        or      ecx,[di+12]
        jz      .SkipEntry
        inc     bp
        add     di,24
.SkipEntry:
        test    ebx,ebx
        jne     .NextEntry
.End:
        mov     word [MemMapEntryCount],bp
        clc
        popad
        ret
.Failed:
        stc
        popad
        ret

;       �������� any key � ���
ResetPC:
        mov     si,PressAnyKeyText
        call    OutText_16
        ;       ���� ������ ������
        xor     ax,ax
        int     16h
        ;       ��� ��設�
        call    KbdWait_16
        mov     al,0FEh
        out     64h,al
        cli
        hlt

MemMapEntryCount        dq      0   ; ������⢮ ����ᥩ � MemMap
MemMap                  MEM_STRUC 100

MemSizeType1            dq      0   ; AddressRangeMemory
MemSizeType2            dq      0   ; AddressRangeReserved
MemSizeType3            dq      0   ; AddressRangeACPI
MemSizeType4            dq      0   ; AddressRangeNVS
MemSizeType5            dq      0   ; AddressRangeUnusable
MemSizeAvail            dq      0


;       ���ଠ�� � CPU
;       SI - 㪠��⥫� �� ��ப� ��� ����� ������
;       DI - 㪠��⥫� �� ��ப� ��� ����� ������
GetCPUinfo:
        pushad
        ;       ��।������ �ந�����⥫� CPU
        xor     eax,eax
        cpuid
        mov     dword [si],ebx
        mov     dword [si+4],edx
        mov     dword [si+8],ecx
        ;       ��।������ ����� CPU
        mov     eax,80000002h
        cpuid
        mov     dword [di],eax
        mov     dword [di+4],ebx
        mov     dword [di+8],ecx
        mov     dword [di+12],edx
        mov     eax,80000003h
        cpuid
        mov     dword [di+16],eax
        mov     dword [di+20],ebx
        mov     dword [di+24],ecx
        mov     dword [di+28],edx
        mov     eax,80000004h
        cpuid
        mov     dword [di+32],eax
        mov     dword [di+36],ebx
        mov     dword [di+40],ecx
        mov     dword [di+44],edx

        cld
        mov     eax,32
        mov     ecx,3*16
        repz    scasb
        dec     di
        mov     word [CPUNameOffset],di
        popad
        ret


include                 'lib16.inc'      ; ������⥪� �㭪権 ��� ॠ�쭮�� ०���

WelcomeText             db      'RAM diagnostic tool. RGATU, 2013',0
CPUNotSupportedText     db      'Error: CPU not supported (required i80486 or better)',0
PressAnyKeyText         db      'Press any key to reset...',0
ACPINotSupportedText    db      'Error: ACPI not supported (required v.3.0 or higher)',0

CPUVendorName           db      12      dup(0),0
CPUName                 db      3*16    dup(0),0
CPUNameOffset           dq      0               ; ��� INTEL - ᬥ饭�� ��砫� ����� CPU � CPUName

txt_TotalRAM            db      'Total RAM:',0
txt_AvailRAM            db      'Available RAM:',0
txt_Mb                  db      'Mb',0
txt_Kb                  db      'Kb',0


txt_CPUMode             db      'CPU mode:',0
txt_64bitMode           db      'long mode (64 bit)',0
txt_PM                  db      'protected (32-bit)',0

x64Support              db      1


;       ����� ����� � PM

        use32


PROTECTED_MODE_ENRTY_POINT:

        mov     ax,DATA_SELECTOR_32
        mov     ds,ax
        mov     es,ax
        mov     fs,ax
        mov     gs,ax
        mov     ss,ax
        mov     esp,STACK_TOP

        ;       ���室�� � LM, �᫨ �� �����ন������
        cmp     [x64Support],1
        je      GoLongMode

        ;       ���� ��⠥��� � PM

        ;       ࠧ���� NMI � ���뢠���
        in      al,70h
        and     al,7Fh
        out     70h,al

        sti

        ;       ���室 � �᭮���� �ணࠬ�� (32 ���)
        jmp     MAIN_32


include         'main32.inc'
include         'idt32.inc'       ; ���ਯ��� ���렢��� ��� PM
include         'exchdl32.inc'    ; ��ࠡ��稪� �᪫�祭�� ��� PM
include         'irqhdl32.inc'    ; ��ࠡ��稪� IRQ ��� PM
include         'lib32.inc'       ; ������⥪� ��� PM

include         'gdt.inc'

;               ॣ���� GDTR_32
GDTR_32         dw cGDT_Size-1     ; ����� GDT
                dd GDT             ; 䨧��᪨� ���� GDT

;               ॣ���� IDTR_32
IDTR_32         dw cIDT_Size_32-1  ; ����� IDT_32
                dd IDT_32          ; 䨧��᪨� ���� IDT_32

;               �६���� ���� ⠡��� �࠭��樨 �� ���室� � LM
PML4_Addr       equ     1FC000h
PDPE_Addr       equ     1FD000h
PDE_Addr        equ     1FE000h


PDT_Addr        equ     8000h     ; ࠧ��� 512 ��  (MaxMemAvail * 4 ��)
PDPT_Addr       equ     88000h    ; ࠧ��� 4 ��
PML4T_Addr      equ     89000h    ; ࠧ��� 4 ��

;               ���ᨬ���� ��ꥬ �⮡ࠦ����� ����� (��)
MaxMemAvail     equ     128     ; 128 ⠡��� �� 4096 = 512 ��


;       �����⮢�� � ���室� � LM

GoLongMode:
        ;       ᭨���� PG
        mov     eax,cr0
        and     eax,7FFFFFFFh
        mov     cr0,eax

        ;       PAE := 1
        mov     eax,cr4
        bts     eax,5
        mov     cr4,eax

        ;       �����⮢�� ��砫쭮� PML4T
        mov     dword [PDE_Addr], 010000011b
        mov     dword [PDE_Addr+4], 0
        mov     dword [PDPE_Addr], PDE_Addr or 11b
        mov     dword [PDPE_Addr+4], 0
        mov     dword [PML4_Addr], PDPE_Addr or 11b
        mov     dword [PML4_Addr+4], 0

        ;       ���� PML4 � CR3
        mov     eax,PML4_Addr
        mov     cr3,eax

        ;       ᮮ�頥� ������� � ����७�� ������� LM
        mov     ecx,0C0000080h
        rdmsr
        bts     eax,8   ; IA32_EFER.LME := 1
        wrmsr

        ;       ����砥� ��࠭���� ������
        ;       �� ���⠢�� ������ ��⠭����� IA32_EFER.LMA � 1,
        ;       �� ᢨ��⥫����� � ���室� � LM
        mov     eax,cr0
        bts     eax,31  ; CR0.PG := 1
        mov     cr0,eax

        ;       ���쭨� ���室 �� ��� �室� � LM
        jmp     CODE_SELECTOR_64 : LONG_MODE_ENTRY_POINT

LONG_MODE_ENTRY_POINT:  ; �窠 �室� � LM


        use64

        mov     rsp,STACK_TOP

        ;       �஢���, ࠡ�⠥� �� ������ � LM
        mov     ecx,0C0000080h
        rdmsr
        bt      eax,10
        jc      LM_Enabled

LM_Enabled:

        ;       ����㧪� 64-��⭮� IDT
        lidt    tword [IDTR_64]

        ;       ����㧪� ॣ���� TR
        mov     ax,TSS_SELECTOR_64
        ltr     ax

        call    Page_Setup

        ;       ���� PML4 � CR3
        mov     rax,PML4T_Addr
        mov     cr3,rax

        ;       ࠧ���� NMI
        in      al,70h
        and     al,7Fh
        out     70h,al

        ;       ࠧ���� IRQ
        sti

        call    CalcRAM

        ;       ���室 � �᭮���� �ணࠬ�� (64 ���)
        jmp     MAIN_64

include         'main64.inc'
include         'idt64.inc'
include         'exchdl64.inc'
include         'irqhdl64.inc'
include         'lib64.inc'



;       ������ ��ꥬ� �����
CalcRAM:
        ;       ��⠥� ��ꥬ ������� ⨯� �����
        mov     rcx,[MemMapEntryCount]
        xor     rsi,rsi
.NextBlock:
        mov     r10d,dword [MemMap + rsi + 16]      ; ⨯ �����
        mov     r11, qword [MemMap + rsi + 8]       ; ࠧ��� �����
        dec     r10d
        add     qword [MemSizeType1 + r10d * 8],r11 ; �㬬��㥬 ����⨯�� �����
        add     rsi,24
        loop    .NextBlock        ; 横� �� ��� �宦����� � ���� �����

        ;       ��⠥� ��騩 ��ꥬ RAM
        mov     rax,qword [MemSizeType1] ; ��騩 ࠧ��� ����㯭�� �����
        sub     rax,qword [MemMap + 8]   ; ����� ࠧ��� ����㯭��� ��ꥬ� ������� �����
        add     rax,100000h              ; + 1 ��
        add     rax,qword [MemSizeType3] ; + ����⮥ ��� ACPI
        add     rax,qword [MemSizeType4] ; + ⨯ 4
        mov     qword [MemSizeAvail],rax
        ret


;       ��������� ������ ����������
Page_Setup:
        ;       ����塞 PDT
        mov     rcx,MaxMemAvail * 4096 / 8   ; MaxMemAvail ⠡��� �� 4 ��
        mov     rdi,PDT_Addr
        xor     rax,rax
        cld
        rep     stosq
        ;       ����塞 PDPT
        mov     rcx,4096 / 8     ; 1 ⠡��� 4 ��
        mov     rdi,PDPT_Addr
        rep     stosq
        ;       ����塞 PML4T
        mov     rcx,4096 / 8     ; 1 ⠡��� 4 ��
        mov     rdi,PML4T_Addr
        rep     stosq

        ;       ����ன�� PML4T
        mov     qword [PML4T_Addr],PDPT_Addr or 11b    ; ��������� ⮫쪮 1 ����� � ⠡��� PML4T

        ;       ����ன�� PDPT (MaxMemAvail ����⮢)
        xor     rdi,rdi
        xor     rsi,rsi
        mov     rcx,MaxMemAvail
.NextPDPE:
        mov     rax,rsi
        shl     rax,12          ; RAX := RSI * 4096
        add     rax,PDT_Addr    ; RAX := RAX + PDT_Addr
        or      rax,11b         ; RAX := RAX or 11b
        mov     qword [PDPT_Addr + rdi],rax
        add     rdi,8
        inc     rsi
        loop    .NextPDPE

        ;       ����ன�� PDT  (MaxMemAvail ⠡��� �� 512 �宦����� �� 8 ����)
        xor     rdi,rdi              ; 䨧��᪨� ���� ��࠭���
        xor     rsi,rsi
        mov     rcx,512 * MaxMemAvail   ; MaxMemAvail ⠡��� �� 512 �宦�����
.NextPDE:
        or      rdi,010000011b
        mov     qword [PDT_Addr + rsi], rdi
        add     rsi,8                   ; 蠣 �宦����� 8 ����
        add     rdi,1024*1024*2         ; 蠣 䨧. ���ᮢ 2 ��
        loop    .NextPDE
        ret


;       64-���� IDTR
IDTR_64 dw      cIDT_Size_64-1
        dq      IDT_64

;       ������� ��।������ 64-��⭮�� TSS
struc   DEFINE_TSS_64
{
        .TSSBase        dd      0
        .RSP0           dq      0
        .RSP1           dq      0
        .RSP2           dq      0
                        dq      0
        .IST1           dq      0
        .IST2           dq      0
        .IST3           dq      0
        .IST4           dq      0
        .IST5           dq      0
        .IST6           dq      0
        .IST7           dq      0
                        dq      0
                        dw      0
        .IOMapBase      dw      $-.TSSBase
}

TSS_64           DEFINE_TSS_64
cTSS_Size_64     =     $-TSS_64
