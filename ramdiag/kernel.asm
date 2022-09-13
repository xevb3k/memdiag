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

CODE_SELECTOR_32        equ     001000b         ; селектор сегмента кода для PM
DATA_SELECTOR_32        equ     010000b         ; селектор сегмента данных для PM

CODE_SELECTOR_64        equ     011000b         ; селектор сегмента кода для LM
DATA_SELECTOR_64        equ     100000b         ; селектор сегмента данных для LM

TSS_SELECTOR_64         equ     0101000b        ; селектор TSS64

PIC1Base                equ     20h             ; базовые адреса для PIC
PIC2Base                equ     28h

struc MEM_STRUC EntryCnt           ; описатель области памяти
{
times EntryCnt          dd      0  ; начальный адрес диапазона, младшие 32 бита
times EntryCnt          dd      0  ; начальный адрес диапазона, старшие 32 бита
times EntryCnt          dd      0  ; длина диапазона, младшие 32 бита
times EntryCnt          dd      0  ; длина диапазона, старшие 32 бита
times EntryCnt          dd      0  ; тип диапазона
times EntryCnt          dd      0  ; атрибуты диапазона
}

include 'macros.inc'

        org     600h

        use16

        jmp     0:REAL_MODE_ENTRY_POINT

        ;       стек 4 КБ (после перехода в PM/LM можно использовать 5.5 КБ)
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

        ;       настройка видеорежима
        ;       80x25x16
        mov     ax,3
        int     10h
        ;       разрешаем 16 цветов
        mov     ax,1003h
        xor     bl,bl
        int     10h
        ;       гасим курсор
        mov     ah,01h
        mov     ch,20h
        int     10h

        ;       гасим диоды клавиатуры
        xor     cl,cl
        call    KbdChangeLED_16

        ;       очищаем экран
        call    ClearScreen_16

        ;       приветственная надпись
        mov     si,WelcomeText
        call    OutText_16

        ;       проверяем поддержку инструкции cpuid
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
        jnz     CPUSupported   ; процессор 486 или выше
        mov     si,CPUNotSupportedText
        call    OutText_16
        jmp     ResetPC

CPUSupported:
        ;       определение поддержки x86-64
        mov     eax,80000000h
        cpuid
        cmp     eax,80000001h       ; если функция 80000001h не поддерживается
        jb      No_LM_Supported     ; то и LM не поддерживается

        ;       заполняем поля о имени процессора
        mov     si,CPUVendorName
        mov     di,CPUName
        call    GetCPUinfo

        mov     eax,80000001h
        cpuid
        bt      edx,29            ; если бит 29 установлен - LM поддерживается
        jc      LM_Supported

No_LM_Supported:
        mov     byte [x64Support],0 ; LM не поддерживается

LM_Supported:
        ;       формирование карты распределения памяти
        call    GetMemoryMap
        jnc     .MemMapDetectOK
        mov     si,ACPINotSupportedText
        call    OutText_16
        jmp     ResetPC

.MemMapDetectOK:
        ; подготовка к переходу в PM

        ; запретить все IRQ
        cli
        in      al,70h
        or      al,80h
        times   5 nop
        out     70h,al

        ; включение A20
        in      al,92h
        or      al,02h
        times   5 nop
        out     92h,al

        ;       инициализация контроллеров прерываний,
        ;       IRQ0 - IRQ7 отображается на PIC1Base - PIC1Base+7,
        ;       a IRQ8 - IRQ15 на PIC2Base - PIC2Base+7
        mov     al,00010001b         ; ICW1
        out     20h,al
        out     0A0h,al
        mov     al,PIC1Base          ; ICW2 для первого контроллера
        out     21h,al
        mov     al,PIC2Base          ; ICW2 для второго контроллера
        out     0A1h,al
        mov     al,04h               ; ICW3 для первого контроллера
        out     21h,al
        mov     al,02h               ; ICW3 для второго контроллера
        out     0A1h,al
        mov     al,00000001b         ; ICW4 для первого контроллера
        out     21h,al
        mov     al,00000001b         ; ICW4 для второго контроллера
        out     0A1h,al

        ;       маскируем все IRQ, кроме клавиатуры и таймера
        ;       1-й контроллер
        in      al,21h
        or      al,11111100b
        out     21h,al
        ;       2-й контроллер
        in      al,0A1h
        or      al,11111111b
        out     0A1h,al

        ;       загрузка GDTR_32 и IDTR_32
        lgdt    fword [GDTR_32]
        lidt    fword [IDTR_32]

        ;       переход PM
        mov     eax,cr0
        or      al,1
        mov     cr0,eax

        ;       переход на точку входа в PM для сброса очереди команд
        ;       и загрузки в CS реального селектора
        db      66h              ; регистр CS еще 16-битный
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
        jc      .Failed     ;   функция не поддерживается
        mov     edx,'PAMS'
        cmp     eax,edx
        jne     .Failed     ;   функция ошибочна
        test    ebx,ebx
        je      .Failed     ;   всего 1 запись - ошибка
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

