static PCRE2_SPTR SLJIT_FUNC do_extuni_utf_invalid(jit_arguments *args, PCRE2_SPTR cc)
{
PCRE2_SPTR start_subject = args->begin;
PCRE2_SPTR end_subject = args->end;
int lgb, rgb, ricount;
PCRE2_SPTR prevcc, endcc, bptr;
BOOL first = TRUE;
uint32_t c;

prevcc = cc;
endcc = NULL;
do
  {
  GETCHARINC_INVALID(c, cc, end_subject, break);
  rgb = UCD_GRAPHBREAK(c);

  if (first)
    {
    lgb = rgb;
    endcc = cc;
    first = FALSE;
    continue;
    }

  if ((PRIV(ucp_gbtable)[lgb] & (1 << rgb)) == 0)
    break;

  /* Not breaking between Regional Indicators is allowed only if there
  are an even number of preceding RIs. */

  if (lgb == ucp_gbRegional_Indicator && rgb == ucp_gbRegional_Indicator)
    {
    ricount = 0;
    bptr = prevcc;

    /* bptr is pointing to the left-hand character */
    while (bptr > start_subject)
      {
      GETCHARBACK_INVALID(c, bptr, start_subject, break);

      if (UCD_GRAPHBREAK(c) != ucp_gbRegional_Indicator)
        break;

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

  prevcc = endcc;
  endcc = cc;
  }
while (cc < end_subject);

return endcc;
}