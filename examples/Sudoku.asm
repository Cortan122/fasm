include 'format/format.inc'
FORMAT PE64 CONSOLE 6.0 at $10000 on "NUL"
        SECTION '.flat' CODE READABLE WRITEABLE EXECUTABLE
        POSTPONE
                msvcrt_name db 'msvcrt',0
                _exit db 0,0,'exit',0
                __printf_p db 0,0,'_printf_p',0
                align 8
                DATA IMPORT
                        dd 0,0,0,RVA msvcrt_name,RVA msvcrt_table
                        dd 0,0,0,0,0
                        align 8
                        msvcrt_table:
                                exit dq RVA _exit
                                _printf_p dq RVA __printf_p
                                dq 0
                END DATA
        END POSTPONE
;###############################################################################



Puzzle000 db \
5,3,0,0,7,0,0,0,0,\
6,0,0,1,9,5,0,0,0,\
0,9,8,0,0,0,0,6,0,\
8,0,0,0,6,0,0,0,3,\
4,0,0,8,0,3,0,0,1,\
7,0,0,0,2,0,0,0,6,\
0,6,0,0,0,0,2,8,0,\
0,0,0,4,1,9,0,0,5,\
0,0,0,0,8,0,0,7,9

; Each location has a linearized form [0,80]. Additionally, a location belongs
; to three groups: row, column, square. The values here convert the linearized
; index into the member groups.
align 64 ; cache friendly
TriQuantize:
repeat 9, j:0
        repeat 9, i:0
                db -i ; delta to current row start
                db -j*9 ; delta to current column start
                ; delta to current square start
                db -((i mod 3) + 9*(j mod 3))
        end repeat
end repeat


_format db "Looking for solutions for:",13,10
_floormat db "%c %c %c %c %c %c %c %c %c",13,10,0

Sudoku__PrettyPrint:    ; nothing fancy, just replace zero with period
        push rsi
        mov rsi,rbp
        push rdi
        push rbp
        mov rbp,rsp
        sub rsp,10*8
        and rsp,-16

        mov edi,9
.LLL:   mov edx,1
        xor eax,eax
.LL:    mov qword[rsp+rdx*8],'0'
        lodsb
        test al,al
        jnz .num
        add qword[rsp+rdx*8],'.' - '0'
.num:   add qword[rsp+rdx*8],rax
        add edx,1
        cmp edx,10
        jc .LL

        mov rdx,[rsp+8]
        mov r8,[rsp+16]
        mov r9,[rsp+24]
        call [_printf_p]
        lea rcx,[_floormat]

        sub edi,1
        jnz .LLL

        mov rsp,rbp
        pop rbp
        pop rdi
        pop rsi
        retn


; RBP : global puzzle pointer
; RDI : pointer of current trial + 1
align 64 ; cache friendly
Validate_XYN:
        ; RSI : linearized index of current trial
        neg rbp
        lea rsi,[rbp+rdi-1]
        neg rbp

        ; check current row
        movsx rdx,byte[TriQuantize+rsi*2+rsi]
        repeat 9, i:0
                cmp [(rdi-1)+rdx+i],cl
                jz .false
        end repeat

        ; check current column
        movsx rdx,byte[TriQuantize+rsi*2+rsi+1]
        repeat 9, i:0
                cmp [(rdi-1)+rdx+9*i],cl
                jz .false
        end repeat

        ; check current square
        movsx rdx,byte[TriQuantize+rsi*2+rsi+2]
        repeat 3, j:0
        repeat 3, i:0
                cmp [(rdi-1)+rdx+9*j+i],cl
                jz .false
        end repeat
        end repeat
.false: retn


; Sudoku solver :
TryStart:
        mov rdi,rbp ; from start
.Try:   push rdi
        mov al,0
        neg rdi
        lea rcx,[81+rdi+rbp]
        neg rdi
        repnz scasb
        jnz .end                        ; no zeroes to fix
                mov ecx,9
        .A:     call Validate_XYN
                jz .B
                mov [rdi-1],cl          ; attempt
                call .Try               ; recurse
                movzx ecx,byte [rdi-1]  ; restore counter
                mov byte[rdi-1],0       ; clear attempt
        .B:     loop .A
        pop rdi
        retn

.end:   pop rdi
        lea rcx,[_floormat-1]
        call Sudoku__PrettyPrint
        retn
;###############################################################################
        ENTRY $
        pop rax ; this doesn't return
        lea rbp,[Puzzle000]
        lea rcx,[_format]
        call Sudoku__PrettyPrint
        call TryStart
        call [exit]
        int3
