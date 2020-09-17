format PE console
entry start
include 'win32a.inc'

section '.code' code readable executable
  start:
    push helloStr
    call [printf]
    push 0
    call [exit]

section '.data' data readable writable
  helloStr: db 'Hello World!',10,0 ; "Hello World!\n\0"

section '.idata' import code readable
  library msvcrt, 'msvcrt.dll'
  import msvcrt, printf, 'printf', exit, '_exit'

