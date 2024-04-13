static SLJIT_INLINE void free_stack(compiler_common *common, int size)
{
DEFINE_COMPILER;

SLJIT_ASSERT(size > 0);
OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, size * sizeof(sljit_sw));
}