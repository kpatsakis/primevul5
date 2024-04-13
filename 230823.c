static SLJIT_INLINE void fast_forward_first_char(compiler_common *common)
{
PCRE2_UCHAR first_char = (PCRE2_UCHAR)(common->re->first_codeunit);
PCRE2_UCHAR oc;

oc = first_char;
if ((common->re->flags & PCRE2_FIRSTCASELESS) != 0)
  {
  oc = TABLE_GET(first_char, common->fcc, first_char);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 8
  if (first_char > 127 && common->utf)
    oc = UCD_OTHERCASE(first_char);
#endif
  }

fast_forward_first_char2(common, first_char, oc, 0);
}