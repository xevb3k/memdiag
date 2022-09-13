;       стандартный вызов функции
;       параметры помещаются в стек в обратном порядке

macro   stdcall proc, [arg]
{
        reverse push arg
        common  call proc
}


;       помещает 4-битные величины clFon и clSyb
;       в старшую и младшую части 8-битного reg соответственно
macro   movcl   reg, clFon, clSym
{
        mov     reg,(clFon shl 4) or clSym
}

