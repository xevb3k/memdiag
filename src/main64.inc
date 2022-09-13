cNoAttr                 =       00b
cFon                    =       10b
cSym                    =       01b

iTestTableY             =       3

iTestNumX               =       1
iTestNameX              =       22
iTestStatusX            =       35
iTestPassCurrX          =       45
;iTestPassMaxX           =       54
;iTestPassPercentX       =       61
;iTestPercentX           =       68
iTestPatternX           =       68
iTestErrCountX          =       78

iMapWindowY             =       18


MaxTestCount            equ     13

MAIN_64:
        stdcall ClearScreen_64, clBlack, clGray
        ;       области экрана
        stdcall Bar_64, 0, 0,  80, 3,  clLightBlue,  clBlack
        stdcall Bar_64, 0, 3,  80, 1,  clGreen,      clBlack
        stdcall Bar_64, 0, 4,  80, 20, clDarkGray,   clBlack
        stdcall Bar_64, 0, 24, 80, 1,  clLightBlue,  clBlack

        ;       отладочная информация
;        mov     rax,rsp
;        stdcall WriteDEC_64, rax, 0, 23, clBlack, clGreen,11b,cFwd

        ;       заголовки столбцов таблицы
        stdcall Write_64, txt_TestNum,     iTestNumX,         iTestTableY, clNone, clNone, cNoAttr, cBack
        stdcall Write_64, txt_TestName,    iTestNameX,        iTestTableY, clNone, clNone, cNoAttr, cBack
        stdcall Write_64, txt_TestStatus,  iTestStatusX,      iTestTableY, clNone, clNone, cNoAttr, cBack
        stdcall Write_64, txt_PassCurr,    iTestPassCurrX,    iTestTableY, clNone, clNone, cNoAttr, cBack
;        stdcall Write_64, txt_PassMax,     iTestPassMaxX,     iTestTableY, clNone, clNone, cNoAttr, cBack
;        stdcall Write_64, txt_PassPercent, iTestPassPercentX, iTestTableY, clNone, clNone, cNoAttr, cBack
;        stdcall Write_64, txt_TestPercent, iTestPercentX,     iTestTableY, clNone, clNone, cNoAttr, cBack
        stdcall Write_64, txt_CurrPattern, iTestPatternX,     iTestTableY, clNone, clNone, cNoAttr, cBack
        stdcall Write_64, txt_Errors,      iTestErrCountX,    iTestTableY, clNone, clNone, cNoAttr, cBack
        ;       информация о CPU и программе
        stdcall Write_64, CPUVendorName, 0,  0,  clNone, clNone, cNoAttr, cFwd
        stdcall Write_64, [CPUNameOffset],           0,  1,  clNone, clNone, cNoAttr, cFwd
        stdcall Write_64, WelcomeText,   77, 0, clNone, clBlack, cSym,    cBack
        stdcall Write_64, txt_CPUMode,   0,  2,  clNone, clNone, cNoAttr, cFwd
        stdcall Write_64, txt_64bitMode, 10, 2,  clNone, clNone, cNoAttr, cFwd
        ;       информация о памяти
        stdcall Write_64, txt_TotalRAM, 46, 1, clNone, clNone, cNoAttr, cFwd
        stdcall Write_64, txt_AvailRAM, 46, 2, clNone, clNone, cNoAttr, cFwd
        ;       размер памяти в МБ
        mov     rax,qword [MemSizeAvail]
        shr     rax,20
        stdcall WriteDEC_64, rax, 65, 1, clNone, clNone, cNoAttr, cBack
        mov     rax,qword [MemSizeType1]
        shr     rax,20
        stdcall WriteDEC_64, rax, 65, 2, clNone, clNone, cNoAttr, cBack
        stdcall Write_64, txt_Mb, 67, 1, clNone, clNone, cNoAttr, cFwd
        stdcall Write_64, txt_Mb, 67, 2, clNone, clNone, cNoAttr, cFwd
        ;       горячие клавиши
        stdcall Write_64, txt_ssProgStart, 0, 24,  clNone, clNone, cNoAttr, cFwd

        call    KbdClear_64
        mov     qword [CurrentTest],14
        call    TestDscFlush
        call    ShowTestTable

