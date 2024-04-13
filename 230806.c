static PCRE2_SPTR compile_char1_matchingpath(compiler_common *common, PCRE2_UCHAR type, PCRE2_SPTR cc, jump_list **backtracks, BOOL check_str_ptr)
{
DEFINE_COMPILER;
int length;
unsigned int c, oc, bit;
compare_context context;
struct sljit_jump *jump[3];
jump_list *end_list;
#ifdef SUPPORT_UNICODE
PCRE2_UCHAR propdata[5];
#endif /* SUPPORT_UNICODE */

switch(type)
  {
  case OP_NOT_DIGIT:
  case OP_DIGIT:
  /* Digits are usually 0-9, so it is worth to optimize them. */
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
  if (common->utf && is_char7_bitset((const sljit_u8*)common->ctypes - cbit_length + cbit_digit, FALSE))
    read_char7_type(common, type == OP_NOT_DIGIT);
  else
#endif
    read_char8_type(common, type == OP_NOT_DIGIT);
    /* Flip the starting bit in the negative case. */
  OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ctype_digit);
  add_jump(compiler, backtracks, JUMP(type == OP_DIGIT ? SLJIT_ZERO : SLJIT_NOT_ZERO));
  return cc;

  case OP_NOT_WHITESPACE:
  case OP_WHITESPACE:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
  if (common->utf && is_char7_bitset((const sljit_u8*)common->ctypes - cbit_length + cbit_space, FALSE))
    read_char7_type(common, type == OP_NOT_WHITESPACE);
  else
#endif
    read_char8_type(common, type == OP_NOT_WHITESPACE);
  OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ctype_space);
  add_jump(compiler, backtracks, JUMP(type == OP_WHITESPACE ? SLJIT_ZERO : SLJIT_NOT_ZERO));
  return cc;

  case OP_NOT_WORDCHAR:
  case OP_WORDCHAR:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
  if (common->utf && is_char7_bitset((const sljit_u8*)common->ctypes - cbit_length + cbit_word, FALSE))
    read_char7_type(common, type == OP_NOT_WORDCHAR);
  else
#endif
    read_char8_type(common, type == OP_NOT_WORDCHAR);
  OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, ctype_word);
  add_jump(compiler, backtracks, JUMP(type == OP_WORDCHAR ? SLJIT_ZERO : SLJIT_NOT_ZERO));
  return cc;

  case OP_ANY:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  read_char_range(common, common->nlmin, common->nlmax, TRUE);
  if (common->nltype == NLTYPE_FIXED && common->newline > 255)
    {
    jump[0] = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, (common->newline >> 8) & 0xff);
    end_list = NULL;
    if (common->mode != PCRE2_JIT_PARTIAL_HARD)
      add_jump(compiler, &end_list, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));
    else
      check_str_end(common, &end_list);

    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, common->newline & 0xff));
    set_jumps(end_list, LABEL());
    JUMPHERE(jump[0]);
    }
  else
    check_newlinechar(common, common->nltype, backtracks, TRUE);
  return cc;

  case OP_ALLANY:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
  if (common->utf)
    {
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
#if PCRE2_CODE_UNIT_WIDTH == 8 || PCRE2_CODE_UNIT_WIDTH == 16
#if PCRE2_CODE_UNIT_WIDTH == 8
    jump[0] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xc0);
    OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
#elif PCRE2_CODE_UNIT_WIDTH == 16
    jump[0] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xd800);
    OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, 0xfc00);
    OP2(SLJIT_SUB | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, SLJIT_IMM, 0xd800);
    OP_FLAGS(SLJIT_MOV, TMP1, 0, SLJIT_EQUAL);
    OP2(SLJIT_SHL, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
#endif
    JUMPHERE(jump[0]);
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16] */
    return cc;
    }
#endif
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  return cc;

  case OP_ANYBYTE:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  return cc;

#ifdef SUPPORT_UNICODE
  case OP_NOTPROP:
  case OP_PROP:
  propdata[0] = XCL_HASPROP;
  propdata[1] = type == OP_NOTPROP ? XCL_NOTPROP : XCL_PROP;
  propdata[2] = cc[0];
  propdata[3] = cc[1];
  propdata[4] = XCL_END;
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  compile_xclass_matchingpath(common, propdata, backtracks);
  return cc + 2;
