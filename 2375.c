static void process_partial_match(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_jump *jump;

/* Partial matching mode. */
if (common->mode == PCRE2_JIT_PARTIAL_SOFT)
  {
  jump = CMP(SLJIT_GREATER_EQUAL, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, STR_PTR, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->hit_start, SLJIT_IMM, 0);
  JUMPHERE(jump);
  }
else if (common->mode == PCRE2_JIT_PARTIAL_HARD)
  {
  if (common->partialmatchlabel != NULL)
    CMPTO(SLJIT_LESS, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, STR_PTR, 0, common->partialmatchlabel);
  else
    add_jump(compiler, &common->partialmatch, CMP(SLJIT_LESS, SLJIT_MEM1(SLJIT_SP), common->start_used_ptr, STR_PTR, 0));
  }
}