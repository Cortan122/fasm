format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
  start:
    mov eax, 0

  loop1:
    push eax
    push formatStr
    call [printf]
    pop eax
    pop eax

  loop1end:
    add eax, 1
    cmp eax, 10
    jne loop1

    push 0
    call [exit]

section '.data' data readable writable
  formatStr: db '%d',10,0

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll'
  import msvcrt, printf, 'printf', exit, '_exit', scanf, 'scanf'