#endif

  case OP_ANYNL:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  read_char_range(common, common->bsr_nlmin, common->bsr_nlmax, FALSE);
  jump[0] = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_CR);
  /* We don't need to handle soft partial matching case. */
  end_list = NULL;
  if (common->mode != PCRE2_JIT_PARTIAL_HARD)
    add_jump(compiler, &end_list, CMP(SLJIT_GREATER_EQUAL, STR_PTR, 0, STR_END, 0));
  else
    check_str_end(common, &end_list);
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
  jump[1] = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, CHAR_NL);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  jump[2] = JUMP(SLJIT_JUMP);
  JUMPHERE(jump[0]);
  check_newlinechar(common, common->bsr_nltype, backtracks, FALSE);
  set_jumps(end_list, LABEL());
  JUMPHERE(jump[1]);
  JUMPHERE(jump[2]);
  return cc;

  case OP_NOT_HSPACE:
  case OP_HSPACE:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  read_char_range(common, 0x9, 0x3000, type == OP_NOT_HSPACE);
  add_jump(compiler, &common->hspace, JUMP(SLJIT_FAST_CALL));
  sljit_set_current_flags(compiler, SLJIT_SET_Z);
  add_jump(compiler, backtracks, JUMP(type == OP_NOT_HSPACE ? SLJIT_NOT_ZERO : SLJIT_ZERO));
  return cc;

  case OP_NOT_VSPACE:
  case OP_VSPACE:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  read_char_range(common, 0xa, 0x2029, type == OP_NOT_VSPACE);
  add_jump(compiler, &common->vspace, JUMP(SLJIT_FAST_CALL));
  sljit_set_current_flags(compiler, SLJIT_SET_Z);
  add_jump(compiler, backtracks, JUMP(type == OP_NOT_VSPACE ? SLJIT_NOT_ZERO : SLJIT_ZERO));
  return cc;

#ifdef SUPPORT_UNICODE
  case OP_EXTUNI:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);

  SLJIT_ASSERT(TMP1 == SLJIT_R0 && STR_PTR == SLJIT_R1);
  OP1(SLJIT_MOV, SLJIT_R0, 0, ARGUMENTS, 0);

#if PCRE2_CODE_UNIT_WIDTH != 32
  sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_RET(SW) | SLJIT_ARG1(SW) | SLJIT_ARG2(SW), SLJIT_IMM,
      common->utf ? SLJIT_FUNC_OFFSET(do_extuni_utf) : SLJIT_FUNC_OFFSET(do_extuni_no_utf));
#else
  sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_RET(SW) | SLJIT_ARG1(SW) | SLJIT_ARG2(SW), SLJIT_IMM, SLJIT_FUNC_OFFSET(do_extuni_no_utf));
#endif

  OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_RETURN_REG, 0);

  if (common->mode == PCRE2_JIT_PARTIAL_HARD)
    {
    jump[0] = CMP(SLJIT_LESS, SLJIT_RETURN_REG, 0, STR_END, 0);
    /* Since we successfully read a char above, partial matching must occure. */
    check_partial(common, TRUE);
    JUMPHERE(jump[0]);
    }
  return cc;
#endif

  case OP_CHAR:
  case OP_CHARI:
  length = 1;
#ifdef SUPPORT_UNICODE
  if (common->utf && HAS_EXTRALEN(*cc)) length += GET_EXTRALEN(*cc);
#endif
  if (common->mode == PCRE2_JIT_COMPLETE && check_str_ptr
      && (type == OP_CHAR || !char_has_othercase(common, cc) || char_get_othercase_bit(common, cc) != 0))
    {
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(length));
    add_jump(compiler, backtracks, CMP(SLJIT_GREATER, STR_PTR, 0, STR_END, 0));

    context.length = IN_UCHARS(length);
    context.sourcereg = -1;
#if defined SLJIT_UNALIGNED && SLJIT_UNALIGNED
    context.ucharptr = 0;
#endif
    return byte_sequence_compare(common, type == OP_CHARI, cc, &context, backtracks);
    }

  if (check_str_ptr)
    detect_partial_match(common, backtracks);
#ifdef SUPPORT_UNICODE
  if (common->utf)
    {
    GETCHAR(c, cc);
    }
  else
