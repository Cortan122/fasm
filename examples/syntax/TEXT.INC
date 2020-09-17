; $$$$$$$$$$$$$$$$$$ MAGIC-ARM $$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? TEXT.INC ????????????????????

; fast portable ARM text operations

; text.n t          ; get # characters (size-1)
; text.write a, b   ; copy with no 0 after
; text.copy a, b    ; standard copy with 0 after
; text.copy.n ...   ; copy with maximum size
; text.attach a, b  ; attach b to a; "concencate"
; text.attach.c...  ; attach character
; text.compare a, b ; compare. return <0>
; text.find t, c    ; search for c. return &/0
; text.find.last... ; search for c reverse
; text.count.c t, c ; count occurrances of c
; text.count.n t    ; count # lines
; text.begins a, b  ; begins with b?
; text.ends a, b    ; ends with b?
; text.upper t      ; convert to uppercase
; text.lower t      ; convert to lowercase
; text.reverse t    ; reverse
; text.expand t, n  ; shift right
; text.prefix a, b  ; prepend
; text.enclose.c... ; enclose in 'c'
; text.align t...   ; align to size: '007F'

; text.array.equal ta, t, n
; text.array.begins ta, t, n

; text.skip.while t, type
; text.skip.until t, type
; text.copy.while a, b, type
; text.copy.until a, b, type

; set.source r
; set.token r
; set.stream s, t

; skip.while type
; skip.until type
; copy.while type
; copy.until type

; skip.space
; skip.white
; skip.comment
; skip.all

; i2t n, t         ; number/text conversions
; u2t n, t
; h2t n, t
; b2t n, t

; t2i t
; t2u t
; t2h t
; t2b t

;;;;;;;;;;;;;;; CHARACTER TABLES ;;;;;;;;;;;;;;;;;

; ILT - insensitive lookup table. A-Z/a-z are
; the same. increases processing speed by
; many times. example: if (tt[a]=tt[b]) instead
; of: if ((a>='a'&a<='z')&(b>='a'&b<='z')) |
; ((a>='A'&a<='Z')&(b>='Z'&b<='Z'))

; TLT - type lookup table. each byte contains
; C.X BITs to determine its type fast in one
; comparison: if tt[c]&SYMBOL

macro define.xlt {

align 4

ILT db \
 00h,01h,02h,03h,04h,05h,06h,07h,\
 08h,09h,0Ah,0Bh,0Ch,0Dh,0Eh,0Fh,\
 10h,11h,12h,13h,14h,15h,16h,17h,\
 18h,19h,1Ah,1Bh,1Ch,1Dh,1Eh,1Fh,\
 20h,21h,22h,23h,24h,25h,26h,27h,\
 28h,29h,2Ah,2Bh,2Ch,2Dh,2Eh,2Fh,\
 30h,31h,32h,33h,34h,35h,36h,37h,\
 38h,39h,3Ah,3Bh,3Ch,3Dh,3Eh,3Fh,\
 40h,41h,42h,43h,44h,45h,46h,47h,\
 48h,49h,4Ah,4Bh,4Ch,4Dh,4Eh,4Fh,\
 50h,51h,52h,53h,54h,55h,56h,57h,\
 58h,59h,5Ah,5Bh,5Ch,5Dh,5Eh,5Fh,\
 60h,41h,42h,43h,44h,45h,46h,47h,\
 48h,49h,4Ah,4Bh,4Ch,4Dh,4Eh,4Fh,\
 50h,51h,52h,53h,54h,55h,56h,57h,\
 58h,59h,5Ah,7Bh,7Ch,7Dh,7Eh,7Fh

TLT db \
 00h,80h,80h,80h,80h,80h,80h,80h,\
 80h,80h,40h,80h,80h,40h,80h,80h,\
 80h,80h,80h,80h,80h,80h,80h,80h,\
 80h,80h,80h,80h,80h,80h,80h,80h,\
 20h,10h,04h,04h,10h,04h,04h,04h,\
 04h,04h,04h,04h,04h,04h,10h,04h,\
 01h,01h,01h,01h,01h,01h,01h,01h,\
 01h,01h,04h,04h,04h,04h,04h,10h,\
 10h,0Ah,0Ah,0Ah,0Ah,0Ah,0Ah,02h,\
 0Ah,02h,02h,0Ah,02h,0Ah,02h,02h,\
 02h,02h,02h,02h,02h,02h,02h,02h,\
 02h,02h,02h,04h,04h,04h,04h,10h,\
 04h,0Ah,0Ah,0Ah,0Ah,0Ah,0Ah,02h,\
 0Ah,02h,02h,0Ah,02h,0Ah,02h,02h,\
 02h,02h,02h,02h,02h,02h,02h,02h,\
 02h,02h,02h,04h,04h,04h,04h,80h
}

