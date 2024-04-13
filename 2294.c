static SLJIT_INLINE void match_script_run_common(compiler_common *common, int private_data_ptr, backtrack_common *parent)
{
DEFINE_COMPILER;

SLJIT_ASSERT(TMP1 == SLJIT_R0 && STR_PTR == SLJIT_R1);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
#ifdef SUPPORT_UNICODE
sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2(W, W, W), SLJIT_IMM,
  common->utf ? SLJIT_FUNC_ADDR(do_script_run_utf) : SLJIT_FUNC_ADDR(do_script_run));
#else
sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS2(W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(do_script_run));
#endif

OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_RETURN_REG, 0);
add_jump(compiler, parent->top != NULL ? &parent->top->nextbacktracks : &parent->topbacktracks, CMP(SLJIT_EQUAL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0));
}