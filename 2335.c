static int detect_early_fail(compiler_common *common, PCRE2_SPTR cc, int *private_data_start,
   sljit_s32 depth, int start, BOOL fast_forward_allowed)
{
PCRE2_SPTR begin = cc;
PCRE2_SPTR next_alt;
PCRE2_SPTR end;
PCRE2_SPTR accelerated_start;
BOOL prev_fast_forward_allowed;
int result = 0;
int count;

SLJIT_ASSERT(*cc == OP_ONCE || *cc == OP_BRA || *cc == OP_CBRA);
SLJIT_ASSERT(*cc != OP_CBRA || common->optimized_cbracket[GET2(cc, 1 + LINK_SIZE)] != 0);
SLJIT_ASSERT(start < EARLY_FAIL_ENHANCE_MAX);

next_alt = cc + GET(cc, 1);
if (*next_alt == OP_ALT)
  fast_forward_allowed = FALSE;

do
  {
  count = start;
  cc += 1 + LINK_SIZE + ((*cc == OP_CBRA) ? IMM2_SIZE : 0);

  while (TRUE)
    {
    accelerated_start = NULL;

    switch(*cc)
      {
      case OP_SOD:
      case OP_SOM:
      case OP_SET_SOM:
      case OP_NOT_WORD_BOUNDARY:
      case OP_WORD_BOUNDARY:
      case OP_EODN:
      case OP_EOD:
      case OP_CIRC:
      case OP_CIRCM:
      case OP_DOLL:
      case OP_DOLLM:
      /* Zero width assertions. */
      cc++;
      continue;

      case OP_NOT_DIGIT:
      case OP_DIGIT:
      case OP_NOT_WHITESPACE:
      case OP_WHITESPACE:
      case OP_NOT_WORDCHAR:
      case OP_WORDCHAR:
      case OP_ANY:
      case OP_ALLANY:
      case OP_ANYBYTE:
      case OP_NOT_HSPACE:
      case OP_HSPACE:
      case OP_NOT_VSPACE:
      case OP_VSPACE:
      fast_forward_allowed = FALSE;
      cc++;
      continue;

      case OP_ANYNL:
      case OP_EXTUNI:
      fast_forward_allowed = FALSE;
      if (count == 0)
        count = 1;
      cc++;
      continue;

      case OP_NOTPROP:
      case OP_PROP:
      fast_forward_allowed = FALSE;
      cc += 1 + 2;
      continue;

      case OP_CHAR:
      case OP_CHARI:
      case OP_NOT:
      case OP_NOTI:
      fast_forward_allowed = FALSE;
      cc += 2;
#ifdef SUPPORT_UNICODE
      if (common->utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
      continue;

      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      case OP_TYPEPOSSTAR:
      case OP_TYPEPOSPLUS:
      /* The type or prop opcode is skipped in the next iteration. */
      cc += 1;

      if (cc[0] != OP_ANYNL && cc[0] != OP_EXTUNI)
        {
        accelerated_start = cc - 1;
        break;
        }

      if (count == 0)
        count = 1;
      fast_forward_allowed = FALSE;
      continue;

      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEEXACT:
      case OP_TYPEPOSUPTO:
      cc += IMM2_SIZE;
      /* Fall through */

      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSQUERY:
      /* The type or prop opcode is skipped in the next iteration. */
      fast_forward_allowed = FALSE;
      if (count == 0)
        count = 1;
      cc += 1;
      continue;

      case OP_STAR:
      case OP_MINSTAR:
      case OP_PLUS:
      case OP_MINPLUS:
      case OP_POSSTAR:
      case OP_POSPLUS:

      case OP_STARI:
      case OP_MINSTARI:
      case OP_PLUSI:
      case OP_MINPLUSI:
      case OP_POSSTARI:
      case OP_POSPLUSI:

      case OP_NOTSTAR:
      case OP_NOTMINSTAR:
      case OP_NOTPLUS:
      case OP_NOTMINPLUS:
      case OP_NOTPOSSTAR:
      case OP_NOTPOSPLUS:

      case OP_NOTSTARI:
      case OP_NOTMINSTARI:
      case OP_NOTPLUSI:
      case OP_NOTMINPLUSI:
      case OP_NOTPOSSTARI:
      case OP_NOTPOSPLUSI:
      accelerated_start = cc;
      cc += 2;
#ifdef SUPPORT_UNICODE
      if (common->utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
      break;

      case OP_UPTO:
      case OP_MINUPTO:
      case OP_EXACT:
      case OP_POSUPTO:
      case OP_UPTOI:
      case OP_MINUPTOI:
      case OP_EXACTI:
      case OP_POSUPTOI:
      case OP_NOTUPTO:
      case OP_NOTMINUPTO:
      case OP_NOTEXACT:
      case OP_NOTPOSUPTO:
      case OP_NOTUPTOI:
      case OP_NOTMINUPTOI:
      case OP_NOTEXACTI:
      case OP_NOTPOSUPTOI:
      cc += IMM2_SIZE;
      /* Fall through */

      case OP_QUERY:
      case OP_MINQUERY:
      case OP_POSQUERY:
      case OP_QUERYI:
      case OP_MINQUERYI:
      case OP_POSQUERYI:
      case OP_NOTQUERY:
      case OP_NOTMINQUERY:
      case OP_NOTPOSQUERY:
      case OP_NOTQUERYI:
      case OP_NOTMINQUERYI:
      case OP_NOTPOSQUERYI:
      fast_forward_allowed = FALSE;
      if (count == 0)
        count = 1;
      cc += 2;
#ifdef SUPPORT_UNICODE
      if (common->utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
      continue;

      case OP_CLASS:
      case OP_NCLASS:
#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
      case OP_XCLASS:
      accelerated_start = cc;
      cc += ((*cc == OP_XCLASS) ? GET(cc, 1) : (unsigned int)(1 + (32 / sizeof(PCRE2_UCHAR))));
#else
      accelerated_start = cc;
      cc += (1 + (32 / sizeof(PCRE2_UCHAR)));
#endif

      switch (*cc)
        {
        case OP_CRSTAR:
        case OP_CRMINSTAR:
        case OP_CRPLUS:
        case OP_CRMINPLUS:
        case OP_CRPOSSTAR:
        case OP_CRPOSPLUS:
        cc++;
        break;

        case OP_CRRANGE:
        case OP_CRMINRANGE:
        case OP_CRPOSRANGE:
        cc += 2 * IMM2_SIZE;
        /* Fall through */
        case OP_CRQUERY:
        case OP_CRMINQUERY:
        case OP_CRPOSQUERY:
        cc++;
        if (count == 0)
          count = 1;
        /* Fall through */
        default:
        accelerated_start = NULL;
        fast_forward_allowed = FALSE;
        continue;
        }
      break;

      case OP_ONCE:
      case OP_BRA:
      case OP_CBRA:
      end = cc + GET(cc, 1);

      prev_fast_forward_allowed = fast_forward_allowed;
      fast_forward_allowed = FALSE;
      if (depth >= 4)
        break;

      end = bracketend(cc) - (1 + LINK_SIZE);
      if (*end != OP_KET || (*cc == OP_CBRA && common->optimized_cbracket[GET2(cc, 1 + LINK_SIZE)] == 0))
        break;

      count = detect_early_fail(common, cc, private_data_start, depth + 1, count, prev_fast_forward_allowed);

      if (PRIVATE_DATA(cc) != 0)
        common->private_data_ptrs[begin - common->start] = 1;

      if (count < EARLY_FAIL_ENHANCE_MAX)
        {
        cc = end + (1 + LINK_SIZE);
        continue;
        }
      break;

      case OP_KET:
      SLJIT_ASSERT(PRIVATE_DATA(cc) == 0);
      if (cc >= next_alt)
        break;
      cc += 1 + LINK_SIZE;
      continue;
      }

    if (accelerated_start != NULL)
      {
      if (count == 0)
        {
        count++;

        if (fast_forward_allowed)
          {
          common->fast_forward_bc_ptr = accelerated_start;
          common->private_data_ptrs[(accelerated_start + 1) - common->start] = ((*private_data_start) << 3) | type_skip;
          *private_data_start += sizeof(sljit_sw);
          }
        else
          {
          common->private_data_ptrs[(accelerated_start + 1) - common->start] = ((*private_data_start) << 3) | type_fail;

          if (common->early_fail_start_ptr == 0)
            common->early_fail_start_ptr = *private_data_start;

          *private_data_start += sizeof(sljit_sw);
          common->early_fail_end_ptr = *private_data_start;

          if (*private_data_start > SLJIT_MAX_LOCAL_SIZE)
            return EARLY_FAIL_ENHANCE_MAX;
          }
        }
      else
        {
        common->private_data_ptrs[(accelerated_start + 1) - common->start] = ((*private_data_start) << 3) | type_fail_range;

        if (common->early_fail_start_ptr == 0)
          common->early_fail_start_ptr = *private_data_start;

        *private_data_start += 2 * sizeof(sljit_sw);
        common->early_fail_end_ptr = *private_data_start;

        if (*private_data_start > SLJIT_MAX_LOCAL_SIZE)
          return EARLY_FAIL_ENHANCE_MAX;
        }

      /* Cannot be part of a repeat. */
      common->private_data_ptrs[begin - common->start] = 1;
      count++;

      if (count < EARLY_FAIL_ENHANCE_MAX)
        continue;
      }

    break;
    }

  if (*cc != OP_ALT && *cc != OP_KET)
    result = EARLY_FAIL_ENHANCE_MAX;
  else if (result < count)
    result = count;

  cc = next_alt;
  next_alt = cc + GET(cc, 1);
  }
while (*cc == OP_ALT);

return result;
}