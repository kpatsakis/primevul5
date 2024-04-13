static SLJIT_INLINE BOOL char_has_othercase(compiler_common *common, PCRE2_SPTR cc)
{
/* Detects if the character has an othercase. */
unsigned int c;

#ifdef SUPPORT_UNICODE
if (common->utf)
  {
  GETCHAR(c, cc);
  if (c > 127)
    {
    return c != UCD_OTHERCASE(c);
    }
#if PCRE2_CODE_UNIT_WIDTH != 8
  return common->fcc[c] != c;
#endif
  }
else
#endif
  c = *cc;
return MAX_255(c) ? common->fcc[c] != c : FALSE;
}