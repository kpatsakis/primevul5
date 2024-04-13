static unsigned int char_get_othercase_bit(compiler_common *common, PCRE2_SPTR cc)
{
/* Detects if the character and its othercase has only 1 bit difference. */
unsigned int c, oc, bit;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
int n;
#endif

#ifdef SUPPORT_UNICODE
if (common->utf || common->ucp)
  {
  if (common->utf)
    {
    GETCHAR(c, cc);
    }
  else
    c = *cc;

  if (c <= 127)
    oc = common->fcc[c];
  else
    oc = UCD_OTHERCASE(c);
  }
else
  {
  c = *cc;
  oc = TABLE_GET(c, common->fcc, c);
  }
#else
c = *cc;
oc = TABLE_GET(c, common->fcc, c);
#endif

SLJIT_ASSERT(c != oc);

bit = c ^ oc;
/* Optimized for English alphabet. */
if (c <= 127 && bit == 0x20)
  return (0 << 8) | 0x20;

/* Since c != oc, they must have at least 1 bit difference. */
if (!is_powerof2(bit))
  return 0;

#if PCRE2_CODE_UNIT_WIDTH == 8

#ifdef SUPPORT_UNICODE
if (common->utf && c > 127)
  {
  n = GET_EXTRALEN(*cc);
  while ((bit & 0x3f) == 0)
    {
    n--;
    bit >>= 6;
    }
  return (n << 8) | bit;
  }
#endif /* SUPPORT_UNICODE */
return (0 << 8) | bit;

#elif PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32

#ifdef SUPPORT_UNICODE
if (common->utf && c > 65535)
  {
  if (bit >= (1u << 10))
    bit >>= 10;
  else
    return (bit < 256) ? ((2 << 8) | bit) : ((3 << 8) | (bit >> 8));
  }
#endif /* SUPPORT_UNICODE */
return (bit < 256) ? ((0u << 8) | bit) : ((1u << 8) | (bit >> 8));

#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16|32] */
}