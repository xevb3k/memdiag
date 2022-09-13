
;       64-��⭠� IDT ��� LM


cIntGate_64     equ     1110b
cTrapGate_64    equ     1111b
cNoGate_64      equ     0
cIDT_Size_64    =       IDT_End_64 - IDT_64

;       ������� ��।����� 64-��⭮�� �
struc   DEFINE_GATE_64 Handler,GateType,IST
{
        dw      Handler and 0FFFFh                 ; ���� 15-0 ᬥ饭��
        dw      CODE_SELECTOR_64                   ; ᥫ���� ᥣ����
        dw      8000h or (GateType shl 8) or IST   ; 15         - ������⢨�, ࠢ�� 1
                                                   ; 14-13      - DPL
                                                   ; 12         - 0
                                                   ; 11-8       - ⨯ �
                                                   ; 7-3        - 0
                                                   ; 2-0        - IST
        dw      (Handler and 0FFFF0000h) shr 16    ; ���� 31-16 ᬥ饭��
        dd      Handler shr 32                     ; ���� 63-32 ᬥ饭��
        dd      0
}

align   8

;       ⠡��� ���ਯ�஢ ���뢠���

IDT_64:

;       00h - 1Fh ���ਯ��� �᪫�祭��
Int00h_64  DEFINE_GATE_64 Int00Handle_64, cIntGate_64, 0
Int01h_64  DEFINE_GATE_64 Int01Handle_64, cIntGate_64, 0
Int02h_64  DEFINE_GATE_64 Int02Handle_64, cIntGate_64, 0
Int03h_64  DEFINE_GATE_64 Int03Handle_64, cIntGate_64, 0
Int04h_64  DEFINE_GATE_64 Int04Handle_64, cIntGate_64, 0
Int05h_64  DEFINE_GATE_64 Int05Handle_64, cIntGate_64, 0
Int06h_64  DEFINE_GATE_64 Int06Handle_64, cIntGate_64, 0
Int07h_64  DEFINE_GATE_64 Int07Handle_64, cIntGate_64, 0
Int08h_64  DEFINE_GATE_64 Int08Handle_64, cIntGate_64, 0
Int09h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int0Ah_64  DEFINE_GATE_64 Int0AHandle_64, cIntGate_64, 0
Int0Bh_64  DEFINE_GATE_64 Int0BHandle_64, cIntGate_64, 0
Int0Ch_64  DEFINE_GATE_64 Int0CHandle_64, cIntGate_64, 0
Int0Dh_64  DEFINE_GATE_64 Int0DHandle_64, cIntGate_64, 0
Int0Eh_64  DEFINE_GATE_64 Int0EHandle_64, cIntGate_64, 0
Int0Fh_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int10h_64  DEFINE_GATE_64 Int10Handle_64, cIntGate_64, 0
Int11h_64  DEFINE_GATE_64 Int11Handle_64, cIntGate_64, 0
Int12h_64  DEFINE_GATE_64 Int12Handle_64, cIntGate_64, 0
Int13h_64  DEFINE_GATE_64 Int13Handle_64, cIntGate_64, 0
Int14h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int15h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int16h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int17h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int18h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int19h_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int1Ah_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int1Bh_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int1Ch_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int1Dh_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int1Eh_64  DEFINE_GATE_64 0,              cNoGate_64,  0
Int1Fh_64  DEFINE_GATE_64 0,              cNoGate_64,  0


;       ���ਯ��� ���뢠���
Int20h_64  DEFINE_GATE_64 Int20Handle_64, cIntGate_64, 0   ; IRQ0 ⠩���
Int21h_64  DEFINE_GATE_64 Int21Handle_64, cIntGate_64, 0   ; IRQ1 ���������
Int22h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int23h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int24h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int25h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int26h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int27h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int28h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int29h_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int2Ah_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int2Bh_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int2Ch_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int2Dh_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int2Eh_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0
Int2Fh_64  DEFINE_GATE_64 OtherIRQ_64,    cIntGate_64, 0


;       30h - FFh - �ணࠬ��� ���뢠���, �� �ᯮ�������

times   0FFh-30h+1      dq      0,0



IDT_End_64: