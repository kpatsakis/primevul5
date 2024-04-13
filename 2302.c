static SLJIT_INLINE PCRE2_SPTR compile_close_matchingpath(compiler_common *common, PCRE2_SPTR cc)
{
DEFINE_COMPILER;
int offset = GET2(cc, 1);
BOOL optimized_cbracket = common->optimized_cbracket[offset] != 0;

/* Data will be discarded anyway... */
if (common->currententry != NULL)
  return cc + 1 + IMM2_SIZE;

if (!optimized_cbracket)
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR_PRIV(offset));
offset <<= 1;
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), STR_PTR, 0);
if (!optimized_cbracket)
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0);
return cc + 1 + IMM2_SIZE;
}