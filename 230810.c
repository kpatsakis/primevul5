static PCRE2_SPTR SLJIT_FUNC do_utf_caselesscmp(PCRE2_SPTR src1, PCRE2_SPTR src2, PCRE2_SPTR end1, PCRE2_SPTR end2)
{
/* This function would be ineffective to do in JIT level. */
sljit_u32 c1, c2;
const ucd_record *ur;
const sljit_u32 *pp;

while (src1 < end1)
  {
  if (src2 >= end2)
    return (PCRE2_SPTR)1;
  GETCHARINC(c1, src1);
  GETCHARINC(c2, src2);
  ur = GET_UCD(c2);
  if (c1 != c2 && c1 != c2 + ur->other_case)
    {
    pp = PRIV(ucd_caseless_sets) + ur->caseset;
    for (;;)
      {
      if (c1 < *pp) return NULL;
      if (c1 == *pp++) break;
      }
    }
  }
return src2;
}