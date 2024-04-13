static PCRE2_SPTR compile_iterator_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
PCRE2_UCHAR opcode;
PCRE2_UCHAR type;
sljit_u32 max = 0, exact;
sljit_s32 early_fail_ptr = PRIVATE_DATA(cc + 1);
sljit_s32 early_fail_type;
BOOL charpos_enabled;
PCRE2_UCHAR charpos_char;
unsigned int charpos_othercasebit;
PCRE2_SPTR end;
jump_list *no_match = NULL;
jump_list *no_char1_match = NULL;
struct sljit_jump *jump = NULL;
struct sljit_label *label;
int private_data_ptr = PRIVATE_DATA(cc);
int base = (private_data_ptr == 0) ? SLJIT_MEM1(STACK_TOP) : SLJIT_MEM1(SLJIT_SP);
int offset0 = (private_data_ptr == 0) ? STACK(0) : private_data_ptr;
int offset1 = (private_data_ptr == 0) ? STACK(1) : private_data_ptr + (int)sizeof(sljit_sw);
int tmp_base, tmp_offset;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
BOOL use_tmp;
#endif

PUSH_BACKTRACK(sizeof(char_iterator_backtrack), cc, NULL);

early_fail_type = (early_fail_ptr & 0x7);
early_fail_ptr >>= 3;

/* During recursion, these optimizations are disabled. */
if (common->early_fail_start_ptr == 0 && common->fast_forward_bc_ptr == NULL)
  {
  early_fail_ptr = 0;
  early_fail_type = type_skip;
  }

SLJIT_ASSERT(common->fast_forward_bc_ptr != NULL || early_fail_ptr == 0
  || (early_fail_ptr >= common->early_fail_start_ptr && early_fail_ptr <= common->early_fail_end_ptr));

if (early_fail_type == type_fail)
  add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), early_fail_ptr));

cc = get_iterator_parameters(common, cc, &opcode, &type, &max, &exact, &end);

if (type != OP_EXTUNI)
  {
  tmp_base = TMP3;
  tmp_offset = 0;
  }
else
  {
  tmp_base = SLJIT_MEM1(SLJIT_SP);
  tmp_offset = POSSESSIVE0;
  }

/* Handle fixed part first. */
if (exact > 1)
  {
  SLJIT_ASSERT(early_fail_ptr == 0);

  if (common->mode == PCRE2_JIT_COMPLETE
#ifdef SUPPORT_UNICODE
      && !common->utf
#endif
      && type != OP_ANYNL && type != OP_EXTUNI)
    {
    OP2(SLJIT_ADD, TMP1, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(exact));
    add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_GREATER, TMP1, 0, STR_END, 0));
    OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, exact);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &backtrack->topbacktracks, FALSE);
    OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, label);
    }
  else
    {
    OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, exact);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &backtrack->topbacktracks, TRUE);
    OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, label);
    }
  }
else if (exact == 1)
  {
  compile_char1_matchingpath(common, type, cc, &backtrack->topbacktracks, TRUE);

  if (early_fail_type == type_fail_range)
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), early_fail_ptr);
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), early_fail_ptr + (int)sizeof(sljit_sw));
    OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, TMP2, 0);
    OP2(SLJIT_SUB, TMP2, 0, STR_PTR, 0, TMP2, 0);
    add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_LESS_EQUAL, TMP2, 0, TMP1, 0));

    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr + (int)sizeof(sljit_sw), STR_PTR, 0);
    }
  }

switch(opcode)
  {
  case OP_STAR:
  case OP_UPTO:
  SLJIT_ASSERT(early_fail_ptr == 0 || opcode == OP_STAR);

  if (type == OP_ANYNL || type == OP_EXTUNI)
    {
    SLJIT_ASSERT(private_data_ptr == 0);
    SLJIT_ASSERT(early_fail_ptr == 0);

    allocate_stack(common, 2);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, 0);

    if (opcode == OP_UPTO)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE0, SLJIT_IMM, max);

    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &BACKTRACK_AS(char_iterator_backtrack)->u.backtracks, TRUE);
    if (opcode == OP_UPTO)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), POSSESSIVE0);
      OP2(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
      jump = JUMP(SLJIT_ZERO);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE0, TMP1, 0);
      }

    /* We cannot use TMP3 because of allocate_stack. */
    allocate_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
    JUMPTO(SLJIT_JUMP, label);
    if (jump != NULL)
      JUMPHERE(jump);
    BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
    break;
    }
#ifdef SUPPORT_UNICODE
  else if (type == OP_ALLANY && !common->invalid_utf)
#else
  else if (type == OP_ALLANY)
