format PE console
entry start
include 'win32a.inc'

STD_OUTPUT_HANDLE = -11

section '.code' code readable executable
  sttysize:
    push STD_OUTPUT_HANDLE
    call [GetStdHandle]
    mov [stdoutHandle], eax

    push consoleStruct
    push dword[stdoutHandle]
    call [GetConsoleScreenBufferInfo]

    mov ax, [consoleStruct]
    and eax, 0xffff
    ret

  isGetchNeeded:
    call [GetConsoleWindow]
    mov [consoleWindowHandle], eax

    push consoleWindowProcessId
    push eax
    call [GetWindowThreadProcessId]

    call [GetCurrentProcessId]
    cmp eax, [consoleWindowProcessId]
    mov eax, 0
    jne isGetchNeeded_ret
    mov eax, 1
    isGetchNeeded_ret:
    test eax, eax
    ret

  gracefulExit:
    call isGetchNeeded
    jz gracefulExit_exit

    push exitStr
    call [printf]
    add esp, 4
    call [getch]

    gracefulExit_exit:
    push 0
    call [exit]
    add esp, 4
    ret

  dramaticEntrance:
    push 65001
    call [SetConsoleOutputCP]

    push env
    push 0
    push env
    push argv
    push argc
    call [getmainargs]
    add esp, 20
    ret

  start:
    call dramaticEntrance

    call sttysize
    mov ebx, [argv]
    push dword[ebx+4]
    push eax
    push helloStr
    call [printf]

    call gracefulExit
    ret

section '.data' data readable writable
  helloStr: db 'привет! у тебя консолька в %d столбцов и argv[1]="%s"',10,0
  exitStr: db 'ты открыли эту программу не из терминала((',10,'поэтому для выхода из неё тебе надо нажать Enter',10,0
  stdoutHandle: dd 0
  consoleWindowHandle: dd 0
  consoleWindowProcessId: dd 0
  argv: dd 0
  argc: dd 0
  env: dd 0
  consoleStruct: rb 22

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll', user32, 'user32.dll'

  import msvcrt, printf, 'printf', exit, '_exit', getch, '_getch', getmainargs, '__getmainargs' ; нам free ненужен))

  import kernel32, \
    GetStdHandle, 'GetStdHandle', \
    GetConsoleScreenBufferInfo, 'GetConsoleScreenBufferInfo', \
    SetConsoleOutputCP, 'SetConsoleOutputCP', \
    GetConsoleWindow, 'GetConsoleWindow', \
    GetCurrentProcessId, 'GetCurrentProcessId'

  import user32, GetWindowThreadProcessId, 'GetWindowThreadProcessId'
