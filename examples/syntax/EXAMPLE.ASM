; EXAMPLE.ASM>*.HTML

include 'html.inc'

source.file equ 'DRAW.INC'
title.t equ 'Drawing Functions'

comment.color equ '#A0A0A0'
number.color equ '#5E00F7'
symbol.color equ '#2207FF'
string.color equ '#FF1777'

html.begin
html.code source.file
html.end