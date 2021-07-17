L1: .data 1,2,3
NOT: .extern W
data: .data 2
.define rd = 4
jmp rd
r3: mov L1,r2
STR: .string "abababa""
mov, r3,r2
TEST: PRN #5
.entry L1
jmp L1[2]
jmp L1, TEST
E3: .entry STR
mov STR[-2],r1
L4: .data 3,,4,6
1NT: data 4
eldi: .string "abcbcbc"
mov PSW,r3
SA: .define sz = 4
.define s3 =k
.define tel 4,5
stop
