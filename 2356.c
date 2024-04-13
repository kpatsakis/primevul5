static SLJIT_INLINE int match_capture_common(compiler_common *common, int stacksize, int offset, int private_data_ptr)
{
DEFINE_COMPILER;

if (common->capture_last_ptr != 0)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr, SLJIT_IMM, offset >> 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), TMP1, 0);
  stacksize++;
  }
if (common->optimized_cbracket[offset >> 1] == 0)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize), TMP1, 0);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), private_data_ptr);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(stacksize + 1), TMP2, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), STR_PTR, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
  stacksize += 2;
  }
return stacksize;
}