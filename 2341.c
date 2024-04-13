static SLJIT_INLINE void count_match(compiler_common *common)
{
DEFINE_COMPILER;

OP2(SLJIT_SUB | SLJIT_SET_Z, COUNT_MATCH, 0, COUNT_MATCH, 0, SLJIT_IMM, 1);
add_jump(compiler, &common->calllimit, JUMP(SLJIT_ZERO));
}