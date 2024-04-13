static SLJIT_INLINE void allocate_stack(compiler_common *common, int size)
{
/* May destroy all locals and registers except TMP2. */
DEFINE_COMPILER;

SLJIT_ASSERT(size > 0);
OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, size * sizeof(sljit_sw));
#ifdef DESTROY_REGISTERS
OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, 12345);
OP1(SLJIT_MOV, TMP3, 0, TMP1, 0);
OP1(SLJIT_MOV, RETURN_ADDR, 0, TMP1, 0);
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS0, TMP1, 0);
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS1, TMP1, 0);
#endif
add_stub(common, CMP(SLJIT_LESS, STACK_TOP, 0, STACK_LIMIT, 0));
}