define.xlt

;            76543210b
C.NULL     = 00000000b ; 0
C.NUMBER   = 00000001b ; 0-9
C.ALPHA    = 00000010b ; A-Z, a-z
C.SYMBOL   = 00000100b ; all symbols except _.?!@$
C.NUMERIC  = 00001000b ; A-F/a-f, h,b,k,m/H,B,K,M
C.SYMBOLIC = 00010000b ; _.?!@$
C.SPACE    = 00100000b ; ' ', '/t'
C.RETURN   = 01000000b ; 0Dh, 0Ah
C.IGNORE   = 10000000b ; extended: 1.XXXXXXXb
C.KEYWORD  = 11111111b

C.DIGIT    = C.NUMBER or C.NUMERIC
C.NAME     = C.ALPHA or C.NUMBER or C.SYMBOLIC
C.SYMBOLS  = C.SYMBOL or C.SYMBOLIC
C.ALPHAN   = C.ALPHA or C.NUMBER
C.VISIBLE  = C.ALPHAN or C.SYMBOLS
C.WHITE    = C.SPACE or C.RETURN
C.BLANK    = C.WHITE or C.IGNORE
C.END      = C.SYMBOL or C.WHITE
C.0        = 0

; is c of type? a1=c, a2=type

function is.c, c, type
  . a1&0FFh, a3=TLT, a3+a1
  . (u8) a1=*a3, a1&a2
endf

macro is.c c, t {
  . a1=c, a2=t
  is.c
}

macro if.is c, t {
  is.c c, C.#t
  .if true
}

numeric ZERO.C='0', SPACE.C=' ',\
 COMMA.C=',', COLIN.C=':', SEMI.C=';',\
 POUND.C='#', ASSIGN.C='=', SLASHB.C='\'

NL equ ,0Dh,0Dh,

macro .if.text.equal a, b
 { !if text.equal, a, b }
macro .if.not.text.equal a, b
 { !if.n text.equal, a, b }
macro .if.text.find a, b
 { !if text.find, a, b }
macro .if.text.find.last a, b
 { !if text.find.last, a, b }
macro .if.text.begins a, b
 { !if text.begins, a, b }
macro .if.text.ends a, b
 { !if text.ends, a, b }

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; text.end(t) - advance a1 to end (*t=0)

function text.end, t
  alias t=a1, c=v1
  . c=1
  .while c           ; until 0
    . (u8) c=*t++    ; read c
  .endw
  . t--
endf

; text.n(t) - get text length, # characters

function text.n, t
  alias t=a1, b=a2, c=v1
  . c=1, b=t
  .while c           ; until 0
    . (u8) c=*t++    ; read c
  .endw
  . t--, t-b         ; return current-start
endf

; text.write(a, b) - copy with no 0 after

function text.write, a, b
  alias a=a1, b=a2, c=v1
  .forever
    . (u8) c=*b++    ; read
    .if c=0
      return
    .end
    . (u8) *a++=c    ; copy
  .endfv
endf

; text.copy(a, b) - standard copy with 0 after

function text.copy, a, b
  alias a=a1, b=a2, c=v1
  . c=1
  .while c
    . (u8) c=*b++    ; read
    . (u8) *a++=c    ; copy
  .endw
  . a--              ; return end
endf

; text.copy.n(a, b, n) - copy with maximum
; size specified

function text.copy.n, a, b, n
  alias a=a1, b=a2,\
   n=a3, c=v1
  .repeat n          ; # times
    . (u8) c=*b++    ; read
    . (u8) *a++=c    ; copy
    .if c=0          ; end?
      .break
    .end
  .endr
  . a--
endf

; text.attach(a, b) - attach b to a

function text.attach, a, b
  alias a=a1,\
   b=a2, p=v1
  . p=b
  text.end
  . b=p
  text.copy
endf

; text.attach.c(t, c) - attach c to t

