; Домашнее задание №2
; Автор: Борисов Костя
; Группа: БПИ199
; Дата: 20.09.2020
; Вариант: 3 (суммы соседних элементов)

format PE console
entry start
include 'win32a.inc'

STD_OUTPUT_HANDLE = -11

section '.code' code readable executable
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
    push dword[arrA]
    call [free]
    add esp, 4
    push dword[arrB]
    call [free]
    add esp, 4

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

    push filemodeWriteStr
    push 2
    call [fdopen]
    add esp, 8
    mov [stderr], eax
    ret

  Array$push:
    mov eax, [esp+4]
    push esi
    push ebx
    mov ebx, eax

    mov esi, [arrA]
    test esi, esi
    jnz Array$push_hasPointer

    push 2*4
    call [malloc]
    add esp, 4
    mov [arrA], eax
    mov esi, eax
    mov dword[arrAcap], 2*4
    mov dword[arrAsize], 0

    Array$push_hasPointer:
    mov eax, [arrAcap]
    cmp eax, [arrAsize]
    jne Array$push_hasSpace
    shl eax, 1
    mov [arrAcap], eax

    push eax
    push esi
    call [realloc]
    add esp, 8
    mov esi, eax

    Array$push_hasSpace:
    mov eax, [arrAsize]
    add esi, eax
    mov [esi], ebx
    add eax, 4
    mov [arrAsize], eax

    pop ebx
    pop esi
    ret 4

  Array$print:
    mov eax, [esp+4]
    push esi
    push ebx
    mov ebx, [esp+16]
    add ebx, eax
    mov esi, eax

    push jsonStartStr
    call [printf]
    add esp, 4

    mov eax, [esp+16]
    test eax, eax
    jz Array$print_loopEnd

    Array$print_loop:
    push dword[esi]
    push jsonNumberStr
    call [printf]
    add esp, 8
    add esi, 4

    cmp esi, ebx
    jge Array$print_loopEnd

    push jsonCommaStr
    call [printf]
    add esp, 4

    jmp Array$print_loop
    Array$print_loopEnd:
    push jsonEndStr
    call [printf]
    add esp, 4


    pop ebx
    pop esi
    ret 8

  readFile:
    push esi ; FILE*

    push filemodeReadStr
    push dword[inputFilename]
    call [fopen]
    add esp, 8
    mov esi, eax
    test eax, eax
    jnz readFile_loopStart

    push dword[inputFilename]
    call [perror]
    add esp, 4

    readFile_loopStart:
    push scanfTrashStr
    push esi
    call [fscanf]
    add esp, 8

    readFile_loop:
    push tempNumberForScanf
    push scanfNumberStr
    push esi
    call [fscanf]
    add esp, 12
    cmp eax, 0
    jle readFile_end

    push dword[tempNumberForScanf]
    call Array$push

    jmp readFile_loop
    readFile_end:
    mov eax, [arrA]
    pop esi
    ret

  makeSecondArray:
    push esi
    push ebx
    push edi

    mov eax, [arrAsize]
    test eax, eax
    jz makeSecondArray_fail
    sub eax, 1
    push eax
    call [malloc]
    add esp, 4
    mov [arrB], eax

    mov esi, [arrA]
    mov edi, esi
    add edi, [arrAsize]
    makeSecondArray_loop:
    mov ebx, [esi]
    add ebx, [esi+4]
    mov [eax], ebx
    add eax, 4
    add esi, 4
    cmp edi, esi
    jne makeSecondArray_loop

    jmp makeSecondArray_ret
    makeSecondArray_fail:
    mov eax, [arrA]
    mov [arrB], eax
    mov dword[arrAsize], 4

    makeSecondArray_ret:
    pop edi
    pop ebx
    pop esi
    ret

  start:
    call dramaticEntrance

    mov eax, [argv]
    mov eax, [eax+4]
    mov [inputFilename], eax
    cmp dword[argc], 1
    jne start_readFile

    push noargvStr
    call [printf]
    add esp, 4
    push pathmax
    push scanfPathStr
    call [scanf]
    add esp, 8
    mov dword[inputFilename], pathmax

    start_readFile:
    push dword[inputFilename]
    push filenameStr
    call [printf]
    add esp, 8

    call readFile

    push arrAStr
    call [printf]
    add esp, 4
    push dword[arrAsize]
    push dword[arrA]
    call Array$print

    call makeSecondArray

    push arrBStr
    call [printf]
    add esp, 4
    mov eax, [arrAsize]
    sub eax, 4
    push eax
    push dword[arrB]
    call Array$print

    call gracefulExit
    ret

section '.data' data readable writable
  filemodeWriteStr: db 'wb',0
  filemodeReadStr: db 'rb',0
  filenameStr: db 'читаем файл %s!',10,0
  arrAStr: db 'массив A: ',0
  arrBStr: db 'массив B: ',0
  noargvStr: db 'из какого файла нам надо читать массив A (CON = колнсолька, ctrl+d или -- = конец)? ',0
  scanfPathStr: db '%259[^',10,']',0
  scanfTrashStr: db '%*[^-+0-9',4,']',0
  scanfNumberStr: db '%i%*[^-+0-9',4,']',0
  printfNumberStr: db '%d',10,0
  jsonStartStr: db '[',0
  jsonNumberStr: db '%d',0
  jsonCommaStr: db ', ',0
  jsonEndStr: db ']',10,0
  exitStr: db 'ты открыли эту программу не из терминала((',10,'поэтому для выхода из неё тебе надо нажать Enter',10,0
  stdoutHandle: dd 0
  consoleStructPointer: dd 0
  consoleWindowHandle: dd 0
  consoleWindowProcessId: dd 0
  argv: dd 0
  argc: dd 0
  env: dd 0
  stderr: dd 0
  arrA: dd 0
  arrAsize: dd 0
  arrAcap: dd 0
  arrB: dd 0
  inputFilename: dd 0
  tempNumberForScanf: dd 0
  pathmax: repeat 260
    db 0
  end repeat

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll', user32, 'user32.dll'

  import msvcrt, \
    printf, 'printf', fprintf, 'fprintf', scprintf, '_scprintf', snprintf, 'snprintf', \
    scanf, 'scanf', fscanf, 'fscanf', fopen, 'fopen', perror, 'perror', \
    malloc, 'malloc', realloc, 'realloc', free, 'free', \
    memcpy, 'memcpy', strcmp, 'strcmp', strlen, 'strlen', \
    exit, '_exit', getch, '_getch', getmainargs, '__getmainargs', fdopen, '_fdopen'

  import kernel32, \
    GetStdHandle, 'GetStdHandle', \
    GetConsoleScreenBufferInfo, 'GetConsoleScreenBufferInfo', \
    SetConsoleOutputCP, 'SetConsoleOutputCP', \
    GetConsoleWindow, 'GetConsoleWindow', \
    GetCurrentProcessId, 'GetCurrentProcessId'

  import user32, GetWindowThreadProcessId, 'GetWindowThreadProcessId'
