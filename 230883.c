static void set_private_data_ptrs(compiler_common *common, int *private_data_start, PCRE2_SPTR ccend)
{
PCRE2_SPTR cc = common->start;
PCRE2_SPTR alternative;
PCRE2_SPTR end = NULL;
int private_data_ptr = *private_data_start;
int space, size, bracketlen;
BOOL repeat_check = TRUE;

while (cc < ccend)
  {
  space = 0;
  size = 0;
  bracketlen = 0;
  if (private_data_ptr > SLJIT_MAX_LOCAL_SIZE)
    break;

  if (repeat_check && (*cc == OP_ONCE || *cc == OP_BRA || *cc == OP_CBRA || *cc == OP_COND))
    {
    if (detect_repeat(common, cc))
      {
      /* These brackets are converted to repeats, so no global
      based single character repeat is allowed. */
      if (cc >= end)
        end = bracketend(cc);
      }
    }
  repeat_check = TRUE;

  switch(*cc)
    {
    case OP_KET:
    if (common->private_data_ptrs[cc + 1 - common->start] != 0)
      {
      common->private_data_ptrs[cc - common->start] = private_data_ptr;
      private_data_ptr += sizeof(sljit_sw);
      cc += common->private_data_ptrs[cc + 1 - common->start];
      }
    cc += 1 + LINK_SIZE;
    break;

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    case OP_ONCE:
    case OP_BRAPOS:
    case OP_SBRA:
    case OP_SBRAPOS:
    case OP_SCOND:
    common->private_data_ptrs[cc - common->start] = private_data_ptr;
    private_data_ptr += sizeof(sljit_sw);
    bracketlen = 1 + LINK_SIZE;
    break;

    case OP_CBRAPOS:
    case OP_SCBRAPOS:
    common->private_data_ptrs[cc - common->start] = private_data_ptr;
    private_data_ptr += sizeof(sljit_sw);
    bracketlen = 1 + LINK_SIZE + IMM2_SIZE;
    break;

    case OP_COND:
    /* Might be a hidden SCOND. */
    alternative = cc + GET(cc, 1);
    if (*alternative == OP_KETRMAX || *alternative == OP_KETRMIN)
      {
      common->private_data_ptrs[cc - common->start] = private_data_ptr;
      private_data_ptr += sizeof(sljit_sw);
      }
    bracketlen = 1 + LINK_SIZE;
    break;

    case OP_BRA:
    bracketlen = 1 + LINK_SIZE;
    break;

    case OP_CBRA:
    case OP_SCBRA:
    bracketlen = 1 + LINK_SIZE + IMM2_SIZE;
    break;

    case OP_BRAZERO:
    case OP_BRAMINZERO:
    case OP_BRAPOSZERO:
    repeat_check = FALSE;
    size = 1;
    break;

    CASE_ITERATOR_PRIVATE_DATA_1
    space = 1;
    size = -2;
    break;

    CASE_ITERATOR_PRIVATE_DATA_2A
    space = 2;
    size = -2;
    break;

    CASE_ITERATOR_PRIVATE_DATA_2B
    space = 2;
    size = -(2 + IMM2_SIZE);
    break;

    CASE_ITERATOR_TYPE_PRIVATE_DATA_1
    space = 1;
    size = 1;
    break;

    CASE_ITERATOR_TYPE_PRIVATE_DATA_2A
    if (cc[1] != OP_ANYNL && cc[1] != OP_EXTUNI)
      space = 2;
    size = 1;
    break;

    case OP_TYPEUPTO:
    if (cc[1 + IMM2_SIZE] != OP_ANYNL && cc[1 + IMM2_SIZE] != OP_EXTUNI)
      space = 2;
    size = 1 + IMM2_SIZE;
    break;

    case OP_TYPEMINUPTO:
    space = 2;
    size = 1 + IMM2_SIZE;
    break;

    case OP_CLASS:
    case OP_NCLASS:
    space = get_class_iterator_size(cc + size);
    size = 1 + 32 / sizeof(PCRE2_UCHAR);
    break;

#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
    case OP_XCLASS:
    space = get_class_iterator_size(cc + size);
    size = GET(cc, 1);
    break;
#endif

    default:
    cc = next_opcode(common, cc);
    SLJIT_ASSERT(cc != NULL);
    break;
    }

  /* Character iterators, which are not inside a repeated bracket,
     gets a private slot instead of allocating it on the stack. */
  if (space > 0 && cc >= end)
    {
    common->private_data_ptrs[cc - common->start] = private_data_ptr;
    private_data_ptr += sizeof(sljit_sw) * space;
    }

  if (size != 0)
    {
    if (size < 0)
      {
      cc += -size;
#ifdef SUPPORT_UNICODE
      if (common->utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
      }
    else
      cc += size;
    }

  if (bracketlen > 0)
    {
    if (cc >= end)
      {
      end = bracketend(cc);
      if (end[-1 - LINK_SIZE] == OP_KET)
        end = NULL;
      }
    cc += bracketlen;
    }
  }
*private_data_start = private_data_ptr;
}