function text.attach.c, t, c
  alias t=a1, c=v2
  . c=a2
  text.end
  . (u8) *t++=c, (u8) *t=0
endf

; text.compare(a, b) - lexical comparison.
; return <0>

function text.compare, a, b
  alias a=a1, b=a2,\
   c=v1, d=v2
   . c=1, d=c
  .while c=d         ; while equal
    . (u8) c=*a++
    . (u8) d=*b++
    .if c=0          ; and both nonzero
      .break
    .end
    .if d=0
      .break
    .end
  .endw
  . a=c-d            ; return *a-*b
endf

; text.equal(a, b) - equal? return 1/0

function text.equal, a, b
  text.compare
  .if false
    return 1
  .end
endf 0

; text.find(t, c) - search for character.
; return address or 0

function text.find, t, c
  alias t=a1,\
   c=v1, key=a2
  .forever           ; loop
    . (u8) c=*t      ; read c
    .if c=0          ; end?
      return 0       ; not found
    .end
    .if c=key        ; found?
      return t       ; t=address
    .end
    . t++            ; next
  .endfv
endf

; text.find.last(t, c) - search for c reverse

function text.find.last, t, c
  alias t=a1, c=a2,\
   key=v1, p=v2
  . key=c, p=t       ; save begin
  . c=1
  .while c           ; advance to end-1,
    . (u8) c=*t++    ; last character
  .endw
  . t-2
  .while t>p         ; loop backwards
    . (u8) c=*t      ; until beginning
    .if c=key        ; found
      return t       ; t=address
    .end
    . t--            ; previous
  .endw
endf 0               ; not found

; text.count.c(t, c) - count # of
; characters in text

function text.count.c, t, c
  alias p=v1, c=v2, n=v3
  . p=a1, c=a2, n=0
  .while true
    get p=text.find p, c
    .if true
      . p++, n++
    .end
  .endw
endf n

; text.count.n(t) - count # lines

function text.count.n, t
  text.count.c a1, 0Dh
  .if a1>0
    . a1++
  .end
endf

; text.begins(a, b) - a begins with b?

