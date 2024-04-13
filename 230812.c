static PCRE2_SPTR compile_iterator_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
PCRE2_UCHAR opcode;
PCRE2_UCHAR type;
sljit_u32 max = 0, exact;
BOOL fast_fail;
sljit_s32 fast_str_ptr;
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

PUSH_BACKTRACK(sizeof(char_iterator_backtrack), cc, NULL);

fast_str_ptr = PRIVATE_DATA(cc + 1);
fast_fail = TRUE;

SLJIT_ASSERT(common->fast_forward_bc_ptr == NULL || fast_str_ptr == 0 || cc == common->fast_forward_bc_ptr);

if (cc == common->fast_forward_bc_ptr)
  fast_fail = FALSE;
else if (common->fast_fail_start_ptr == 0)
  fast_str_ptr = 0;

SLJIT_ASSERT(common->fast_forward_bc_ptr != NULL || fast_str_ptr == 0
  || (fast_str_ptr >= common->fast_fail_start_ptr && fast_str_ptr <= common->fast_fail_end_ptr));

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

if (fast_fail && fast_str_ptr != 0)
  add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), fast_str_ptr));

/* Handle fixed part first. */
if (exact > 1)
  {
  SLJIT_ASSERT(fast_str_ptr == 0);
  if (common->mode == PCRE2_JIT_COMPLETE
#ifdef SUPPORT_UNICODE
      && !common->utf
#endif
      )
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
  compile_char1_matchingpath(common, type, cc, &backtrack->topbacktracks, TRUE);

switch(opcode)
  {
  case OP_STAR:
  case OP_UPTO:
  SLJIT_ASSERT(fast_str_ptr == 0 || opcode == OP_STAR);

  if (type == OP_ANYNL || type == OP_EXTUNI)
    {
    SLJIT_ASSERT(private_data_ptr == 0);
    SLJIT_ASSERT(fast_str_ptr == 0);

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

    /* We cannot use TMP3 because of this allocate_stack. */
    allocate_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
    JUMPTO(SLJIT_JUMP, label);
    if (jump != NULL)
      JUMPHERE(jump);
    }
  else
    {
    charpos_enabled = FALSE;
    charpos_char = 0;
    charpos_othercasebit = 0;

    if ((type != OP_CHAR && type != OP_CHARI) && (*end == OP_CHAR || *end == OP_CHARI))
      {
      charpos_enabled = TRUE;
#ifdef SUPPORT_UNICODE
      charpos_enabled = !common->utf || !HAS_EXTRALEN(end[1]);
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
        /* Consumpe the OP_CHAR opcode. */
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
      if (fast_str_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
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
      if (fast_str_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
      detect_partial_match(common, &no_match);
      OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(0));
      if (charpos_othercasebit != 0)
        OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, charpos_othercasebit);
      if (opcode == OP_STAR)
        {
        CMPTO(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, charpos_char, label);
        OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
        }
      else
        {
        jump = CMP(SLJIT_NOT_EQUAL, TMP1, 0, SLJIT_IMM, charpos_char);
        OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
        JUMPHERE(jump);
        }

      if (opcode == OP_UPTO)
        {
        OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
        JUMPTO(SLJIT_NOT_ZERO, label);
        }
      else
        JUMPTO(SLJIT_JUMP, label);

      set_jumps(no_match, LABEL());
      OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
      OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      }
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    else if (common->utf)
      {
      if (private_data_ptr == 0)
        allocate_stack(common, 2);

      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      OP1(SLJIT_MOV, base, offset1, STR_PTR, 0);

      if (opcode == OP_UPTO)
        OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);

      label = LABEL();
      compile_char1_matchingpath(common, type, cc, &no_match, TRUE);
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);

      if (opcode == OP_UPTO)
        {
        OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
        JUMPTO(SLJIT_NOT_ZERO, label);
        }
      else
        JUMPTO(SLJIT_JUMP, label);

      set_jumps(no_match, LABEL());
      OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
      if (fast_str_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
      }
#endif
    else
      {
      if (private_data_ptr == 0)
        allocate_stack(common, 2);

      OP1(SLJIT_MOV, base, offset1, STR_PTR, 0);
      if (opcode == OP_UPTO)
        OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);

      label = LABEL();
      detect_partial_match(common, &no_match);
      compile_char1_matchingpath(common, type, cc, &no_char1_match, FALSE);
      if (opcode == OP_UPTO)
        {
        OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
        JUMPTO(SLJIT_NOT_ZERO, label);
        OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
        }
      else
        JUMPTO(SLJIT_JUMP, label);

      set_jumps(no_char1_match, LABEL());
      OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
      set_jumps(no_match, LABEL());
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      if (fast_str_ptr != 0)
        OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
      }
    }
  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  break;

  case OP_MINSTAR:
  if (private_data_ptr == 0)
    allocate_stack(common, 1);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  if (fast_str_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
  break;

  case OP_MINUPTO:
  SLJIT_ASSERT(fast_str_ptr == 0);
  if (private_data_ptr == 0)
    allocate_stack(common, 2);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  OP1(SLJIT_MOV, base, offset1, SLJIT_IMM, max + 1);
  BACKTRACK_AS(char_iterator_backtrack)->matchingpath = LABEL();
  break;

  case OP_QUERY:
  case OP_MINQUERY:
  SLJIT_ASSERT(fast_str_ptr == 0);
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
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
  if (common->utf)
    {
    OP1(SLJIT_MOV, tmp_base, tmp_offset, STR_PTR, 0);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &no_match, TRUE);
    OP1(SLJIT_MOV, tmp_base, tmp_offset, STR_PTR, 0);
    JUMPTO(SLJIT_JUMP, label);
    set_jumps(no_match, LABEL());
    OP1(SLJIT_MOV, STR_PTR, 0, tmp_base, tmp_offset);
    if (fast_str_ptr != 0)
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
    break;
    }
#endif
  label = LABEL();
  detect_partial_match(common, &no_match);
  compile_char1_matchingpath(common, type, cc, &no_char1_match, FALSE);
  JUMPTO(SLJIT_JUMP, label);
  set_jumps(no_char1_match, LABEL());
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  set_jumps(no_match, LABEL());
  if (fast_str_ptr != 0)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), fast_str_ptr, STR_PTR, 0);
  break;

  case OP_POSUPTO:
  SLJIT_ASSERT(fast_str_ptr == 0);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
  if (common->utf)
    {
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1, STR_PTR, 0);
    OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);
    label = LABEL();
    compile_char1_matchingpath(common, type, cc, &no_match, TRUE);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1, STR_PTR, 0);
    OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
    JUMPTO(SLJIT_NOT_ZERO, label);
    set_jumps(no_match, LABEL());
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1);
    break;
    }
#endif
  OP1(SLJIT_MOV, tmp_base, tmp_offset, SLJIT_IMM, max);
  label = LABEL();
  detect_partial_match(common, &no_match);
  compile_char1_matchingpath(common, type, cc, &no_char1_match, FALSE);
  OP2(SLJIT_SUB | SLJIT_SET_Z, tmp_base, tmp_offset, tmp_base, tmp_offset, SLJIT_IMM, 1);
  JUMPTO(SLJIT_NOT_ZERO, label);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  set_jumps(no_char1_match, LABEL());
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  set_jumps(no_match, LABEL());
  break;

  case OP_POSQUERY:
  SLJIT_ASSERT(fast_str_ptr == 0);
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