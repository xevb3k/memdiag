;       32-битная IDT для PM


cIntGate_32     equ     10001110b
cTrapGate_32    equ     10001111b
cNoGate_32      equ     0
cIDT_Size_32    =       IDT_End_32 - IDT_32

struc DEFINE_GATE_32 Handler,GateType
{
        dw      Handler and 0FFFFh  ; биты 15-0 смещения
        dw      CODE_SELECTOR_32    ; селектор сегмента
        db      0                   ; 0 для всех шлюзов, кроме TSS
        db      GateType            ; тип шлюза (8E - ловушка, 8F - прерывание)
                                    ; 0-3 - тип шлюза
                                    ; 4   - тип дескриптора (0 - системный)
                                    ; 5-6 - DPL
                                    ; 7   - бит присутствия, 1
        dw      Handler shr 16      ; биты 31-16 смещения
}


align   8

;       таблица дескрипторов прерываний

IDT_32:
;       00h - 1Fh дескрипторы исключений
Int00h_32  DEFINE_GATE_32 Int00Handle_32, cIntGate_32
Int01h_32  DEFINE_GATE_32 Int01Handle_32, cIntGate_32
Int02h_32  DEFINE_GATE_32 Int02Handle_32, cIntGate_32
Int03h_32  DEFINE_GATE_32 Int03Handle_32, cIntGate_32
Int04h_32  DEFINE_GATE_32 Int04Handle_32, cIntGate_32
Int05h_32  DEFINE_GATE_32 Int05Handle_32, cIntGate_32
Int06h_32  DEFINE_GATE_32 Int06Handle_32, cIntGate_32
Int07h_32  DEFINE_GATE_32 Int07Handle_32, cIntGate_32
Int08h_32  DEFINE_GATE_32 Int08Handle_32, cIntGate_32
Int09h_32  DEFINE_GATE_32 0,              cNoGate_32
Int0Ah_32  DEFINE_GATE_32 Int0AHandle_32, cIntGate_32
Int0Bh_32  DEFINE_GATE_32 Int0BHandle_32, cIntGate_32
Int0Ch_32  DEFINE_GATE_32 Int0CHandle_32, cIntGate_32
Int0Dh_32  DEFINE_GATE_32 Int0DHandle_32, cIntGate_32
Int0Eh_32  DEFINE_GATE_32 Int0EHandle_32, cIntGate_32
Int0Fh_32  DEFINE_GATE_32 0,              cNoGate_32
Int10h_32  DEFINE_GATE_32 Int10Handle_32, cIntGate_32
Int11h_32  DEFINE_GATE_32 Int11Handle_32, cIntGate_32
Int12h_32  DEFINE_GATE_32 Int12Handle_32, cIntGate_32
Int13h_32  DEFINE_GATE_32 0,              cNoGate_32
Int14h_32  DEFINE_GATE_32 0,              cNoGate_32
Int15h_32  DEFINE_GATE_32 0,              cNoGate_32
Int16h_32  DEFINE_GATE_32 0,              cNoGate_32
Int17h_32  DEFINE_GATE_32 0,              cNoGate_32
Int18h_32  DEFINE_GATE_32 0,              cNoGate_32
Int19h_32  DEFINE_GATE_32 0,              cNoGate_32
Int1Ah_32  DEFINE_GATE_32 0,              cNoGate_32
Int1Bh_32  DEFINE_GATE_32 0,              cNoGate_32
Int1Ch_32  DEFINE_GATE_32 0,              cNoGate_32
Int1Dh_32  DEFINE_GATE_32 0,              cNoGate_32
Int1Eh_32  DEFINE_GATE_32 0,              cNoGate_32
Int1Fh_32  DEFINE_GATE_32 0,              cNoGate_32

;       дескрипторы прерываний
Int20h_32  DEFINE_GATE_32 Int20Handle_32, cIntGate_32     ; IRQ0 таймер
Int21h_32  DEFINE_GATE_32 Int21Handle_32, cIntGate_32     ; IRQ1 клавиатура
Int22h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int23h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int24h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int25h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int26h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int27h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int28h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int29h_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int2Ah_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int2Bh_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int2Ch_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int2Dh_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int2Eh_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32
Int2Fh_32  DEFINE_GATE_32 OtherIRQ_32,    cIntGate_32


;       30h - FFh - программные прерывания, не используются

times   0FFh-30h+1      dq      0

IDT_End_32:
