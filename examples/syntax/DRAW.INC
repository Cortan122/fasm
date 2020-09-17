; $$$$$$$$$$$$$$$$$$ MAGIC-ARM $$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ?????????????????? DRAW.INC ????????????????????

; fast portable ARM graphics for RGB16 (5.6.5)

; vga.xy x, y  ; &vga[xy(x,y)]
; color.at     ; vga[xy(x,y)]

; clear.screen

; draw.pixel x, y, c
; draw.line.h x, y, w, c
; draw.line.v x, y, h, c

; draw.box x, y, w, h, c
; draw.box.t x, y, w, h, c, a
; draw.outline x, y, w, h, c

; draw.scanline x, y, w, p
; draw.scanline.t x, y, w, p
; draw.scanline.v x, y, w, p, c

; draw.bitmap x, y, w, h, p
; draw.bitmap.t x, y, w, h, p
; draw.bitmap.a x, y, w, h, p, a

; draw.fade x, y, w, h, c1, c2
; draw.shade x, y, w, h, c1, c2
; draw.black.bar x, y, w, h

;;;;;;;;;;;;;;;;;;;; DRAWING ;;;;;;;;;;;;;;;;;;;;;

; address &vga[(y*(screen.w*2))+(x*2)]...

function vga.xy, x, y
  . a3=SCREEN.PITCH, a4=a2*a3
  . a4+(a1<<1), a2=@vga, a1=a2+a4
endf

; erase screen with color/a1...

function clear.screen, c
  alias vga=a3,\
   w=a2, c=a1
  . vga=@vga, w=SCREEN.N
  .repeat w
    . (u16) *vga++=c
  .endr
endf

;;;;;;;;;;;;;;;; DRAW PIXEL, LINE ;;;;;;;;;;;;;;;;

; draw pixel: a1-a3=x/y/c...

function draw.pixel, x, y, c
  alias vga=a1, c=v1
  . c=a3
  vga.xy
  . (u16) *vga=c
endf

; draw horizontal line: a1-a4=x/y/w/c...

function draw.line.h, x, y, w, c
  alias vga=a1,\
   c=v1, w=v2
  . c=a4, w=a3
  vga.xy
  .repeat w
    . (u16) *vga++=c
  .endr
endf

; draw vertical line: a1-a4=x/y/h/c...

function draw.line.v, x, y, h, c
  alias vga=a1,\
   h=v1, c=v2, pitch=v3
  . c=a4, h=a3
  vga.xy
  . pitch=SCREEN.PITCH
  .repeat h
    . (u16) *vga=c, vga+pitch
  .endr
endf

;;;;;;;;;;;;;;; DRAW BOX, OUTLINE ;;;;;;;;;;;;;;;;

; draw solid rectangle: a1-v1=x/y/w/h/c...

function draw.box
  alias vga=a1, c=v1,\
   sw=v2, w=v3, h=v4,\
   pitch=a2, wb=a3
  . w=a3, sw=w, h=a4    ; save w/h
  vga.xy
  . pitch=SCREEN.PITCH  ; screen+row
  . wb=w<<1             ; w in bytes
  .repeat h             ; h # times
    . w=sw              ; reset saved
    .repeat w           ; w # times
      . (u16) *vga++=c  ; copy color
    .endr
    . vga+pitch, vga-wb ; advance
  .endr
endf

macro draw.box x, y, w, h, c {
  if ~x eq
    . v1=c, a1=x, a2=y, a3=w, a4=h
    draw.box
  end if
}

; draw rectangle outline...

function draw.outline
  alias c=v1,\
   x=v2, y=v3, w=v4, h=v5
  . x=a1, y=a2, w=a3, h=a4
  vga.xy
  draw.line.h x, y, w, c   ; top: x, y, w
  draw.line.v x, y, h, c   ; left: x, y, h
  . a1=x+w, a1--           ; right: x+w-1
  draw.line.v \
   a1, y+1, h-1, c
  . a2=y+h, a2--           ; bottom: y+h-1
  draw.line.h x, a2, w, c
endf

macro draw.outline x, y, w, h, c {
  if ~x eq
    . a1=x, a2=y, a3=w, a4=h, v1=c
  end if
  draw.outline
}

; draw rectangle with transparency...
; a1-a4=x/y/w/h. v5=color, v6=alpha

function draw.box.t
  alias vga=a1, c=v1,\
   w=v3, h=v4, sw=v2,\
   cl=v5, a=v6,\
   pitch=a2,\
   wb=a3, sc=v7
  . w=a3, sw=w, h=a4    ; save w/h
  vga.xy
  . pitch=SCREEN.PITCH  ; screen+row
  . wb=w<<1             ; w in bytes
  .repeat h             ; h # times
    . w=sw              ; reset saved
    .repeat w           ; w # times
     . (u16) c=*vga     ; screen color
      pusha
      mix c, cl, a      ; combine
      . c=a1
      popa
      . (u16) *vga++=c  ; copy
    .endr
    . vga+pitch, vga-wb ; advance
  .endr
endf

macro draw.box.t x, y, w, h, c, a {
  . a1=x, a2=y, a3=w, a4=h, v5=c, v6=a
  draw.box.t
}

;;;;;;;;;;;;;;;;; DRAW SCANLINE ;;;;;;;;;;;;;;;;;;

; draw 16BPP scanline. a1-a4=x/y/w/pixels...

function draw.scanline, x, y, w, p
  alias vga=a1,\          ; aliases
   p=v1, w=v2
  . p=a4, w=a3            ; save w
  vga.xy                  ; &vga(x,y)
  .repeat w               ; w # times
    . (u16) *vga++=*p++   ; copy pixel
  .endr
endf

; draw with transparent color...

function draw.scanline.t, x, y, w, p
  alias vga=a1,\     ; aliases
   c=a2, key=v2,\
   w=v3, p=v1
  . w=a3, p=a4       ; save w/p
  vga.xy             ; &vga(x,y)
   . (u16) key=*p    ; color at 0x0
  .repeat w          ; w # times
    . (u16) c=*p++   ; get source pixel
    .if c<>key       ; opaque?
      . (u16) *vga=c ; copy to screen
    .end
    . vga+2          ; next
  .endr
endf

; draw 16BPP variant scanline.
; a1-v1=x/y/w/c/pixels...

function draw.scanline.v
  alias vga=a1,\       ; aliases
   p=v1, c=v2, s=v3,\
   cl=v4, x=v5, w=v6,\
   n=v7
  . w=a3, cl=a4        ; save w, color
  vga.xy               ; &vga(x,y)
  .repeat w            ; w # times
    . (u16) c=*p++     ; get source pixel
    .if c=0            ; black, 100% invisible
      go .next
    .end
    . x=WHITE
    .if c=x            ; white, 100% visible
      . c=cl
      go .draw
    .end
    . (u16) s=*vga     ; else mixed
    . n=c&11111b, n<<3 ; alpha intensity
    .if n>=252         ; invisible
      go .next         ; skip
    .end
    .if n<=4           ; pure
      . c=cl
      go .draw
    .end
    pusha
    mix s, cl, n       ; combine
    . c=r0             ; screen+color
    popa
    .draw:
    . (u16) *vga=c
    .next:
    . vga+2            ; next
  .endr
endf

macro draw.scanline.v x, y, w, c, p {
  . a1=x, a2=y, a3=w, a4=c, v1=p
  draw.scanline.v
}

;;;;;;;;;;;;;;;;;; DRAW BITMAP ;;;;;;;;;;;;;;;;;;;

; draw 15/16BPP bitmap. a1-a4=x/y/w/h.
; v1=pixels...

function draw.bitmap
  alias vga=a1,\         ; aliases
   image=v1,\
   w=v2, h=v3, sw=v4,\   ; sizes, saved w
   pitch=v5, iwb=v6
  . w=a3, h=a4, sw=w     ; save size
  . pitch=SCREEN.PITCH   ; screen+image
  . iwb=w<<1             ; w in bytes
  vga.xy                 ; &vga(x,y)
  .repeat h              ; h # times
    . w=sw               ; get saved w
    .repeat w            ; w # times
      . (u16) \
       *vga++=*image++   ; copy pixel
    .endr
    . vga+pitch, vga-iwb ; advance
  .endr
endf

macro draw.bitmap p, x, y, w, h {
  if ~p eq
    . v1=p, a1=x, a2=y, a3=w, a4=h
  end if
  draw.bitmap
}

; draw bitmap with transparency
; by upper left pixel color (0x0).
; a1-a4=x/y/w/h. v1=pixels...

function draw.bitmap.t
  alias \                 ; aliases
   vga=a1, image=v1,\
   w=v2, h=v3, sw=v4,\    ; sizes, saved w
   pitch=v5, iwb=v6,\
   key=v7, c=a2
  . w=a3, h=a4, sw=w      ; save size
  . pitch=SCREEN.PITCH    ; screen+image
  . iwb=w<<1              ; w in bytes
  . (u16) key=*image      ; transparent color
  vga.xy                  ; &vga(x,y)
  .repeat h               ; h # times
    . w=sw
    .repeat w             ; w # times
      . (u16) c=*image++
      .if c<>key          ; opaque?
        . (u16) *vga++=c  ; copy pixel
      .else
        . vga+2           ; next
      .end
    .endr
    . vga+pitch, vga-iwb  ; advance
  .endr
endf

macro draw.bitmap.t p, x, y, w, h {
  if ~p eq
   . v1=p, a1=x, a2=y, a3=w, a4=h
  end if
  draw.bitmap.t
}

; draw bitmap with alpha in v8

function draw.bitmap.a
  alias \                 ; aliases
   vga=a1, image=v1,\
   w=v2, h=v3, sw=v4,\    ; sizes, saved w
   pitch=v5, iwb=v6,\
   key=v7, a=v8, c=r12
  . w=a3, h=a4, sw=w      ; save size
  . pitch=SCREEN.PITCH    ; screen+image
  . iwb=w<<1              ; w in bytes
  . (u16) key=*image      ; transparent color
  vga.xy                  ; &vga(x,y)
  .repeat h               ; h # times
    . w=sw
    .repeat w             ; w # times
      . (u16) c=*image++
      .if c<>key          ; opaque?
        pusha
        . (u16) a2=*vga
        . a1=c, a3=a
        mix
        . c=a1
        popa
        . (u16) *vga++=c  ; copy pixel
      .else
        . vga+2           ; next
      .end
    .endr
    . vga+pitch, vga-iwb  ; advance
  .endr
endf

macro draw.bitmap.a p, x, y, w, h, a {
  if ~p eq
   . v1=p, a1=x, a2=y, a3=w, a4=h, v8=a
  end if
  draw.bitmap.a
}

;;;;;;;;;;;;;;;;; DRAW GRADIENTS ;;;;;;;;;;;;;;;;;

; draw gradual vertical fade. parameters:
; a1-a4=x/y/w/h. v5-v6=c1/c2

function draw.fade
  alias vga=a1,\
   r=v1, g=v2, b=v3,\
   rn=v4, gn=v5, bn=v6,\
   c1=v7, c2=v8, h=r12

  pusha                 ; save a1-a4=x/y/w/h
  . c1=v5, c2=v6, h=a4  ; save colors+h
  delta8 c1, c2, 11, h  ; get deltas...
  . rn=a1
  delta8 c1, c2, 5, h
  . gn=a1
  delta8 c1, c2, 0, h
  . bn=a1, a1=c1        ; extract and scale
  get.rgb               ; r/g/b components
  . r=a1<<8
  . g=a2<<8
  . b=a3<<8
  popa                  ; restore x/y/w/h
  push a3-a4            ; save w/h
  vga.xy                ; x/y no longer needed
  pop a3-a4

  .repeat h             ; h # times
    pusha
    get c1=rgb r>>>8,\  ; c=(c>>>8)
     g>>>8, b>>>8
    popa
    . v8=a3             ; draw lines...
    .repeat v8          ; w # times
      . (u16) *vga++=c1 ; *vga++=c
    .endr
    . a2=SCREEN.PITCH
    . a1-(a3<<1), a1+a2 ; vga-(w*2)+pitch
    . r+rn, g+gn, b+bn  ; r/g/b+deltas
  .endr                 ; 8.8 fixed points
endf

macro draw.fade x, y, w, h, c1, c2 {
  . a1=x, a2=y, a3=w, a4=h, v5=c1, v6=c2
  draw.fade
}

; draw with vertical center fade:
; from c1 to c2 then c2 to c1.
; a1-a4=x/y/w/h. v5-v6=c1/c2

function draw.shade
  alias \
   x=a1, y=a2,\
   w=a3, h=a4,\
   c1=v5, c2=v6,\
   tmp=v7
  . h>>>1           ; h/2
  pusha
  draw.fade \
   x, y, w, h,\
   c1, c2
  popa
  . y+h, tmp=c1     ; move down
  . c1=c2, c2=tmp   ; exchange colors
  draw.fade \
   x, y, w, h,\
   c1, c2
endf

macro draw.shade x, y, w, h, c1, c2 {
  . a1=x, a2=y, a3=w, a4=h, v5=c1, v6=c2
  draw.shade
}

function draw.black.bar, x, y, w, h
 alias \
   x=a1, y=a2,\
   w=a3, h=a4,\
   c1=v5, c2=v6
  . h>>>1                 ; h/2
  pusha
  draw.fade x, y, w, h,\
   GRAY, GRAY25
  popa
  pusha
  . y+h                   ; move down
  draw.fade x, y, w, h,\
   BLACK, GRAY25
  popa
  pusha
  . h<<1
  draw.outline \          ; outline
   x, y, w, h, GRAY
  popa
  pusha
  draw.line.h \           ; highlight
   x, y, w, GRAY.L
  popa
endf