; EXAMPLE2.ASM>*.HTML

include 'html.inc'

source1.file equ 'TEXT.INC'
source2.file equ 'DRAW.INC'

title.t equ 'Functions'
subject1.t equ 'Text Functions'
subject2.t equ 'Drawing Functions'

comment.color equ '#A0A0A0'
number.color equ '#5E00F7'
symbol.color equ '#2207FF'
string.color equ '#FF1777'

html.begin
html.subject subject1.t, left
html.code source1.file
html.subject subject2.t, left
html.code source2.file
html.end