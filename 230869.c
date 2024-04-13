static void check_partial(compiler_common *common, BOOL force)
{
/* Checks whether a partial matching is occurred. Does not modify registers. */
DEFINE_COMPILER;
struct sljit_jump *jump = NULL;

SLJIT_ASSERT(!force || common->mode != PCRE2_JIT_COMPLETE);

if (common->mode == PCRE2_JIT_COMPLETE)
  return;

if (!force)
  jump = CMP(SLJIT_GREATER_EQUAL, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, STR_PTR, 0);
else if (common->mode == PCRE2_JIT_PARTIAL_SOFT)
  jump = CMP(SLJIT_EQUAL, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, SLJIT_IMM, -1);

if (common->mode == PCRE2_JIT_PARTIAL_SOFT)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->hit_start, SLJIT_IMM, 0);
else
  {
  if (common->partialmatchlabel != NULL)
    JUMPTO(SLJIT_JUMP, common->partialmatchlabel);
  else
    add_jump(compiler, &common->partialmatch, JUMP(SLJIT_JUMP));
  }

if (jump != NULL)
  JUMPHERE(jump);
}