;       �⠭����� �맮� �㭪樨
;       ��ࠬ���� ��������� � �⥪ � ���⭮� ���浪�

macro   stdcall proc, [arg]
{
        reverse push arg
        common  call proc
}


;       ����頥� 4-���� ����稭� clFon � clSyb
;       � ������ � ������� ��� 8-��⭮�� reg ᮮ⢥��⢥���
macro   movcl   reg, clFon, clSym
{
        mov     reg,(clFon shl 4) or clSym
}