;       ожидание any key и сброс
ResetPC:
        mov     si,PressAnyKeyText
        call    OutText_16
        ;       ждем нажатия клавиши
        xor     ax,ax
        int     16h
        ;       сброс машины
        call    KbdWait_16
        mov     al,0FEh
        out     64h,al
        cli
        hlt

MemMapEntryCount        dq      0   ; количество записей в MemMap
MemMap                  MEM_STRUC 100

MemSizeType1            dq      0   ; AddressRangeMemory
MemSizeType2            dq      0   ; AddressRangeReserved
MemSizeType3            dq      0   ; AddressRangeACPI
MemSizeType4            dq      0   ; AddressRangeNVS
MemSizeType5            dq      0   ; AddressRangeUnusable
MemSizeAvail            dq      0


;       Информация о CPU
;       SI - указатель на строку для имени вендора
;       DI - указатель на строку для имени процессора
GetCPUinfo:
        pushad
        ;       определение производителя CPU
        xor     eax,eax
        cpuid
        mov     dword [si],ebx
        mov     dword [si+4],edx
        mov     dword [si+8],ecx
        ;       определение имени CPU
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


include                 'lib16.inc'      ; библиотека функций для реального режима

WelcomeText             db      'RAM diagnostic tool. RGATU, 2013',0
CPUNotSupportedText     db      'Error: CPU not supported (required i80486 or better)',0
PressAnyKeyText         db      'Press any key to reset...',0
ACPINotSupportedText    db      'Error: ACPI not supported (required v.3.0 or higher)',0

CPUVendorName           db      12      dup(0),0
CPUName                 db      3*16    dup(0),0
CPUNameOffset           dq      0               ; для INTEL - смещение начала имени CPU в CPUName

txt_TotalRAM            db      'Total RAM:',0
txt_AvailRAM            db      'Available RAM:',0
txt_Mb                  db      'Mb',0
txt_Kb                  db      'Kb',0


txt_CPUMode             db      'CPU mode:',0
txt_64bitMode           db      'long mode (64 bit)',0
txt_PM                  db      'protected (32-bit)',0

x64Support              db      1


;       ТОЧКА ВХОДА В PM

        use32


PROTECTED_MODE_ENRTY_POINT:

        mov     ax,DATA_SELECTOR_32
        mov     ds,ax
        mov     es,ax
        mov     fs,ax
        mov     gs,ax
        mov     ss,ax
        mov     esp,STACK_TOP

        ;       переходим в LM, если он поддерживается
        cmp     [x64Support],1
        je      GoLongMode

        ;       иначе остаемся в PM

        ;       разрешить NMI и прерывания
        in      al,70h
        and     al,7Fh
        out     70h,al

        sti

        ;       переход в основную программу (32 бита)
        jmp     MAIN_32


include         'main32.inc'
include         'idt32.inc'       ; дескрипторы прерыавний для PM
include         'exchdl32.inc'    ; обработчики исключений для PM
include         'irqhdl32.inc'    ; обработчики IRQ для PM
include         'lib32.inc'       ; библиотеки для PM

include         'gdt.inc'

;               регистр GDTR_32
GDTR_32         dw cGDT_Size-1     ; лимит GDT
                dd GDT             ; физический адрес GDT

;               регистр IDTR_32
IDTR_32         dw cIDT_Size_32-1  ; лимит IDT_32
                dd IDT_32          ; физический адрес IDT_32

;               временные адреса таблиц трансляции до перехода в LM
PML4_Addr       equ     1FC000h
PDPE_Addr       equ     1FD000h
PDE_Addr        equ     1FE000h


PDT_Addr        equ     8000h     ; размер 512 КБ  (MaxMemAvail * 4 КБ)
PDPT_Addr       equ     88000h    ; размер 4 КБ
PML4T_Addr      equ     89000h    ; размер 4 КБ

;               максимальный объем отображаемой памяти (ГБ)
MaxMemAvail     equ     128     ; 128 таблиц по 4096 = 512 КБ


;       подготовка к переходу в LM

GoLongMode:
        ;       снимаем PG
        mov     eax,cr0
        and     eax,7FFFFFFFh
        mov     cr0,eax

        ;       PAE := 1
        mov     eax,cr4
        bts     eax,5
        mov     cr4,eax

        ;       подготовка начальной PML4T
        mov     dword [PDE_Addr], 010000011b
        mov     dword [PDE_Addr+4], 0
        mov     dword [PDPE_Addr], PDE_Addr or 11b
        mov     dword [PDPE_Addr+4], 0
        mov     dword [PML4_Addr], PDPE_Addr or 11b
        mov     dword [PML4_Addr+4], 0

        ;       адрес PML4 в CR3
        mov     eax,PML4_Addr
        mov     cr3,eax

        ;       сообщаем процессору о намерении включить LM
        mov     ecx,0C0000080h
        rdmsr
        bts     eax,8   ; IA32_EFER.LME := 1
        wrmsr

        ;       включаем страничную адресацию
        ;       это заставит процессор установить IA32_EFER.LMA в 1,
        ;       что свидетельствует о переходе в LM
        mov     eax,cr0
        bts     eax,31  ; CR0.PG := 1
        mov     cr0,eax

        ;       дальний переход на точку входа в LM
        jmp     CODE_SELECTOR_64 : LONG_MODE_ENTRY_POINT

LONG_MODE_ENTRY_POINT:  ; точка входа в LM


        use64

        mov     rsp,STACK_TOP

        ;       проверям, работает ли процессор в LM
        mov     ecx,0C0000080h
        rdmsr
        bt      eax,10
        jc      LM_Enabled

LM_Enabled:

        ;       загрузка 64-битной IDT
        lidt    tword [IDTR_64]

        ;       загрузка регистра TR
        mov     ax,TSS_SELECTOR_64
        ltr     ax

        call    Page_Setup

        ;       адрес PML4 в CR3
        mov     rax,PML4T_Addr
        mov     cr3,rax

        ;       разрешить NMI
        in      al,70h
        and     al,7Fh
        out     70h,al

        ;       разрешить IRQ
        sti

        call    CalcRAM

        ;       переход в основную программу (64 бита)
        jmp     MAIN_64

include         'main64.inc'
include         'idt64.inc'
include         'exchdl64.inc'
include         'irqhdl64.inc'
include         'lib64.inc'



;       подсчет объема памяти
CalcRAM:
        ;       считаем объем каждого типа памяти
        mov     rcx,[MemMapEntryCount]
        xor     rsi,rsi
.NextBlock:
        mov     r10d,dword [MemMap + rsi + 16]      ; тип памяти
        mov     r11, qword [MemMap + rsi + 8]       ; размер блока
        dec     r10d
        add     qword [MemSizeType1 + r10d * 8],r11 ; суммируем однотипные блоки
        add     rsi,24
        loop    .NextBlock        ; цикл по числу вхождений в карте памяти

        ;       считаем общий объем RAM
        mov     rax,qword [MemSizeType1] ; общий размер доступной памяти
        sub     rax,qword [MemMap + 8]   ; минус размер доступного объема базовой памяти
        add     rax,100000h              ; + 1 МБ
        add     rax,qword [MemSizeType3] ; + занятое под ACPI
        add     rax,qword [MemSizeType4] ; + тип 4
        mov     qword [MemSizeAvail],rax
        ret


;       НАСТРОЙКА ТАБЛИЦ ТРАНСЛЯЦИИ
Page_Setup:
        ;       обнуляем PDT
        mov     rcx,MaxMemAvail * 4096 / 8   ; MaxMemAvail таблиц по 4 КБ
        mov     rdi,PDT_Addr
        xor     rax,rax
        cld
        rep     stosq
        ;       обнуляем PDPT
        mov     rcx,4096 / 8     ; 1 таблица 4 КБ
        mov     rdi,PDPT_Addr
        rep     stosq
        ;       обнуляем PML4T
        mov     rcx,4096 / 8     ; 1 таблица 4 КБ
        mov     rdi,PML4T_Addr
        rep     stosq

        ;       настройка PML4T
        mov     qword [PML4T_Addr],PDPT_Addr or 11b    ; присутствует только 1 элемент в таблице PML4T

        ;       настройка PDPT (MaxMemAvail элементов)
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

        ;       настройка PDT  (MaxMemAvail таблиц по 512 вхождений по 8 байт)
        xor     rdi,rdi              ; физический адрес страницы
        xor     rsi,rsi
        mov     rcx,512 * MaxMemAvail   ; MaxMemAvail таблиц по 512 вхождений
.NextPDE:
        or      rdi,010000011b
        mov     qword [PDT_Addr + rsi], rdi
        add     rsi,8                   ; шаг вхождений 8 байт
        add     rdi,1024*1024*2         ; шаг физ. адресов 2 МБ
        loop    .NextPDE
        ret


;       64-битный IDTR
IDTR_64 dw      cIDT_Size_64-1
        dq      IDT_64

;       структура определения 64-битного TSS
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
