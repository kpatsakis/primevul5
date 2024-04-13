static SLJIT_INLINE void detect_fast_fail(compiler_common *common, PCRE2_SPTR cc, int *private_data_start, sljit_s32 depth)
{
  PCRE2_SPTR next_alt;

  SLJIT_ASSERT(*cc == OP_BRA || *cc == OP_CBRA);

  if (*cc == OP_CBRA && common->optimized_cbracket[GET2(cc, 1 + LINK_SIZE)] == 0)
    return;

  next_alt = bracketend(cc) - (1 + LINK_SIZE);
  if (*next_alt != OP_KET || PRIVATE_DATA(next_alt) != 0)
    return;

  do
    {
    next_alt = cc + GET(cc, 1);

    cc += 1 + LINK_SIZE + ((*cc == OP_CBRA) ? IMM2_SIZE : 0);

    while (TRUE)
      {
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
        }
      break;
      }

    if (depth > 0 && (*cc == OP_BRA || *cc == OP_CBRA))
      detect_fast_fail(common, cc, private_data_start, depth - 1);

    if (is_accelerated_repeat(cc))
      {
      common->private_data_ptrs[(cc + 1) - common->start] = *private_data_start;

      if (common->fast_fail_start_ptr == 0)
        common->fast_fail_start_ptr = *private_data_start;

      *private_data_start += sizeof(sljit_sw);
      common->fast_fail_end_ptr = *private_data_start;

      if (*private_data_start > SLJIT_MAX_LOCAL_SIZE)
        return;
      }

    cc = next_alt;
    }
  while (*cc == OP_ALT);
}