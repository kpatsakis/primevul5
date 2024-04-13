static PCRE2_SPTR SLJIT_FUNC do_extuni_no_utf(jit_arguments *args, PCRE2_SPTR cc)
{
PCRE2_SPTR start_subject = args->begin;
PCRE2_SPTR end_subject = args->end;
int lgb, rgb, ricount;
PCRE2_SPTR bptr;
uint32_t c;

/* Patch by PH */
/* GETCHARINC(c, cc); */
c = *cc++;

#if PCRE2_CODE_UNIT_WIDTH == 32
if (c >= 0x110000)
  return NULL;
#endif /* PCRE2_CODE_UNIT_WIDTH == 32 */
lgb = UCD_GRAPHBREAK(c);

while (cc < end_subject)
  {
  c = *cc;
#if PCRE2_CODE_UNIT_WIDTH == 32
  if (c >= 0x110000)
    break;
#endif /* PCRE2_CODE_UNIT_WIDTH == 32 */
  rgb = UCD_GRAPHBREAK(c);

  if ((PRIV(ucp_gbtable)[lgb] & (1 << rgb)) == 0)
    break;

  /* Not breaking between Regional Indicators is allowed only if there
  are an even number of preceding RIs. */

  if (lgb == ucp_gbRegional_Indicator && rgb == ucp_gbRegional_Indicator)
    {
    ricount = 0;
    bptr = cc - 1;

    /* bptr is pointing to the left-hand character */
    while (bptr > start_subject)
      {
      bptr--;
      c = *bptr;
#if PCRE2_CODE_UNIT_WIDTH == 32
      if (c >= 0x110000)
        break;
#endif /* PCRE2_CODE_UNIT_WIDTH == 32 */

      if (UCD_GRAPHBREAK(c) != ucp_gbRegional_Indicator) break;

      ricount++;
      }

    if ((ricount & 1) != 0)
      break;  /* Grapheme break required */
    }

  /* If Extend or ZWJ follows Extended_Pictographic, do not update lgb; this
  allows any number of them before a following Extended_Pictographic. */

  if ((rgb != ucp_gbExtend && rgb != ucp_gbZWJ) ||
       lgb != ucp_gbExtended_Pictographic)
    lgb = rgb;

  cc++;
  }

return cc;
}