.MainLoop:
        call    KeyPressed_64  ; нажата ли клавиша?
        test    rax,rax
        jz      .NoKeyPressed
        call    KbdPop_64      ; если да - извлекаем скан из буфера
;        stdcall WriteHEX_64, rax, 0, 0, 0,0,0,2, cFwd
        cmp     rax,57h         ; показать полную карту памяти
        jne     .NoF11
        call    ShowMemMapFull
        jmp     .NoKeyPressed
.NoF11:
        cmp     rax,58h         ; выход из программы
        jne     .NoF12
        call    Exit_Dialog
        test    rax,rax
        jz      .NoKeyPressed
        call    KbdWait_64
        mov     al,0FEh
        out     64h,al
.NoF12:
        cmp     rax,3Bh
        jne     .NoF1
        stdcall Write_64, txt_ssTestRun, 0, 24,  clNone, clNone, cNoAttr, cFwd
        call    TestFullPrepare
        call    MemoryTest
        call    TestDscFlush
        stdcall Write_64, txt_ssProgStart, 0, 24,  clNone, clNone, cNoAttr, cFwd
        mov     qword [CurrentTest],14
        stdcall Bar_64, 0, 4,  80, 20, clDarkGray,   clBlack
        call    ShowTestTable
        call    KbdClear_64
        jmp     .NoKeyPressed
.NoF1:
        cmp     rax,3Ch
        jne     .NoF2
        stdcall Write_64, txt_ssTestRun, 0, 24,  clNone, clNone, cNoAttr, cFwd
        call    TestFastPrepare
        call    MemoryTest
        call    TestDscFlush
        stdcall Write_64, txt_ssProgStart, 0, 24,  clNone, clNone, cNoAttr, cFwd
        mov     qword [CurrentTest],14
        stdcall Bar_64, 0, 4,  80, 20, clDarkGray,   clBlack
        call    ShowTestTable
        call    KbdClear_64
.NoF2:
        jmp     .NoKeyPressed

.NoKeyPressed:

        jmp     .MainLoop


include 'dlgexit.inc'

Test_MSCAN:
        ret


;       МАРШЕВЫЙ ТЕСТ БЛОКА ПОСТОЯННЫМ ШАБЛОНОМ
;       вход:   Address, Size, Template, ^TestDesc
Test_March_ConstTemplate:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdi rsi
        mov     rdi,[rbp+16]  ; адрес блока
        mov     rcx,[rbp+24]  ; длина блока
        mov     rax,[rbp+32]  ; шаблон
        mov     rsi,[rbp+40]  ; адрес дескриптора теста
        shr     rcx,3         ; делим длину на 8
        ;       заполняем блок шаблоном
        push    rdi rcx
        cld
        rep     stosq
        pop     rcx rdi
        ;       сканируем блок
.NextScan:
        mov     rbx,[rdi]
        cmp     rbx,rax
        je      .ScanOK
        inc     qword [rsi + 7*8]
.ScanOK:
        add     rdi,8
        dec     rcx
        jnz     .NextScan
        pop     rsi rdi rcx rbx rax
        pop     rbp
        ret     32

;       МАРШЕВЫЙ ТЕСТ БЛОКА АДРЕСОМ
;       вход:   Address, Size, ^TestDesc
Test_March_AddrTemplate:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdi rsi
        mov     rdi,[rbp+16]  ; адрес блока
        mov     rcx,[rbp+24]  ; длина блока
        mov     rsi,[rbp+32]  ; адрес дескриптора теста
        shr     rcx,3         ; делим длину на 8
        ;       заполняем блок шаблоном
        push    rdi rcx
