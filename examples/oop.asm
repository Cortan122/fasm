format PE console

entry start
include 'win32a.inc'

Point:
        dd 0
        dd 0
.size = $-Point

Initialize:
        push ebp
        mov ebp, esp
        sub esp, 40
        ;[ebp+12] = size of struct
        ;[ebp+8] = address of struct
        invoke malloc, dword [ebp+12]
        add esp, 4
        mov [ebp-4], eax ;new address
        mov esi, [ebp+8]
        mov edi, [ebp-4]
        mov ecx, [ebp+12]
        cld
        rep movsb
        mov eax, [ebp-4]
        mov esp,ebp
        pop ebp
        ret

MoveRight:
        push ebp
        mov ebp, esp
        ;[ebp+8] = this ptr
        mov eax, [ebp+8]
        inc dword [eax]
        mov esp, ebp
        pop ebp
        ret


msg: db '%d',10,0
start:
        push ebp
        mov ebp, esp
        sub esp, 40
        ;;;;;;;;;;;;;;;;;first obj
        push Point.size
        push Point
        call Initialize
        add esp, 4
        mov [ebp-4], eax;new object's address

        push eax
        call MoveRight
        add esp, 4
        ;;;;;;;;;;;;;;;;;;;; second obj
        push Point.size
        push Point
        call Initialize
        add esp, 4
        mov [ebp-8], eax;new object's address

        push eax
        call MoveRight
        add esp, 4
        push eax
        call MoveRight
        add esp, 4
        push eax
        call MoveRight
        add esp, 4
        ;;;;;;;;;;;;;;;;;;;;;;;printf to check obj's value
        mov eax, [ebp-4]
        invoke printf,msg,[eax]
        add esp, 8
        mov eax, [ebp-8]
        invoke printf,msg,[eax]
        add esp, 8
        invoke getch

        mov esp, ebp
        pop ebp
        invoke exit

data import
     library msvcrt, 'msvcrt'
     import msvcrt, printf,'printf',exit,'exit',getch,'_getch', malloc,'malloc',free,'free'
end data