#endif
    c = *cc;

  if (type == OP_CHAR || !char_has_othercase(common, cc))
    {
    read_char_range(common, c, c, FALSE);
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, c));
    return cc + length;
    }
  oc = char_othercase(common, c);
  read_char_range(common, c < oc ? c : oc, c > oc ? c : oc, FALSE);
  bit = c ^ oc;
  if (is_powerof2(bit))
    {
    OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, bit);
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, c | bit));
    return cc + length;
    }
  jump[0] = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, c);
  add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, oc));
  JUMPHERE(jump[0]);
  return cc + length;

  case OP_NOT:
  case OP_NOTI:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);

  length = 1;
#ifdef SUPPORT_UNICODE
  if (common->utf)
    {
#if PCRE2_CODE_UNIT_WIDTH == 8
    c = *cc;
    if (c < 128)
      {
      OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(STR_PTR), 0);
      if (type == OP_NOT || !char_has_othercase(common, cc))
        add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, c));
      else
        {
        /* Since UTF8 code page is fixed, we know that c is in [a-z] or [A-Z] range. */
        OP2(SLJIT_OR, TMP2, 0, TMP1, 0, SLJIT_IMM, 0x20);
        add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, c | 0x20));
        }
      /* Skip the variable-length character. */
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      jump[0] = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, 0xc0);
      OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(TMP1), (sljit_sw)PRIV(utf8_table4) - 0xc0);
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, TMP1, 0);
      JUMPHERE(jump[0]);
      return cc + 1;
      }
    else
#endif /* PCRE2_CODE_UNIT_WIDTH == 8 */
      {
      GETCHARLEN(c, cc, length);
      }
    }
  else
#endif /* SUPPORT_UNICODE */
    c = *cc;

  if (type == OP_NOT || !char_has_othercase(common, cc))
    {
    read_char_range(common, c, c, TRUE);
    add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, c));
    }
  else
    {
    oc = char_othercase(common, c);
    read_char_range(common, c < oc ? c : oc, c > oc ? c : oc, TRUE);
    bit = c ^ oc;
    if (is_powerof2(bit))
      {
      OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, bit);
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, c | bit));
      }
    else
      {
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, c));
      add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, oc));
      }
    }
  return cc + length;

  case OP_CLASS:
  case OP_NCLASS:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
  bit = (common->utf && is_char7_bitset((const sljit_u8 *)cc, type == OP_NCLASS)) ? 127 : 255;
  read_char_range(common, 0, bit, type == OP_NCLASS);
#else
  read_char_range(common, 0, 255, type == OP_NCLASS);
#endif

  if (optimize_class(common, (const sljit_u8 *)cc, type == OP_NCLASS, FALSE, backtracks))
    return cc + 32 / sizeof(PCRE2_UCHAR);

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
  jump[0] = NULL;
  if (common->utf)
    {
    jump[0] = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, bit);
    if (type == OP_CLASS)
      {
      add_jump(compiler, backtracks, jump[0]);
      jump[0] = NULL;
      }
    }
#elif PCRE2_CODE_UNIT_WIDTH != 8
  jump[0] = CMP(SLJIT_GREATER, TMP1, 0, SLJIT_IMM, 255);
  if (type == OP_CLASS)
    {
    add_jump(compiler, backtracks, jump[0]);
    jump[0] = NULL;
    }
#endif /* SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8 */

  OP2(SLJIT_AND, TMP2, 0, TMP1, 0, SLJIT_IMM, 0x7);
  OP2(SLJIT_LSHR, TMP1, 0, TMP1, 0, SLJIT_IMM, 3);
  OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(TMP1), (sljit_sw)cc);
  OP2(SLJIT_SHL, TMP2, 0, SLJIT_IMM, 1, TMP2, 0);
  OP2(SLJIT_AND | SLJIT_SET_Z, SLJIT_UNUSED, 0, TMP1, 0, TMP2, 0);
  add_jump(compiler, backtracks, JUMP(SLJIT_ZERO));

#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
  if (jump[0] != NULL)
    JUMPHERE(jump[0]);
#endif
  return cc + 32 / sizeof(PCRE2_UCHAR);

#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
  case OP_XCLASS:
  if (check_str_ptr)
    detect_partial_match(common, backtracks);
  compile_xclass_matchingpath(common, cc + LINK_SIZE, backtracks);
  return cc + GET(cc, 0) - 1;
#endif
  }
SLJIT_UNREACHABLE();
return cc;
}