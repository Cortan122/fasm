format PE console 6.0
include 'win32a.inc'
entry start

section '.data' data readable writeable
format_f db "%2.8f",10,0
val1 dd 1,0
val2 dd 4,0
decnum dd 1110000.0,0
decnum2 dd 0.2,0
divnum dd 1000000.0,0

section '.text' code readable executable
start:
push 5
finit
fild dword[esp]
fiadd dword[val1]
fst st1
fmul st0,st1
fiadd dword[val2]
fmul st0,st0
fadd dword[decnum]
fadd dword[decnum2]
fdiv dword[divnum]
fnop    ;Mnemonic for "FPU no operation"
sub esp,4
fstp qword[esp]
push format_f
call [printf]
invoke ExitProcess,0

section '.idata' import data readable
library msvcrt,'msvcrt.dll',\
        kernel,'kernel32.dll'

import msvcrt,\
       printf,'printf'

import kernel,\
       ExitProcess,'ExitProcess'