#endif
    {
    if (opcode == OP_STAR)
      {
      if (private_data_ptr == 0)
        allocate_stack(common, 2);

      OP1(SLJIT_MOV, base, offset0, STR_END, 0);
      OP1(SLJIT_MOV, base, offset1, STR_PTR, 0);

      OP1(SLJIT_MOV, STR_PTR, 0, STR_END, 0);
      process_partial_match(common);

      if (early_fail_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_END, 0);
      BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
      break;
      }
#ifdef SUPPORT_UNICODE
    else if (!common->utf)
#else
    else
#endif
      {
      if (private_data_ptr == 0)
        allocate_stack(common, 2);

      OP1(SLJIT_MOV, base, offset1, STR_PTR, 0);
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(max));

      if (common->mode == PCRE2_JIT_COMPLETE)
        {
        OP2U(SLJIT_SUB | SLJIT_SET_GREATER, STR_PTR, 0, STR_END, 0);
        CMOV(SLJIT_GREATER, STR_PTR, STR_END, 0);
        }
      else
        {
        jump = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, STR_END, 0);
        process_partial_match(common);
        JUMPHERE(jump);
        }

      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);

      if (early_fail_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
      BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
      break;
      }
    }

  charpos_enabled = FALSE;
  charpos_char = 0;
  charpos_othercasebit = 0;

  if ((type != OP_CHAR && type != OP_CHARI) && (*end == OP_CHAR || *end == OP_CHARI))
    {
#ifdef SUPPORT_UNICODE
    charpos_enabled = !common->utf || !HAS_EXTRALEN(end[1]);
#else
    charpos_enabled = TRUE;
#endif
    if (charpos_enabled && *end == OP_CHARI && char_has_othercase(common, end + 1))
      {
      charpos_othercasebit = char_get_othercase_bit(common, end + 1);
      if (charpos_othercasebit == 0)
        charpos_enabled = FALSE;
      }

    if (charpos_enabled)
      {
      charpos_char = end[1];
      /* Consume the OP_CHAR opcode. */
      end += 2;
#if PCRE2_CODE_UNIT_WIDTH == 8
      SLJIT_ASSERT((charpos_othercasebit >> 8) == 0);
#elif PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
      SLJIT_ASSERT((charpos_othercasebit >> 9) == 0);
      if ((charpos_othercasebit & 0x100) != 0)
        charpos_othercasebit = (charpos_othercasebit & 0xff) << 8;
#endif
      if (charpos_othercasebit != 0)
        charpos_char |= charpos_othercasebit;

      BACKTRACK_AS(char_iterator_backtrack)->u.charpos.enabled = TRUE;
      BACKTRACK_AS(char_iterator_backtrack)->u.charpos.chr = charpos_char;
      BACKTRACK_AS(char_iterator_backtrack)->u.charpos.othercasebit = charpos_othercasebit;
      }
    }

  if (charpos_enabled)
    {
    if (opcode == OP_UPTO)
      OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max + 1);

    /* Search the first instance of charpos_char. */
    jump = JUMP(SLJIT_JUMP);
    label = LABEL();
    if (opcode == OP_UPTO)
      {
      OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
      add_jump(compiler, &backtrack->topbacktracks, JUMP(SLJIT_ZERO));
      }
    compile_char1_matchingpath(common, type, cc, &backtrack->topbacktracks, FALSE);
    if (early_fail_ptr != 0)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
    JUMPHERE(jump);

    detect_partial_match(common, &backtrack->topbacktracks);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    if (charpos_othercasebit != 0)
      OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, charpos_othercasebit);
    CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, charpos_char, label);

    if (private_data_ptr == 0)
      allocate_stack(common, 2);
    OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
    OP1(SLJIT_MOV, base, offset1, STR_PTR, 0);

    if (opcode == OP_UPTO)
      {
      OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
      add_jump(compiler, &no_match, JUMP(SLJIT_ZERO));
      }

    /* Search the last instance of charpos_char. */
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &no_match, FALSE);
    if (early_fail_ptr != 0)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
    detect_partial_match(common, &no_match);
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
    if (charpos_othercasebit != 0)
      OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, charpos_othercasebit);

    if (opcode == OP_STAR)
      {
      CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, charpos_char, label);
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      JUMPTO(SLJIT_JUMP, label);
      }
    else
      {
      jump = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, charpos_char);
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      JUMPHERE(jump);
      OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
      JUMPTO(SLJIT_NOT_ZERO, label);
      }

    set_jumps(no_match, LABEL());
    OP2(SLJIT_ADD, STR_PTR, 0, base, offset0, SLJIT_IMM, IN_UCHARS(1));
    OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
    }
  else
    {
    if (private_data_ptr == 0)
      allocate_stack(common, 2);

    OP1(SLJIT_MOV, base, offset1, STR_PTR, 0);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    use_tmp = (!HAS_VIRTUAL_REGISTERS && opcode == OP_STAR);
    SLJIT_ASSERT(!use_tmp || tmp_base == TMP3);

    if (common->utf)
      OP1(SLJIT_MOV, use_tmp ? TMP3 : base, use_tmp ? 0 : offset0, STR_PTR, 0);
#endif
    if (opcode == OP_UPTO)
      OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);

    detect_partial_match(common, &no_match);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &no_char1_match, FALSE);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf)
      OP1(SLJIT_MOV, use_tmp ? TMP3 : base, use_tmp ? 0 : offset0, STR_PTR, 0);
