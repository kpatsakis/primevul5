static SLJIT_INLINE BOOL detect_fast_forward_skip(compiler_common *common, int *private_data_start)
{
PCRE2_SPTR cc = common->start;
PCRE2_SPTR end;

/* Skip not repeated brackets. */
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

  if (*cc != OP_BRA && *cc != OP_CBRA)
    break;

  end = cc + GET(cc, 1);
  if (*end != OP_KET || PRIVATE_DATA(end) != 0)
    return FALSE;
  if (*cc == OP_CBRA)
    {
    if (common->optimized_cbracket[GET2(cc, 1 + LINK_SIZE)] == 0)
      return FALSE;
    cc += IMM2_SIZE;
    }
  cc += 1 + LINK_SIZE;
  }

if (is_accelerated_repeat(cc))
  {
  common->fast_forward_bc_ptr = cc;
  common->private_data_ptrs[(cc + 1) - common->start] = *private_data_start;
  *private_data_start += sizeof(sljit_sw);
  return TRUE;
  }
return FALSE;
}