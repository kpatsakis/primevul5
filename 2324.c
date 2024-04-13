static PCRE2_SPTR compile_simple_assertion_matchingpath(compiler_common *common, PCRE2_UCHAR type, PCRE2_SPTR cc, jump_list **backtracks)
{
DEFINE_COMPILER;
int length;
struct sljit_jump *jump[4];
#ifdef SUPPORT_UNICODE
struct sljit_label *label;
#endif /* SUPPORT_UNICODE */

switch(type)
  {
  case OP_SOD:
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, begin));
    }
  else
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, begin));
  add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, TMP1, 0));
  return cc;

  case OP_SOM:
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, str));
    }
  else
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, str));
  add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, TMP1, 0));
  return cc;

  case OP_NOT_WORD_BOUNDARY:
  case OP_WORD_BOUNDARY:
  add_jump(compiler, &common->wordboundary, JUMP(SLJIT_FAST_CALL));
#ifdef SUPPORT_UNICODE
  if (common->invalid_utf)
    {
    add_jump(compiler, backtracks, CMP((type == OP_NOT_WORD_BOUNDARY) ? SLJIT_NOT_EQUAL : SLJIT_SIG_LESS_EQUAL, TMP2, 0, SLJIT_IMM, 0));
    return cc;
    }
#endif /* SUPPORT_UNICODE */
  sljit_set_current_flags(compiler, SLJIT_SET_Z);
  add_jump(compiler, backtracks, JUMP(type == OP_NOT_WORD_BOUNDARY ? SLJIT_NOT_ZERO : SLJIT_ZERO));
  return cc;

  case OP_EODN:
  /* Requires rather complex checks. */
  jump[0] = CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0);
  if (common->nltype == NLTYPE_FIXED && common->newline > 255)
    {
    OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    if (common->mode == PCRE2_JIT_COMPLETE)
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, STR_END, 0));
    else
      {
      jump[1] = CMP(SLJIT_EQUAL, TMP2, 0, STR_END, 0);
      OP2U(SLJIT_SUB | SLJIT_SET_LESS, TMP2, 0, STR_END, 0);
      OP_FLAGS(SLJIT_MOV, TMP2, 0, SLJIT_LESS);
      OP2U(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff);
      OP_FLAGS(SLJIT_OR | SLJIT_SET_Z, TMP2, 0, SLJIT_NOT_EQUAL);
      add_jump(compiler, backtracks, JUMP(SLJIT_NOT_EQUAL));
      check_partial(common, TRUE);
      add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));
      JUMPHERE(jump[1]);
      }
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, common->newline & 0xff));
    }
  else if (common->nltype == NLTYPE_FIXED)
    {
    OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, STR_END, 0));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, common->newline));
    }
  else
    {
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    jump[1] = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_CR);
    OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
    OP2U(SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_GREATER, TMP2, 0, STR_END, 0);
    jump[2] = JUMP(SLJIT_GREATER);
    add_jump(compiler, backtracks, JUMP(SLJIT_NOT_EQUAL) /* LESS */);
    /* Equal. */
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    jump[3] = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_NL);
    add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));

    JUMPHERE(jump[1]);
    if (common->nltype == NLTYPE_ANYCRLF)
      {
      OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP2, 0, STR_END, 0));
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_NL));
      }
    else
      {
      OP1(SLJIT_MOV, TMP3, 0, STR_PTR, 0);
      read_char(common, common->nlmin, common->nlmax, backtracks, READ_CHAR_UPDATE_STR_PTR);
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, STR_PTR, 0, STR_END, 0));
      add_jump(compiler, &common->anynewline, JUMP(SLJIT_FAST_CALL));
      sljit_set_current_flags(compiler, SLJIT_SET_Z);
      add_jump(compiler, backtracks, JUMP(SLJIT_ZERO));
      OP1(SLJIT_MOV, STR_PTR, 0, TMP3, 0);
      }
    JUMPHERE(jump[2]);
    JUMPHERE(jump[3]);
    }
  JUMPHERE(jump[0]);
  if (common->mode != PCRE2_JIT_COMPLETE)
    check_partial(common, TRUE);
  return cc;

  case OP_EOD:
  add_jump(compiler, backtracks, CMP(SLJIT_LESS, STR_PTR, 0, STR_END, 0));
  if (common->mode != PCRE2_JIT_COMPLETE)
    check_partial(common, TRUE);
  return cc;

  case OP_DOLL:
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP2, 0, ARGUMENTS, 0);
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTEOL);
    }
  else
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTEOL);
  add_jump(compiler, backtracks, JUMP(SLJIT_NOT_ZERO));

  if (!common->endonly)
    compile_simple_assertion_matchingpath(common, OP_EODN, cc, backtracks);
  else
    {
    add_jump(compiler, backtracks, CMP(SLJIT_LESS, STR_PTR, 0, STR_END, 0));
    check_partial(common, FALSE);
    }
  return cc;

  case OP_DOLLM:
  jump[1] = CMP(SLJIT_LESS, STR_PTR, 0, STR_END, 0);
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP2, 0, ARGUMENTS, 0);
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTEOL);
    }
  else
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTEOL);
  add_jump(compiler, backtracks, JUMP(SLJIT_NOT_ZERO));
  check_partial(common, FALSE);
  jump[0] = JUMP(SLJIT_JUMP);
  JUMPHERE(jump[1]);

  if (common->nltype == NLTYPE_FIXED && common->newline > 255)
    {
    OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    if (common->mode == PCRE2_JIT_COMPLETE)
      add_jump(compiler, backtracks, CMP(SLJIT_GREATER, TMP2, 0, STR_END, 0));
    else
      {
      jump[1] = CMP(SLJIT_LESS_EQUAL, TMP2, 0, STR_END, 0);
      /* STR_PTR = STR_END - IN_UCHARS(1) */
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff));
      check_partial(common, TRUE);
      add_jump(compiler, backtracks, JUMP(SLJIT_JUMP));
      JUMPHERE(jump[1]);
      }

    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, common->newline & 0xff));
    }
  else
    {
    peek_char(common, common->nlmax, TMP3, 0, NULL);
    check_newlinechar(common, common->nltype, backtracks, FALSE);
    }
  JUMPHERE(jump[0]);
  return cc;

  case OP_CIRC:
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP2, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(jit_arguments, begin));
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER, STR_PTR, 0, TMP1, 0));
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(TMP2), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTBOL);
    add_jump(compiler, backtracks, JUMP(SLJIT_NOT_ZERO));
    }
  else
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, begin));
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER, STR_PTR, 0, TMP1, 0));
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTBOL);
    add_jump(compiler, backtracks, JUMP(SLJIT_NOT_ZERO));
    }
  return cc;

  case OP_CIRCM:
  /* TMP2 might be used by peek_char_back. */
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, begin));
    jump[1] = CMP(SLJIT_GREATER, STR_PTR, 0, TMP2, 0);
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTBOL);
    }
  else
    {
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, begin));
    jump[1] = CMP(SLJIT_GREATER, STR_PTR, 0, TMP2, 0);
    OP2U(SLJIT_AND32 | SLJIT_SET_Z, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, options), SLJIT_IMM, PCRE2_NOTBOL);
    }
  add_jump(compiler, backtracks, JUMP(SLJIT_NOT_ZERO));
  jump[0] = JUMP(SLJIT_JUMP);
  JUMPHERE(jump[1]);

  if (!common->alt_circumflex)
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));

  if (common->nltype == NLTYPE_FIXED && common->newline > 255)
    {
    OP2(SLJIT_SUB, TMP1, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(2));
    add_jump(compiler, backtracks, CMP(SLJIT_LESS, TMP1, 0, TMP2, 0));
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-2));
    OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff));
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP2, 0, SLJIT_IMM, common->newline & 0xff));
    }
  else
    {
    peek_char_back(common, common->nlmax, backtracks);
    check_newlinechar(common, common->nltype, backtracks, FALSE);
    }
  JUMPHERE(jump[0]);
  return cc;

  case OP_REVERSE:
  length = GET(cc, 0);
  if (length == 0)
    return cc + LINK_SIZE;
  if (HAS_VIRTUAL_REGISTERS)
    {
    OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, begin));
    }
  else
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(ARGUMENTS), SLJIT_OFFSETOF(jit_arguments, begin));
#ifdef SUPPORT_UNICODE
  if (common->utf)
    {
    OP1(SLJIT_MOV, TMP3, 0, SLJIT_IMM, length);
    label = LABEL();
    add_jump(compiler, backtracks, CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, TMP2, 0));
    move_back(common, backtracks, FALSE);
    OP2(SLJIT_SUB | SLJIT_SET_Z, TMP3, 0, TMP3, 0, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, label);
    }
  else
#endif
    {
    OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(length));
    add_jump(compiler, backtracks, CMP(SLJIT_LESS, STR_PTR, 0, TMP2, 0));
    }
  check_start_used_ptr(common);
  return cc + LINK_SIZE;
  }
SLJIT_UNREACHABLE();
return cc;
}