#endif

    if (opcode == OP_UPTO)
      {
      OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
      add_jump(compiler, &no_match, JUMP(SLJIT_ZERO));
      }

    detect_partial_match_to(common, label);
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

    set_jumps(no_char1_match, LABEL());
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf)
      {
      set_jumps(no_match, LABEL());
      if (use_tmp)
        {
        OP1(SLJIT_MOV, STR_PTR, 0, TMP3, 0);
        OP1(SLJIT_MOV, base, offset0, TMP3, 0);
        }
      else
        OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
      }
    else
#endif
      {
      OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      set_jumps(no_match, LABEL());
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      }

    if (early_fail_ptr != 0)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
    }

  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  break;

  case OP_MINSTAR:
  if (private_data_ptr == 0)
    allocate_stack(common, 1);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  if (early_fail_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
  break;

  case OP_MINUPTO:
  SLJIT_ASSERT(early_fail_ptr == 0);
  if (private_data_ptr == 0)
    allocate_stack(common, 2);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  OP1(SLJIT_MOV, base, offset1, SLJIT_IMM, max + 1);
  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  break;

  case OP_QUERY:
  case OP_MINQUERY:
  SLJIT_ASSERT(early_fail_ptr == 0);
  if (private_data_ptr == 0)
    allocate_stack(common, 1);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  if (opcode == OP_QUERY)
    compile_char1_matchingpath(common, type, cc, &BACKTRACK_AS(char_iterator_backtrack)->u.backtracks, TRUE);
  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  break;

  case OP_EXACT:
  break;

  case OP_POSSTAR:
#if defined SUPPORT_UNICODE
  if (type == OP_ALLANY && !common->invalid_utf)
#else
  if (type == OP_ALLANY)
#endif
    {
    OP1(SLJIT_MOV, STR_PTR, 0, STR_END, 0);
    process_partial_match(common);
    if (early_fail_ptr != 0)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_END, 0);
    break;
    }

#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
  if (common->utf)
    {
    OP1(SLJIT_MOV, tmp_base, tmp_offset, STR_PTR, 0);
    detect_partial_match(common, &no_match);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &no_match, FALSE);
    OP1(SLJIT_MOV, tmp_base, tmp_offset, STR_PTR, 0);
    detect_partial_match_to(common, label);

    set_jumps(no_match, LABEL());
    OP1(SLJIT_MOV, STR_PTR, 0, tmp_base, tmp_offset);
    if (early_fail_ptr != 0)
      {
      if (!HAS_VIRTUAL_REGISTERS && tmp_base == TMP3)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, TMP3, 0);
      else
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
      }
    break;
    }
#endif

  detect_partial_match(common, &no_match);
  label = LABEL();
  compile_char1_matchingpath(common, type, cc, &no_char1_match, FALSE);
  detect_partial_match_to(common, label);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

  set_jumps(no_char1_match, LABEL());
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  set_jumps(no_match, LABEL());
  if (early_fail_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), early_fail_ptr, STR_PTR, 0);
  break;

  case OP_POSUPTO:
  SLJIT_ASSERT(early_fail_ptr == 0);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
  if (common->utf)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1, STR_PTR, 0);
    OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);

    detect_partial_match(common, &no_match);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &no_match, FALSE);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1, STR_PTR, 0);
    OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
    add_jump(compiler, &no_match, JUMP(SLJIT_ZERO));
    detect_partial_match_to(common, label);

    set_jumps(no_match, LABEL());
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1);
    break;
    }
#endif

  if (type == OP_ALLANY)
    {
    OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(max));

    if (common->mode == PCRE2_JIT_COMPLETE)
      {
      OP2U(SLJIT_SUB | SLJIT_SET_GREATER, STR_PTR, 0, STR_END, 0);
      CMOV(SLJIT_GREATER, STR_PTR, STR_END, 0);
      }
    else
      {
      jump = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, STR_END, 0);
      process_partial_match(common);
      JUMPHERE(jump);
      }
    break;
    }

  OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);

  detect_partial_match(common, &no_match);
  label = LABEL();
  compile_char1_matchingpath(common, type, cc, &no_char1_match, FALSE);
  OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
  add_jump(compiler, &no_match, JUMP(SLJIT_ZERO));
  detect_partial_match_to(common, label);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

  set_jumps(no_char1_match, LABEL());
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  set_jumps(no_match, LABEL());
  break;

  case OP_POSQUERY:
  SLJIT_ASSERT(early_fail_ptr == 0);
  OP1(SLJIT_MOV, tmp_base, tmp_offset, STR_PTR, 0);
  compile_char1_matchingpath(common, type, cc, &no_match, TRUE);
  OP1(SLJIT_MOV, tmp_base, tmp_offset, STR_PTR, 0);
  set_jumps(no_match, LABEL());
  OP1(SLJIT_MOV, STR_PTR, 0, tmp_base, tmp_offset);
  break;

  default:
  SLJIT_UNREACHABLE();
  break;
  }

count_match(common);
return end;
}