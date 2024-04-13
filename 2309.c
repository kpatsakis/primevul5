static SLJIT_INLINE BOOL assert_needs_str_ptr_saving(PCRE2_SPTR cc)
{
while (TRUE)
  {
  switch (*cc)
    {
    case OP_CALLOUT_STR:
    cc += GET(cc, 1 + 2*LINK_SIZE);
    break;

    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
    case OP_CIRC:
    case OP_CIRCM:
    case OP_DOLL:
    case OP_DOLLM:
    case OP_CALLOUT:
    case OP_ALT:
    cc += PRIV(OP_lengths)[*cc];
    break;

    case OP_KET:
    return FALSE;

    default:
    return TRUE;
    }
  }
}