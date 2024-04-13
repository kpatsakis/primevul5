static void compile_ref_matchingpath(compiler_common *common, PCRE2_SPTR cc, jump_list **backtracks, BOOL withchecks, BOOL emptyfail)
{
DEFINE_COMPILER;
BOOL ref = (*cc == OP_REF || *cc == OP_REFI);
int offset = 0;
struct sljit_jump *jump = NULL;
struct sljit_jump *partial;
struct sljit_jump *nopartial;
#if defined SUPPORT_UNICODE
struct sljit_label *loop;
struct sljit_label *caseless_loop;
jump_list *no_match = NULL;
int source_reg = COUNT_MATCH;
int source_end_reg = ARGUMENTS;
int char1_reg = STACK_LIMIT;
#endif /* SUPPORT_UNICODE */

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
  SLJIT_ASSERT(common->iref_ptr != 0);

  if (ref)
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
  else
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw));

  if (withchecks && emptyfail)
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, TMP2, 0));

  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->iref_ptr, source_reg, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw), source_end_reg, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw) * 2, char1_reg, 0);

  OP1(SLJIT_MOV, source_reg, 0, TMP1, 0);
  OP1(SLJIT_MOV, source_end_reg, 0, TMP2, 0);

  loop = LABEL();
  jump = CMP(SLJIT_GREATER_EQUAL, source_reg, 0, source_end_reg, 0);
  partial = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);

  /* Read original character. It must be a valid UTF character. */
  OP1(SLJIT_MOV, TMP3, 0, STR_PTR, 0);
  OP1(SLJIT_MOV, STR_PTR, 0, source_reg, 0);

  read_char(common, 0, READ_CHAR_MAX, NULL, READ_CHAR_UPDATE_STR_PTR | READ_CHAR_VALID_UTF);

  OP1(SLJIT_MOV, source_reg, 0, STR_PTR, 0);
  OP1(SLJIT_MOV, STR_PTR, 0, TMP3, 0);
  OP1(SLJIT_MOV, char1_reg, 0, TMP1, 0);

  /* Read second character. */
  read_char(common, 0, READ_CHAR_MAX, &no_match, READ_CHAR_UPDATE_STR_PTR);

  CMPTO(SLJIT_EQUAL, TMP1, 0, char1_reg, 0, loop);

  OP1(SLJIT_MOV, TMP3, 0, TMP1, 0);

  add_jump(compiler, &common->getucd, JUMP(SLJIT_FAST_CALL));

  OP2(SLJIT_SHL, TMP1, 0, TMP2, 0, SLJIT_IMM, 2);
  OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 3);
  OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, TMP1, 0);

  OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, (sljit_sw)PRIV(ucd_records));

  OP1(SLJIT_MOV_S32, TMP1, 0, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(ucd_record, other_case));
  OP1(SLJIT_MOV_U8, TMP2, 0, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(ucd_record, caseset));
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP3, 0);
  CMPTO(SLJIT_EQUAL, TMP1, 0, char1_reg, 0, loop);

  add_jump(compiler, &no_match, CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, 0));
  OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 2);
  OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, (sljit_sw)PRIV(ucd_caseless_sets));

  caseless_loop = LABEL();
  OP1(SLJIT_MOV_U32, TMP1, 0, SLJIT_MEM1(TMP2), 0);
  OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, SLJIT_IMM, sizeof(uint32_t));
  OP2U(SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_LESS, TMP1, 0, char1_reg, 0);
  JUMPTO(SLJIT_EQUAL, loop);
  JUMPTO(SLJIT_LESS, caseless_loop);

  set_jumps(no_match, LABEL());
  if (common->mode == PCRE2_JIT_COMPLETE)
    JUMPHERE(partial);

  OP1(SLJIT_MOV, source_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr);
  OP1(SLJIT_MOV, source_end_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw));
  OP1(SLJIT_MOV, char1_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw) * 2);
  add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));

  if (common->mode != PCRE2_JIT_COMPLETE)
    {
    JUMPHERE(partial);
    OP1(SLJIT_MOV, source_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr);
    OP1(SLJIT_MOV, source_end_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw));
    OP1(SLJIT_MOV, char1_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw) * 2);

    check_partial(common, FALSE);
    add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));
    }

  JUMPHERE(jump);
  OP1(SLJIT_MOV, source_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr);
  OP1(SLJIT_MOV, source_end_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw));
  OP1(SLJIT_MOV, char1_reg, 0, SLJIT_MEM1(SLJIT_SP), common->iref_ptr + sizeof(sljit_sw) * 2);
  return;
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