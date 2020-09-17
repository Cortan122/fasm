format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
  start:
    push helloStr
    call [printf]
    add esp, 4 ; тут мы убераем наши аргументы со стака (один аргумент а байт 4)

    push n ; аргументы тут в обратном порядке (точнее это стек у нас перевёрнутый)
    push scanfStr
    call [scanf]
    add esp, 8 ; тут аргумента 2

    push dword[n]
    push printStr
    call [printf]
    add esp, 8

    push 0
    call [exit]

section '.data' data readable writable
  helloStr: db 'Привет!! Мне нужно чтобы ты ввели число',10,0 ; db это DataByte, а 10 это \n
  scanfStr: db '%d',0
  n: dd 0 ; тут dd = DataDword (dword это int32)
  printStr: db 'Ты ввели %d',10,0

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll'
  import msvcrt, printf, 'printf', exit, '_exit', scanf, 'scanf'

