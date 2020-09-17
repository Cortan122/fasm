format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
  start:
    mov eax, 1

  loop1:
    mov ebx, 0

    push eax
    mov ecx, 3
    mov edx, 0
    div ecx ; edx = eax % ecx
    pop eax

    cmp edx, 0
    jne if1end
    push eax
    push fizzStr
    call [printf]
    pop eax
    pop eax
    mov ebx, 1
  if1end:

    push eax
    mov ecx, 5
    mov edx, 0
    div ecx ; edx = eax % ecx
    pop eax

    cmp edx, 0
    jne if2end
    push eax
    push buzzStr
    call [printf]
    pop eax
    pop eax
    mov ebx, 1
  if2end:

    push eax
    cmp ebx, 0
    jne if3else
    push formatStr
    jmp if3end
  if3else:
    push newlineStr
  if3end:
    call [printf]
    pop eax
    pop eax

  loop1end:
    add eax, 1
    cmp eax, 100
    jne loop1

    push 0
    call [exit]

section '.data' data readable writable
  fizzStr: db 'Fizz',0
  buzzStr: db 'Buzz',0
  formatStr: db '%d',10,0
  newlineStr: db 10,0
  n: dd 0

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll'
  import msvcrt, printf, 'printf', exit, '_exit', scanf, 'scanf'

