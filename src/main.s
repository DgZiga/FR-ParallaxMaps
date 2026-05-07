.gba
.thumb
.open "./BPRE0.gba","./out.gba", 0x08000000

//change scrolling amount
.org 0x0805A636
.thumb
.align 2
LDR r0, =changeScrollAmt|1
BX r0
.pool

//make it so coords8_add is actually coords16_add so that we can properly divide by 2
.org 0x0805AC7E
.thumb
.align 2
LDR r0, =addCoords16|1
BX r0
.pool
//reset coords16 on map update
.org 0x0805A5A8
.thumb
.align 2
LDR r0, =resetCoords16|1
BX r0
.pool

.org 0x0805AA78
.thumb
.align 2
MOV r1, r1
MOV r1, r1
MOV R1, R0
ADD R1, #0x40
MOV r1, r1
ADD R0, #0x42
MOV r1, r1
.byte 0x16
.byte 0x48
.pool

.org 0x0805AA16
.thumb
.align 2
MOV r1, r1
LDRH    R1, [R4,#2]
MOV r1, r1
MOV    R2, R0
ADD    R2, #0x40
LDRH    R1, [R4,#4]
MOV r1, r1
ADD    R0, #0x42
LDRH    R1, [R4,#6]
MOV r1, r1
.pool

.org freespace
.importobj "./build/linked.o"

changeScrollAmt:
MOV r0, R4
MOV r1, R5
BL main

changeScrollAmt_back_to_normal:
POP {R4,R5}
POP {R0}
BX r0

tilemap_camera_move_something equ 0x03000E90
nu_x equ 0x0300506C
free_ram equ 0x0202402C //using opponents pkmn as free ram

addCoords16:
LDR     R0, =tilemap_camera_move_something
MOV     R1, R7
LDR     R2, =free_ram

//taken straight from 0805A5D4 coords8_add
LDRH    R3, [R2]
ADD     R3, R3, R1


STRH    R3, [R2]

STRB    R3, [R0]
LDRH    R1, [R2,#2]
MOV     R2, R8
ADD     R1, R1, R2
STRB    R1, [R0,#1]

LDR     R2, =free_ram
STRH    R1, [R2,#2]
MOV     R2, R8

LDR     R1, =nu_x
LDRH    R0, [R1]
SUB     R0, R0, R7
STRH    R0, [R1]
LDR     R1, =0x0805AC90|1
BX      R1

resetCoords16:
LDR     R0, =tilemap_camera_move_something
MOV     R1, #0
STRB    R1, [R0,#2]
STRB    R1, [R0,#3]
STRB    R1, [R0]
STRB    R1, [R0,#1]
MOV     R1, #1
STRB    R1, [R0,#4]

LDR     R0, =free_ram
MOV     R1, #0
STR     R1, [R0]

BX LR
/*
POP     {R0}
BX      R0
*/


.pool

.close
