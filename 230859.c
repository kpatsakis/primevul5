static SLJIT_INLINE PCRE2_SPTR compile_fail_accept_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;

PUSH_BACKTRACK(sizeof(backtrack_common), cc, NULL);

if (*cc == OP_FAIL)
  {
  add_jump(compiler, &backtrack->topbacktracks, JUMP(SLJIT_JUMP));
  return cc + 1;
  }

if (*cc == OP_ACCEPT && common->currententry == NULL && (common->re->overall_options & PCRE2_ENDANCHORED) != 0)
  add_jump(compiler, &common->reset_match, CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, STR_END, 0));

if (*cc == OP_ASSERT_ACCEPT || common->currententry != NULL || !common->might_be_empty)
  {
  /* No need to check notempty conditions. */
  if (common->accept_label == NULL)
    add_jump(compiler, &common->accept, JUMP(SLJIT_JUMP));
  else
    JUMPTO(SLJIT_JUMP, common->accept_label);
  return cc + 1;
  }

if (common->accept_label == NULL)
  add_jump(compiler, &common->accept, CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(0)));
else
  CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(0), common->accept_label);
OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
OP1(SLJIT_MOV_U32, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, options));
OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP2, 0, SLJIT_IMM, PCRE2_NOTEMPTY);
add_jump(compiler, &backtrack->topbacktracks, JUMP(SLJIT_NOT_ZERO));
OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP2, 0, SLJIT_IMM, PCRE2_NOTEMPTY_ATSTART);
if (common->accept_label == NULL)
  add_jump(compiler, &common->accept, JUMP(SLJIT_ZERO));
else
  JUMPTO(SLJIT_ZERO, common->accept_label);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, str));
if (common->accept_label == NULL)
  add_jump(compiler, &common->accept, CMP(SLJIT_NOT_EQUAL, TMP2, 0, STR_PTR, 0));
else
  CMPTO(SLJIT_NOT_EQUAL, TMP2, 0, STR_PTR, 0, common->accept_label);
add_jump(compiler, &backtrack->topbacktracks, JUMP(SLJIT_JUMP));
return cc + 1;
}