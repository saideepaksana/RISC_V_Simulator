add x5, x6, x7
sub x31, x30, x29
sub t3, t5, t6
xor x5, x6, x7
or x5, x6, x7
or zero, s9, x18
L2: and x30, x29, x28
sll x9, x5, x6
srl s1, t1, t2
srl s3, a2, a5
sra x9, x5, x6
addi x5, x6, -45
andi x30, x5, -56
xori x7, x6, 3
ori t0, t1, -45
slli x9, x5, 5
srli s1, x5, 4
srai x7, x6, 9
lb x7, 6(x5)
lh t2, 8(x4)
lw x31, 12(x6)
L3: ld x29, 24(x7)
lbu x31, 2(x5)
lhu t1, 4(x7)
lwu x5, 20(t2)
sb x6, 0(x4)
sh t2, 2(x6)
L1: sw x7, 12(x5)
sd x6, 24(t6)
jalr x30, 8(t0)
lui x9, 0x12f36
jal x5, L1
beq x4, t2, L2
bne x5, x7, L3
blt t0, t6, L1
bge x5, x8, L2
bltu x6, x29, L1
bgeu t2, x30, L2