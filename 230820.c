static void compile_ref_matchingpath(compiler_common *common, PCRE2_SPTR cc, jump_list **backtracks, BOOL withchecks, BOOL emptyfail)
{
DEFINE_COMPILER;
BOOL ref = (*cc == OP_REF || *cc == OP_REFI);
int offset = 0;
struct sljit_jump *jump = NULL;
struct sljit_jump *partial;
struct sljit_jump *nopartial;

if (ref)
  {
  offset = GET2(cc, 1) << 1;
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
  /* OVECTOR(1) contains the "string begin - 1" constant. */
  if (withchecks && !common->unset_backref)
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1)));
  }
else
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), 0);

#if defined SUPPORT_UNICODE
if (common->utf && *cc == OP_REFI)
  {
  SLJIT_ASSERT(TMP1 == SLJIT_R0 && STR_PTR == SLJIT_R1);
  if (ref)
    OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
  else
    OP1(SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw));

  if (withchecks)
    jump = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_R2, 0);
  /* No free saved registers so save data on stack. */

  OP1(SLJIT_MOV, SLJIT_R3, 0, STR_END, 0);
  sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_RET(SW) | SLJIT_ARG1(SW) | SLJIT_ARG2(SW) | SLJIT_ARG3(SW) | SLJIT_ARG4(SW), SLJIT_IMM, SLJIT_FUNC_OFFSET(do_utf_caselesscmp));
  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_RETURN_REG, 0);

  if (common->mode == PCRE2_JIT_COMPLETE)
    add_jump(compiler, backtracks, CMP(SLJIT_LESS_EQUAL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 1));
  else
    {
    OP2(SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_LESS, SLJIT_UNUSED, 0, SLJIT_RETURN_REG, 0, SLJIT_IMM, 1);

    add_jump(compiler, backtracks, JUMP(SLJIT_LESS));

    nopartial = JUMP(SLJIT_NOT_EQUAL);
    OP1(SLJIT_MOV, STR_PTR, 0, STR_END, 0);
    check_partial(common, FALSE);
    add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));
    JUMPHERE(nopartial);
    }
  }
else
#endif /* SUPPORT_UNICODE */
  {
  if (ref)
    OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1), TMP1, 0);
  else
    OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw), TMP1, 0);

  if (withchecks)
    jump = JUMP(SLJIT_ZERO);

  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP2, 0);
  partial = CMP(SLJIT_GREATER, STR_PTR, 0, STR_END, 0);
  if (common->mode == PCRE2_JIT_COMPLETE)
    add_jump(compiler, backtracks, partial);

  add_jump(compiler, *cc == OP_REF ? &common->casefulcmp : &common->caselesscmp, JUMP(SLJIT_FAST_CALL));
  add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, 0));

  if (common->mode != PCRE2_JIT_COMPLETE)
    {
    nopartial = JUMP(SLJIT_JUMP);
    JUMPHERE(partial);
    /* TMP2 -= STR_END - STR_PTR */
    OP2(SLJIT_SUB, TMP2, 0, TMP2, 0, STR_PTR, 0);
    OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, STR_END, 0);
    partial = CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, 0);
    OP1(SLJIT_MOV, STR_PTR, 0, STR_END, 0);
    add_jump(compiler, *cc == OP_REF ? &common->casefulcmp : &common->caselesscmp, JUMP(SLJIT_FAST_CALL));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, 0));
    JUMPHERE(partial);
    check_partial(common, FALSE);
    add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));
    JUMPHERE(nopartial);
    }
  }

if (jump != NULL)
  {
  if (emptyfail)
    add_jump(compiler, backtracks, jump);
  else
    JUMPHERE(jump);
  }
}