.NextFill:
        mov     qword [rdi],rdi
        add     rdi,8
        dec     rcx
        jnz     .NextFill
        pop     rcx rdi
        ;       сканируем блок
.NextScan:
        mov     rbx,[rdi]
        cmp     rbx,rdi
        je      .ScanOK
        inc     qword [rsi + 7*8]
.ScanOK:
        add     rdi,8
        dec     rcx
        jnz     .NextScan
        pop     rsi rdi rcx rbx rax
        pop     rbp
        ret     24

;       ТЕСТ С ПЕРЕМЕННЫМ ШАБЛОНОМ
;       вход:   Address, Size, Template, ^TestDesc
Test5_VariablePattern:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdi rsi r15
        mov     rdi,[rbp+16]  ; адрес блока
        mov     rcx,[rbp+24]  ; длина блока
        mov     rax,[rbp+32]  ; шаблон
        mov     rsi,[rbp+40]  ; адрес дескриптора теста
        shr     rcx,3         ; делим длину на 8
        ;       заполняем блок шаблоном
        mov     r15,3
.NextChange:
        not     rax
        push    rdi rcx
        cld
        rep     stosq
        pop     rcx rdi

        dec     r15
        jnz     .NextChange
        ;       сканируем блок
.NextScan:

        mov     rbx,qword [rdi]
        cmp     rbx,rax
        je      .ScanOK
        inc     qword [rsi + 7*8]
.ScanOK:
        add     rdi,8
        dec     rcx
        jnz     .NextScan
        pop     r15 rsi rdi rcx rbx rax
        pop     rbp
        ret     32

;       МАРШЕВЫЙ ТЕСТ БЛОКА ПОСТОЯННЫМ ШАБЛОНОМ
;       вход:   Address, Size, ^TestDesc
Test6_CellsOfInfluence:
        push    rbp
        mov     rbp,rsp
        push    rax rbx rcx rdi rsi r10
        mov     rdi,[rbp+16]  ; адрес блока
        mov     rcx,[rbp+24]  ; длина блока
        mov     rsi,[rbp+32]  ; адрес дескриптора теста
        shr     rcx,3         ; делим длину на 8
        ;       заполняем блок
        push    rdi rcx
        xor     r10,r10
        inc     r10
.NextFill:
        mov     qword [rdi],r10
        add     rdi,8
        rol     r10,1
        dec     rcx
        jnz     .NextFill
        pop     rcx rdi

        ;       сканируем блок
        xor     r10,r10
        inc     r10
        push    rdi rcx
.NextScan:
        mov     rbx,[rdi]
        cmp     rbx,r10
        je      .ScanOK
        inc     qword [rsi + 7*8]
.ScanOK:
        add     rdi,8
        rol     r10,1
        dec     rcx
        jnz     .NextScan
        pop     rcx rdi
        ;;;;;;;;
        ;       заполняем блок
        push    rdi rcx
        mov     r10,0FFFFFFFFFFFFFFFEh
.NextFill2:
        mov     qword [rdi],r10
        add     rdi,8
        rol     r10,1
        dec     rcx
        jnz     .NextFill2
        pop     rcx rdi
        ;       сканируем блок
        mov     r10,0FFFFFFFFFFFFFFFEh
        push    rdi rcx
.NextScan2:
        mov     rbx,[rdi]
        cmp     rbx,r10
        je      .ScanOK2
        inc     qword [rsi + 7*8]
.ScanOK2:
        add     rdi,8
        rol     r10,1
        dec     rcx
        jnz     .NextScan2
        pop     rcx rdi
        pop     r10 rsi rdi rcx rbx rax
        pop     rbp
        ret     24



;       вход:   начало блока, размер блока, адрес дескриптора теста, шаблон
Test1_Handler:
        push    rbp
        mov     rbp,rsp
        push    rax
        mov     rax,qword [rbp + 40]
        stdcall Test_March_ConstTemplate, qword [rbp+16], qword [rbp+24], rax, qword [rbp+32]
        pop     rax
        pop     rbp
        ret     32

