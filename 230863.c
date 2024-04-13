static PCRE2_SPTR SLJIT_FUNC do_extuni_utf(jit_arguments *args, PCRE2_SPTR cc)
{
PCRE2_SPTR start_subject = args->begin;
PCRE2_SPTR end_subject = args->end;
int lgb, rgb, len, ricount;
PCRE2_SPTR prevcc, bptr;
uint32_t c;

prevcc = cc;
GETCHARINC(c, cc);
lgb = UCD_GRAPHBREAK(c);

while (cc < end_subject)
  {
  len = 1;
  GETCHARLEN(c, cc, len);
  rgb = UCD_GRAPHBREAK(c);

  if ((PRIV(ucp_gbtable)[lgb] & (1 << rgb)) == 0) break;

  /* Not breaking between Regional Indicators is allowed only if there
  are an even number of preceding RIs. */

  if (lgb == ucp_gbRegionalIndicator && rgb == ucp_gbRegionalIndicator)
    {
    ricount = 0;
    bptr = prevcc;

    /* bptr is pointing to the left-hand character */
    while (bptr > start_subject)
      {
      bptr--;
      BACKCHAR(bptr);
      GETCHAR(c, bptr);

      if (UCD_GRAPHBREAK(c) != ucp_gbRegionalIndicator) break;

      ricount++;
      }

    if ((ricount & 1) != 0) break;  /* Grapheme break required */
    }

  /* If Extend or ZWJ follows Extended_Pictographic, do not update lgb; this
  allows any number of them before a following Extended_Pictographic. */

  if ((rgb != ucp_gbExtend && rgb != ucp_gbZWJ) ||
       lgb != ucp_gbExtended_Pictographic)
    lgb = rgb;

  prevcc = cc;
  cc += len;
  }

return cc;
}