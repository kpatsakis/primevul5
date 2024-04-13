static void check_str_end(compiler_common *common, jump_list **end_reached)
{
/* Does not affect registers. Usually used in a tight spot. */
DEFINE_COMPILER;
struct sljit_jump *jump;

if (common->mode == PCRE2_JIT_COMPLETE)
  {
  add_jump(compiler, end_reached, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));
  return;
  }

jump = CMP(SLJIT_LESS, STR_PTR, 0, STR_END, 0);
if (common->mode == PCRE2_JIT_PARTIAL_SOFT)
  {
  add_jump(compiler, end_reached, CMP(SLJIT_GREATER_EQUAL, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, STR_PTR, 0));
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->hit_start, SLJIT_IMM, 0);
  add_jump(compiler, end_reached, JUMP(SLJIT_JUMP));
  }
else
  {
  add_jump(compiler, end_reached, CMP(SLJIT_GREATER_EQUAL, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, STR_PTR, 0));
  if (common->partialmatchlabel != NULL)
    JUMPTO(SLJIT_JUMP, common->partialmatchlabel);
  else
    add_jump(compiler, &common->partialmatch, JUMP(SLJIT_JUMP));
  }
JUMPHERE(jump);
}