;       вход:   начало блока, размер блока, адрес дескриптора теста, шаблон
Test2_Handler:
        push    rbp
        mov     rbp,rsp
        push    rax
        mov     rax,qword [rbp + 40]
        stdcall Test_March_ConstTemplate, qword [rbp+16], qword [rbp+24], rax, qword [rbp+32]
        pop     rax
        pop     rbp
        ret     32


;       вход:   начало блока, размер блока, адрес дескриптора теста, шаблон
Test3_Handler:
        push    rbp
        mov     rbp,rsp
        push    rax
        mov     rax,qword [rbp + 40]
        stdcall Test_March_ConstTemplate, qword [rbp+16], qword [rbp+24], rax, qword [rbp+32]
        pop     rax
        pop     rbp
        ret     32


;       вход:   начало блока, размер блока, адрес дескриптора теста, шаблон (не используется)
Test4_Handler:
        push    rbp
        mov     rbp,rsp
        stdcall Test_March_AddrTemplate, qword [rbp+16], qword [rbp+24], qword [rbp+32]
        pop     rbp
        ret     32

;       вход:   начало блока, размер блока, адрес дескриптора теста, шаблон
Test5_Handler:
        push    rbp
        mov     rbp,rsp
        stdcall Test5_VariablePattern, qword [rbp+16], qword [rbp+24], qword [rbp+40], qword [rbp+32]
        pop     rbp
        ret     32

;       вход:   начало блока, размер блока, адрес дескриптора теста, шаблон (не используется)
Test6_Handler:
        push    rbp
        mov     rbp,rsp
        stdcall Test6_CellsOfInfluence, qword [rbp+16], qword [rbp+24], qword [rbp+32]
        pop     rbp
        ret     32


MemoryTest:
        push    rbp
        mov     rbp,rsp
        xor     r10,r10           ; номер теста
.NextTest:
;        mov     rax,cr0
;        bt      rax,29
;        bt      rax,30
;        mov     cr0,rax

        mov     qword [CurrentTest],r10
        mov     r11,r10
        shl     r11,6             ; умножим номер дескриптора на 64 (размер дескриптора)
        add     r11,TestsDscTable ; + адрес начала таблицы дескрипторов тестов

        cmp     qword [r11],1
        je      .NoSkipThisTest
        mov     qword [r11 + 16],txt_Skipped
        jmp     .SkipThisTest
.NoSkipThisTest:

        mov     qword [r11 + 16],txt_Running
        mov     r8,[r11 + 32]     ; r8 - кол-во проходов
.NextPass:
        inc     qword [r11 + 24]  ; считаем проходы
        mov     r9,[MemMapEntryCount]
        xor     rsi,rsi
        add     rsi,24            ; пропуск первого МБ
        dec     r9
.NextBlock:
        cmp     dword [MemMap + rsi + 16],1
        jne     .SkipBlock              ; пропускаем недоступные блоки
        mov     rdi,[MemMap + rsi]      ; адрес блока
        mov     rcx,[MemMap + rsi + 8]  ; длина блока
        stdcall ShowMemMap, rdi

        mov     r12,[r11 + 40]         ; указатель на дескриптор списка шаблонов
        mov     r13,qword [r12]       ; количество шаблонов в списке
        test    r13,r13
        jnz     .No0
        inc     r13                   ; если шаблонов нет, то выполняем тест 1 раз
.No0:
        add     r12,8                    ; r12 - ссылка на список
        xor     r14,r14
.NextPattern:
        mov     r15,qword [r12 + r14 * 8]
        mov     qword [CurrentPattern],r15

        call    ShowTestTable

        call    KeyPressed_64
        cmp     rax,0
        jz      .NoKeyPressed
        call    KbdPop_64
        cmp     al,01h        ; TAB - skip test
        jz      .CancelTests

        cmp     al,0Fh        ; TAB - skip test
        jnz     .NoTAB
