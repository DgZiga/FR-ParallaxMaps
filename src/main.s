.gba
.thumb
.open "./BPRE0.gba","./out.gba", 0x08000000


.org 0x0805A636
.thumb
.align 2
LDR r0, =test|1
BX r0
.pool


.org freespace
.importobj "./build/linked.o"

test:
MOV r0, R4
MOV r1, R5
BL main

back_to_normal:
POP {R4,R5}
POP {R0}
BX r0

.pool

.close
