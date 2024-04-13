static SLJIT_INLINE PCRE2_SPTR compile_charn_matchingpath(compiler_common *common, PCRE2_SPTR cc, PCRE2_SPTR ccend, jump_list **backtracks)
{
/* This function consumes at least one input character. */
/* To decrease the number of length checks, we try to concatenate the fixed length character sequences. */
DEFINE_COMPILER;
PCRE2_SPTR ccbegin = cc;
compare_context context;
int size;

context.length = 0;
do
  {
  if (cc >= ccend)
    break;

  if (*cc == OP_CHAR)
    {
    size = 1;
#ifdef SUPPORT_UNICODE
    if (common->utf && HAS_EXTRALEN(cc[1]))
      size += GET_EXTRALEN(cc[1]);
#endif
    }
  else if (*cc == OP_CHARI)
    {
    size = 1;
#ifdef SUPPORT_UNICODE
    if (common->utf)
      {
      if (char_has_othercase(common, cc + 1) && char_get_othercase_bit(common, cc + 1) == 0)
        size = 0;
      else if (HAS_EXTRALEN(cc[1]))
        size += GET_EXTRALEN(cc[1]);
      }
    else
#endif
    if (char_has_othercase(common, cc + 1) && char_get_othercase_bit(common, cc + 1) == 0)
      size = 0;
    }
  else
    size = 0;

  cc += 1 + size;
  context.length += IN_UCHARS(size);
  }
while (size > 0 && context.length <= 128);

cc = ccbegin;
if (context.length > 0)
  {
  /* We have a fixed-length byte sequence. */
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, context.length);
  add_jump(compiler, backtracks, CMP(SLJIT_GREATER, STR_PTR, 0, STR_END, 0));

  context.sourcereg = -1;
#if defined SLJIT_UNALIGNED && SLJIT_UNALIGNED
  context.ucharptr = 0;
#endif
  do cc = byte_sequence_compare(common, *cc == OP_CHARI, cc + 1, &context, backtracks); while (context.length > 0);
  return cc;
  }

/* A non-fixed length character will be checked if length == 0. */
return compile_char1_matchingpath(common, *cc, cc + 1, backtracks, TRUE);
}