;        mov     qword [r11],0
        jmp     .SkipThisTest
.NoTAB:
.NoKeyPressed:

        stdcall qword [r11+48], rdi, rcx, r11, r15
        inc     r14
        dec     r13
        jnz     .NextPattern
        jmp     .SkipBlock
.SkipBlock:
        add     rsi,24                  ; адрес следующей записи в MemMap
        dec     r9
        jnz     .NextBlock              ; цикл по числу блоков

;        mov     r15,2000000000h
;        mov     qword [r15],0



        dec     r8
        jnz     .NextPass               ; цикл по числу проходов

        jmp     .m2
.SkipThisTest:
        mov     qword [r11 + 16],txt_Skipped
        jmp     .m1
.m2:
        mov     qword [r11 + 16],txt_Passed
.m1:
        inc     r10
        cmp     r10,MaxTestCount    ; цикл по числу тестов
        jne     .NextTest
.CancelTests:

        pop     rbp
        ret



;       вывод на экран карты распределения памяти (выводятся только блоки типа 1)
;       вход:   текущий адрес
ShowMemMap:
        push    rbp
        mov     rbp,rsp
        push    rax rcx rsi r8 r9 r10 r11 r12
        mov     r11,[rbp+16]
        mov     rcx,[MemMapEntryCount]
        mov     rax,iMapWindowY
        xor     rsi,rsi
.NextView:
        cmp     dword [MemMap + rsi + 16],1
        jne     .NoView
        mov     r12,clDarkGray
        mov     r8,qword [MemMap + rsi]
        cmp     r8,r11
        jne     .NoCurrAddr
        mov     r12,clYellow
.NoCurrAddr:
        mov     r9,qword [MemMap + rsi + 8]
        stdcall WriteHEX_64, r8,        1,  rax, r12, clNone, cFon, 16, cFwd
        lea     r10,[r8+r9-1]
        stdcall WriteHEX_64, r10,       18, rax, r12, clNone, cFon, 16, cFwd
        stdcall Write_64,    txt_Razd,  17, rax, r12, clNone, cFon,     cFwd
        stdcall Write_64,    txt_Kb,    44, rax, clNone, clNone, cNoAttr, cFwd
        shr     r9,10
        stdcall WriteDEC_64, r9,        42, rax, clNone, clNone, cNoAttr, cBack
        inc     rax
.NoView:
        add     rsi,24
        dec     rcx
        jnz    .NextView
        pop     r12 r11 r10 r9 r8 rsi rcx rax
        pop     rbx
        ret     8


include 'memmap.inc'



ShowTestTable:
        push    rax rbx rcx rsi rdi r8
        xor     rcx,rcx
        xor     r8,r8
.NextTest:
        mov     rsi,rcx
        shl     rsi,6             ; умножим номер дескриптора на 64 (размер дескриптора)
        add     rsi,TestsDscTable ; + адрес начала таблицы дескрипторов тестов

        lea     rax,[iTestTableY + rcx + 1]
        mov     rbx,clDarkGray
        cmp     rcx,qword [CurrentTest]
        jne     .NoCurr
        mov     rbx,clYellow

.NoCurr:
        stdcall Bar_64,      0, rax, 80, 1, rbx, clBlack
        inc     r8
        stdcall WriteDEC_64, r8,               iTestNumX,         rax, clNone, clNone, cNoAttr, cBack
        stdcall Write_64,    qword [rsi + 8],  iTestNameX,        rax, clNone, clNone, cNoAttr, cBack
        stdcall Write_64,    qword [rsi + 16], iTestStatusX,      rax, clNone, clNone, cNoAttr, cBack
        stdcall WriteDEC_64, qword [rsi + 24], iTestPassCurrX,  rax, clNone, clNone, cNoAttr, cBack
;        stdcall Write_64,    txt_Slash,        iTestPassCurrX-3,  rax, clNone, clNone, cNoAttr, cBack
 ;       stdcall WriteDEC_64, qword [rsi + 32], iTestPassCurrX,    rax, clNone, clNone, cNoAttr, cBack
        cmp     rcx,qword [CurrentTest]
        jne     .Skip
        mov     rdi,qword [rsi + 40]
        cmp     qword [rdi],0
        jne     .NoDiff
        stdcall Write_64,    txt_DiffTmp,      iTestPatternX,        rax, clNone, clNone, cNoAttr, cBack
        jmp     .Skip
.NoDiff:
        stdcall WriteHEX_64, qword [CurrentPattern], iTestPatternX,     rax, clNone, clNone, cNoAttr, 16, cBack
.Skip:
        stdcall WriteDEC_64, qword [rsi + 56], iTestErrCountX,    rax, clNone, clNone, cNoAttr, cBack

        inc     rcx
        cmp     rcx,MaxTestCount
        jne     .NextTest
        pop     r8 rdi rsi rcx rbx rax
        ret


CurrentTest     dq      0
CurrentPattern  dq      0

txt_DiffTmp     db      'Different patterns',0
txt_CurrPattern db      'Current pattern',0

txt_TestNum     db      '#',0
txt_TestName    db      'Test name',0
txt_TestStatus  db      'Status',0
txt_PassCurr    db      'Percent',0
txt_PassMax     db      'Total pass',0
txt_PassPercent db      'Pass %',0
txt_TestPercent db      'Test %',0
txt_Errors      db      'Errors',0

txt_Slash       db      '/',0
txt_Razd        db      ':',0

txt_Passed      db      'Passed',0
txt_Running     db      'Running',0
txt_Waiting     db      'Waiting',0
txt_Skipped     db      'Skipped',0
txt_Ready       db      'Ready',0


txt_Test1Name   db      'Zero-One (MSCAN)',0
txt_Test2Name   db      'Checkerboard',0
txt_Test3Name   db      'Address test',0
txt_Test4Name   db      'Moving inversions',0
txt_Test5Name   db      'MATS',0
txt_Test6Name   db      'MATS+',0
txt_Test7Name   db      'Marching 1/0',0
txt_Test8Name   db      'MATS++',0

txt_Test9Name    db      'MARCH X',0
txt_Test10Name   db      'MARCH C',0
txt_Test11Name   db      'MARCH A',0
txt_Test12Name   db      'MARCH Y',0
txt_Test13Name   db      'MARCH B',0

txt_ssProgStar  db      ' F1 - Full scan   F2 - Fast scan ENTER - Start    F11 - Memory map   F12 - Exit ',0
txt_ssProgStart db      ' F1 - Full scan   F2 - Fast scan             F11 - Show memory map   F12 - Exit ',0
txt_ssMemShow   db      '                                             F11 - Hide memory map              ',0
txt_ssExitDlg   db      ' Left, Right - Button selection   ENTER - confirm the action                    ',0
txt_ssTestRun   db      ' ESC - Cancel   TAB - Skip current test                                         ',0

txt_MemMapWindowTitle   db      'Memory Map:',0
txt_Start               db      'Start:',0
txt_End                 db      'End:',0
txt_Size                db      'Size:',0
txt_Type                db      'Type:',0

txt_Type1               db      'Memory',0
txt_Type2               db      'Reserved',0
txt_Type3               db      'ACPI',0
txt_Type4               db      'NVS',0
txt_Type5               db      'Unusable',0




struc   TEST_DESCRIPTOR Num, HdlAddr, NamePtr, StatusPtr, MaxPass, TmpListPtr
{
        .Num            dq      Num
        .Name           dq      NamePtr      ; 8
        .Status         dq      StatusPtr    ; 16
        .PassCurr       dq      0            ; 24
        .PassMax        dq      MaxPass      ; 32
        .PatternList    dq      TmpListPtr   ; 40  ; указатель на список шаблонов
        .HdlAddr        dq      HdlAddr      ; 48
        .ErrCnt         dq      0            ; 56
}

TestsDscTable:
Test1Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test1Name, txt_Waiting, 100, TemplateList1
Test2Dsc        TEST_DESCRIPTOR 0, Test2_Handler, txt_Test2Name, txt_Waiting, 100, TemplateList2
Test3Dsc        TEST_DESCRIPTOR 1, Test4_Handler, txt_Test3Name, txt_Waiting, 100, TemplateList4
Test4Dsc        TEST_DESCRIPTOR 1, Test3_Handler, txt_Test4Name, txt_Waiting, 100, TemplateList3
Test5Dsc        TEST_DESCRIPTOR 1, Test5_Handler, txt_Test5Name, txt_Waiting, 100, TemplateList5
Test6Dsc        TEST_DESCRIPTOR 1, Test6_Handler, txt_Test6Name, txt_Waiting, 100, TemplateList6
Test7Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test7Name, txt_Waiting, 100, TemplateList7
Test8Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test8Name, txt_Waiting, 100, TemplateList8

Test9Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test9Name,  txt_Waiting, 100, TemplateList8
Test10Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test10Name, txt_Waiting, 100, TemplateList8
Test11Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test11Name, txt_Waiting, 100, TemplateList8
Test12Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test12Name, txt_Waiting, 100, TemplateList8
Test13Dsc        TEST_DESCRIPTOR 1, Test1_Handler, txt_Test13Name, txt_Waiting, 100, TemplateList8

TestDscSize     = Test2Dsc - Test1Dsc


TestFullPrepare:
        mov     qword [Test1Dsc.Num],1
        mov     qword [Test2Dsc.Num],1
        mov     qword [Test3Dsc.Num],1
        mov     qword [Test4Dsc.Num],1
        mov     qword [Test5Dsc.Num],1
        mov     qword [Test6Dsc.Num],1
        mov     qword [Test7Dsc.Num],1
        mov     qword [Test8Dsc.Num],1
        mov     qword [Test9Dsc.Num],1
        mov     qword [Test10Dsc.Num],1
        mov     qword [Test11Dsc.Num],1
        mov     qword [Test12Dsc.Num],1
        mov     qword [Test13Dsc.Num],1

        mov     qword [Test1Dsc.Status],txt_Waiting
        mov     qword [Test2Dsc.Status],txt_Waiting
        mov     qword [Test3Dsc.Status],txt_Waiting
        mov     qword [Test4Dsc.Status],txt_Waiting
        mov     qword [Test5Dsc.Status],txt_Waiting
        mov     qword [Test6Dsc.Status],txt_Waiting
        mov     qword [Test7Dsc.Status],txt_Waiting
        mov     qword [Test8Dsc.Status],txt_Waiting
        mov     qword [Test9Dsc.Status],txt_Waiting
        mov     qword [Test10Dsc.Status],txt_Waiting
        mov     qword [Test11Dsc.Status],txt_Waiting
        mov     qword [Test12Dsc.Status],txt_Waiting
        mov     qword [Test13Dsc.Status],txt_Waiting
        ret

TestFastPrepare:
        mov     qword [Test1Dsc.Num],1
        mov     qword [Test2Dsc.Num],1
        mov     qword [Test3Dsc.Num],1
        mov     qword [Test4Dsc.Num],0
        mov     qword [Test5Dsc.Num],0
        mov     qword [Test6Dsc.Num],0
        mov     qword [Test7Dsc.Num],1
        mov     qword [Test8Dsc.Num],1
        mov     qword [Test9Dsc.Num],1
        mov     qword [Test10Dsc.Num],1
        mov     qword [Test11Dsc.Num],1
        mov     qword [Test12Dsc.Num],1
        mov     qword [Test13Dsc.Num],1
        mov     qword [Test1Dsc.Status],txt_Waiting
        mov     qword [Test2Dsc.Status],txt_Waiting
        mov     qword [Test3Dsc.Status],txt_Waiting
        mov     qword [Test4Dsc.Status],txt_Skipped
        mov     qword [Test5Dsc.Status],txt_Skipped
        mov     qword [Test6Dsc.Status],txt_Skipped
        mov     qword [Test7Dsc.Status],txt_Waiting
        mov     qword [Test8Dsc.Status],txt_Waiting
        mov     qword [Test9Dsc.Status],txt_Waiting
        mov     qword [Test10Dsc.Status],txt_Waiting
        mov     qword [Test11Dsc.Status],txt_Waiting
        mov     qword [Test12Dsc.Status],txt_Waiting
        mov     qword [Test13Dsc.Status],txt_Waiting

        ret



TestDscFlush:
        mov     qword [Test1Dsc.Status],txt_Ready
        mov     qword [Test2Dsc.Status],txt_Ready
        mov     qword [Test3Dsc.Status],txt_Ready
        mov     qword [Test4Dsc.Status],txt_Ready
        mov     qword [Test5Dsc.Status],txt_Ready
        mov     qword [Test6Dsc.Status],txt_Ready
        mov     qword [Test7Dsc.Status],txt_Ready
        mov     qword [Test8Dsc.Status],txt_Ready
        mov     qword [Test9Dsc.Status],txt_Ready
        mov     qword [Test10Dsc.Status],txt_Ready
        mov     qword [Test11Dsc.Status],txt_Ready
        mov     qword [Test12Dsc.Status],txt_Ready
        mov     qword [Test13Dsc.Status],txt_Ready

        mov     qword [Test1Dsc.PassCurr],0
        mov     qword [Test2Dsc.PassCurr],0
        mov     qword [Test3Dsc.PassCurr],0
        mov     qword [Test4Dsc.PassCurr],0
        mov     qword [Test5Dsc.PassCurr],0
        mov     qword [Test6Dsc.PassCurr],0
        mov     qword [Test7Dsc.PassCurr],0
        mov     qword [Test8Dsc.PassCurr],0
        mov     qword [Test9Dsc.PassCurr],0
        mov     qword [Test10Dsc.PassCurr],0
        mov     qword [Test11Dsc.PassCurr],0
        mov     qword [Test12Dsc.PassCurr],0
        mov     qword [Test13Dsc.PassCurr],0
        ret





TemplateList1:
        .EntryCount     dq      2
        .TmpList        dq      0
                        dq      -1

TemplateList2:
        .EntryCount     dq      10
        .TmpList        dq      05555555555555555h
                        dq      0AAAAAAAAAAAAAAAAh
                        dq      05A5A5A5A5A5A5A5Ah
                        dq      0A5A5A5A5A5A5A5A5h
                        dq      055AA55AA55AA55AAh
                        dq      0AA55AA55AA55AA55h
                        dq      0AAAA5555AAAA5555h
                        dq      05555AAAA5555AAAAh
                        dq      0AAAAAAAA55555555h
                        dq      055555555AAAAAAAAh

TemplateList3:
        .EntryCount     dq      128
        .TmpList:
                        rept 64 c:0
                        {
                                dq      (1 shl c)
                        }
                        rept 64 c:0
                        {
                                dq      (1 shl c) xor 0FFFFFFFFFFFFFFFFh
                        }

TemplateList4:
        .EntryCount     dq      0
        .TmpList        dq      0


TemplateList5:
        .EntryCount     dq      2
        .TmpList        dq      05555555555555555h
                        dq      0AAAAAAAAAAAAAAAAh

TemplateList6:
        .EntryCount     dq      0
        .TmpList        dq      0

TemplateList7:
        .EntryCount     dq      0
        .TmpList        dq      0

TemplateList8:
        .EntryCount     dq      0
        .TmpList        dq      0