function text.begins, a, b
  alias a=a1, b=a2,\
   c=v1, d=v2, e=v3
  . c=1, d=c
  .while c=d         ; while equal
    . (u8) c=*a++    ; load *a/*b
    . (u8) d=*b++
    . e=c|d          ; and both nonzero
    .breakz          ; break if either=0
  .endw
  .if d<>0           ; *b must=0
    return 0
  .end
endf

; text.ends(a, b) - a ends with b?

function text.ends, a, b
  alias \
   a=v1, b=v2, p=v3
  . a=a1, b=a2
  text.end            ; a=end(a)-length(b)
  . p=a1, a1=b
  text.n
  . p-a1, a1=p, a2=b
  text.compare
  .if false
    return 1
  .end
endf 0

; text.upper(t) - convert to uppercase

function text.upper, t
  alias t=a1, c=v1
  .forever
    . (u8) c=*t      ; get c
    .if c=0          ; end?
      return t
    .end
    .if c>=97        ; lowercase?
      .if c<=122
        . c-32
        . (u8) *t=c  ; copy c
      .end
    .end
    . t++            ; next c
  .endfv
endf

; text.lower(t) - convert to lowercase

function text.lower, t
  alias t=a1, c=v1
  .forever
    . (u8) c=*t      ; get c
    .if c=0          ; end?
      return
    .end
    .if c>=65        ; uppercase?
      .if c<=90
        . c+32
        . (u8) *t=c  ; copy c
      .end
    .end
    . t++            ; next c
  .endfv
endf

; text.reverse(t) - reverse text

function text.reverse, t
  alias t=a1,\
   p=a2, s=a3,\
   c=v1, d=v2
  . s=t, c=1         ; save start
  .while c
    . (u8) c=*t++    ; advance to end
  .endw
  . p=t-2, t=s
  .while t<p         ; exchange *t++/*p--
    . (u8) c=*t
    . (u8) d=*p
    . (u8) *t++=d
    . (u8) *p=c
    . p--
  .endw
  . t--
endf

; expand; shift all characters right.
; example: 'abc123' becomes 'XXXabc123'
; after expand 3 where X is unknown

function text.expand, t, n
  alias t=v1, p=v2,\
   n=v3, tn=v4, a=v5, b=v6
  . t=a1, n=a2
  get tn=text.n t
  . a=t+tn, a--, b=a, a+n
  .repeat tn
    . (u8) *a=*b, a--, b--
  .endr
endf

; insert text at beginning

function text.prefix, a, b
  alias a=v1, b=v2, n=v3
  . a=a1, b=a2
  get n=text.n b
  text.expand a, n
  text.write a, b
endf

; enclose t in b/egin and e/nd characters.
; example: text.enclose t, 28h, 29h
; '(', ')'

function text.enclose.c, t, b, e
  alias t=v1, b=v2, e=v3,\
   p=v4, n=v5, c=v6
  . t=a1, b=a2, e=a3
  get n=text.n t
  text.expand t, 1
  . p=t, (u8) *p=b, p=t+n, p++
  . (u8) *p++=e, (u8) *p=0
endf

; another way:

; text.copy buffer, b
; text.attach buffer, a
; text.copy a, buffer

; prefix text with c's ('0', ' ', etc)
; or ensure maximum n. example:
; before: text t='7FAB'
; text.align t, 30h, 8
; after: t='00007FAB', aligned to hex32

function text.align, t, c, n
  alias t=v1, c=v2,\
   n=v4, p=v5, tn=v6
  . t=a1, c=a2, n=a3
  get tn=text.n t
  .if n=tn            ; same size
    return            ; do nothing
  .end
  .if tn>n            ; exceeds maximum
    . t+n, (u8) *t=0  ; end at t+n
    return
  .end
  . n-tn              ; expand t
  text.expand t, n
  .repeat n
    . (u8) *t++=c
  .endr
  . t+tn, (u8) *t=0   ; terminate
endf

; search text array ta for t using text.equal.
; return index or -1 (<0) if not found. ta is
; an array of text addresses (texts)

function text.array.equal, ta, t, n
  alias ta=v1, t=v2,\
   n=v3, i=v4, p=v5
  . ta=a1, t=a2, n=a3
  .loop i=0 to n
    . p=ta[i]
    .if.text.equal t, p ; p, t
      return i
    .end
  .endl
endf -1

macro .if.text.array.equal ta, t {
  text.array.equal ta, t, ta#.$
  .if r0<>-1
}

function text.array.begins, ta, t, n
  alias ta=v1, t=v2,\
   n=v3, i=v4, p=v5
  . ta=a1, t=a2, n=a3
  .loop i=0 to n
    . p=ta[i]
    .if.text.begins p, t
      return i
    .end
  .endl
endf -1

macro .if.text.array.begins ta, t {
  text.array.begins ta, t, ta#.$
  .if r0<>-1
}

; 2-DO: insensitive compare and search

;;;;;;;;;;;;;;;;;; CONVERSIONS ;;;;;;;;;;;;;;;;;;;

; x2t(n, t) ; number to text

; convert unsigned 32BIT integer to text

function u2t, n, t
  alias n=a1,\
   x=a2, y=a3,\
   t=v1, s=v2, c=v3
  . t=a2, s=a2
  .if n=0             ; zero?
    . (u8) *t++=30h
    . (u8) *t=0
    return t
  .end
  .while n            ; until 0
    . y=n             ; dividend
    . x=1999999Ah     ; ((2^32)/10)+1
    . n=n-(n>>>30)    ; n=n-(n>>>30)
    umull c, n, n, x  ; n*reciprocal
    . x=n<<1         
    . x=x+(x<<2)
    . x=y-x           ; remainder
    . c=x+30h         ; c=(n%10)+'0'
    . (u8) *t++=c     ; *t++=c
  .endw
  . (u8) *t=0
  text.reverse s
endf t

; convert signed 32BIT integer to text

function i2t, n, t
  alias n=a1,\
   t=a2, sign=a3
  . sign=80000000h
  .if n&sign          ; negate?
    . -n
    . (u8) *t++=2Dh   ; prepend '-'
  .end
  u2t                 ; convert
endf

; convert 32BIT hexadecimal number to text

function h2t, n, t
  alias t=v1, s=v2,\
   n=v3, x=v4, hex=v5
  . n=a1, t=a2, s=t
  . hex=_hex
  .if n=0             ; zero?
    . (u8) *t++=30h
    . (u8) *t=0
    return t
  .end
  .while n            ; *t++=*(hex+(n&(16-1)))
    . x=n&15
    ldrb x, [hex, x]  ; (u8) x=hex[x]
    . (u8) *t++=x
    . n>>>4           ; n/16
  .endw
  . (u8) *t=0
  text.reverse s
  return t
  _hex: db \
   '0123456789ABCDEF' ; 16 bytes
endf t

; convert 32BIT binary number to text

function b2t, n, t
  alias t=v1, s=v2,\
   n=v3, x=v4
  . n=a1, t=a2, s=t
  .if n=0             ; zero?
    . (u8) *t++=30h
    . (u8) *t=0
    return t
  .end
  .while n            ; *t++=(n&1)+'0'
    . x=n&1, x+30h
    . (u8) *t++=x
    . n>>>1           ; n/2
  .endw
  . (u8) *t=0
  text.reverse s
endf t

; t2x(t) ; text to number

; convert text to unsigned 32BIT integer

function t2u, t
  alias t=v1,\
   c=v2, n=v3
  . t=a1, (u8) c=*t
  .if c=30h           ; skip preceding
    .while c=30h      ; '0's...
      . (u8) c=*t++
    .endw
    . t--
  .end
  .if c=0             ; 0 value?
    return 0
  .end
  . n=0
  .forever
    . (u8) c=*t++
    .if c=0
      return n
    .end
    . n+(n<<2), n+n   ; n=n*10+*t++-'0'
    . n-30h, n+c
  .endfv
endf

; convert text to signed 32BIT integer

function t2i, t
  alias t=v1,\
   c=v2, negate=v3
  . t=a1, (u8) c=*t
  . negate=0
  .if c='-'           ; negative?
    . t++, negate=1   ; skip
  .end
  t2u t
  .if negate
    . -a1
  .end
endf

; convert hexadecimal text to number

function t2h, t       ; text to hexadecimal
  alias t=v1,\
   c=v2, n=v3
  . t=a1, (u8) c=*t
  .if c=30h           ; skip preceding
    .while c=30h      ; '0's...
      . (u8) c=*t++
    .endw
    . t--
  .end
  .if c=0             ; 0 value?
    return 0
  .end
  . n=0
  .forever
    . (u8) c=*t++
    .if c=0
      return n
    .end
    . n<<4            ; n=n*16+c2h(*t++)
    .if c<=39h        ; 0-9
      . c-30h
    .else.if c>=97    ; a-f
      . c-57h
    .else             ; A-F
      . c-37h
    .end
    . n+c
  .endfv
endf

; convert binary text to number

function t2b, t       ; text to binary
  alias t=v1,\
   c=v2, n=v3
  . t=a1, (u8) c=*t
  .if c=30h           ; skip preceding
    .while c=30h      ; '0's...
      . (u8) c=*t++
    .endw
    . t--
  .end
  .if c=0             ; 0 value?
    return 0
  .end
  . n=0
  .forever
    . (u8) c=*t++
    .if c=0
      return n
    .end
    . n<<1, n+c       ; n=n*2+*t++-'0'
    . n-30h
  .endfv
endf

;;;;;;;;;;;;;;;;;;;;; PRINT ;;;;;;;;;;;;;;;;;;;;;;

; 2-DO: print formatted text to buffer

;;;;;;;;;;;;;;;;;;; PARSE TEXT ;;;;;;;;;;;;;;;;;;;

; skip while type and not 0

; get p=text.skip.while p, C.WHITE

function text.skip.while, t, type
  alias t=a1, type=a2,\
   c=a3, p=a4, n=v1
  .forever
    . (u8) c=*t
    .if c=0
      return 0
    .end
    .if c=0Ah
      . p=line.n, (u32) n=*p
      . n++, (u32) *p=n
    .end
    . p=TLT, p+c
    . (u8) c=*p, c&type
    .breakz
    . t++
  .endfv
  . (u8) c=*t
  .if c=0
    return 0
  .end
endf

; skip until type and while not 0

; get p=text.skip.until p, C.RETURN

function text.skip.until, t, type
  alias t=a1, type=a2,\
   c=a3, p=a4, n=v1
  .forever
    . (u8) c=*t
    .if c=0
      return 0
    .end
    .if c=0Ah
      . p=line.n, (u32) n=*p
      . n++, (u32) *p=n
    .end
    . p=TLT, p+c
    . (u8) c=*p, c&type
    .breakn
    . t++
  .endfv
  . (u8) c=*t
  .if c=0
    return 0
  .end
endf

; copy while type and not 0

; get s=text.copy.while t, s, C.NAME

function text.copy.while, a, b, type
  alias a=a1, b=a2,\
   type=a3, c=a4, p=v1, n=v2
  .forever
    . (u8) c=*b
    .if c=0
      go .e
    .end
    .if c=0Ah
      . p=line.n, (u32) n=*p
      . n++, (u32) *p=n
    .end
    . p=TLT, p+c
    . (u8) c=*p, c&type
    .breakz
    . (u8) *a++=*b++
  .endfv
  .e:
  . (u8) *a=0, (u8) c=*b
  .if c=0
    return 0
  .end
endf b

; copy until type and while not 0

; get s=text.copy.until t, s, C.END

function text.copy.until, a, b, type
  alias a=a1, b=a2,\
   type=a3, c=a4, p=v1, n=v2
  .forever
    . (u8) c=*b
    .if c=0
      return 0
    .end
    .if c=0Ah
      . p=line.n, (u32) n=*p
      . n++, (u32) *p=n
    .end
    . p=TLT, p+c
    . (u8) c=*p, c&type
    .breakn
    . (u8) *a++=*b++
  .endfv
  . (u8) *a=0, (u8) c=*b
  .if c=0
    return 0
  .end
endf b

;;;;;;;;;;;;;;;;;; SOURCE, TOKEN ;;;;;;;;;;;;;;;;;

; safer, easier global skip/copy while/until

macro define.source {
 align 4
 void source.p, destiny.p, token.p
 integer token.type, token.value,\
  token.size, line.n, n.lines
}

macro set.source r
 { . r12=source.p, r11=r, (u32) *r12=r11 }

macro set.token r
 { . r12=token.p, r11=r, (u32) *r12=r11 }

macro set.stream s, t {
  set.source s
  set.token t
}

macro get.source s, t {
  . s=source.p, (u32) s=*s
  if ~t eq
    . t=token.p, (u32) t=*t
  end if
}

; 2-DO...

get.destiny fix get.console
set.destiny fix set.console
save.destiny fix save.console

macro get.token.p t
 { . t=token.p, (u32) t=*t }

macro set.token.x x, v
 {  . r12=token#x, r11=v, (u32) *r12=r11 }

macro set.token.type v { set.token.x .type, v }
macro set.token.class v { set.token.x .class, v }
macro set.token.value v { set.token.x .value, v }

macro get.token.value x
{ . x=token.value, (u32) x=*x }

; skip/copy while/until type...

function skip.while, type
  alias p=v1, type=v2
  . type=a1
  get.source p
  try p=text.skip.while p, type
  set.source p
endf p

function skip.until, type
  alias p=v1, type=v2
  . type=a1
  get.source p
  try p=text.skip.until p, type
  set.source p
endf p

function copy.while, type
  alias p=v1, t=v2, type=v3
  . type=a1
  get.source p, t
  try p=text.copy.while t, p, type
  set.source p
endf p

function copy.until, type
  alias p=v1, t=v2, type=v3
  . type=a1
  get.source p, t
  try p=text.copy.until t, p, type
  set.source p
endf p

macro skip.space   { skip.while C.SPACE }
macro skip.white   { skip.while C.WHITE }
macro skip.comment { skip.until C.RETURN }

function skip.all
  alias s=v1, c=v2
  .white:
  try skip.white
  get.source s
  . (u8) c=*s
  .if c=0
    return 0
  .end
  .if c=SEMI.C
    try skip.comment
    go .white
  .end
endf 1

; skip spaces then character. return 0
; if next c not key

function skip.c, key
  alias p=v1,\
   c=v2, key=v3
  . key=a1
  skip.space
  get.source p
  . (u8) c=*p
  .if c<>key
    return 0
  .end
  . p++
  set.source p
endf p

; ideas for parsing grammar:

; syntax: parse 'c' ; c=command/s

; '='      ; copy
; '>'      ; skip forward
; '<'      ; skip reverse
; '>1'     ; move right # times
; '<1'     ; move left # times
; '>ws'    ; skip while space/tab
; '>ur'    ; skip until return
; '>w_'    ; skip all whitespace
; '>w;'    ; skip all+comments
; '=u.'    ; copy until end/delimiter
; '=w#'    ; while digit
; '=w@'    ; while name/identifier
; '>w_=u.' ; skip whitespace then copy

; '=w(a-z|A-Z|0-9|_|.|?)'