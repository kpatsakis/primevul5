static int scan_prefix(compiler_common *common, PCRE2_SPTR cc, fast_forward_char_data *chars, int max_chars, sljit_u32 *rec_count)
{
/* Recursive function, which scans prefix literals. */
BOOL last, any, class, caseless;
int len, repeat, len_save, consumed = 0;
sljit_u32 chr; /* Any unicode character. */
sljit_u8 *bytes, *bytes_end, byte;
PCRE2_SPTR alternative, cc_save, oc;
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
PCRE2_UCHAR othercase[4];
#elif defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 16
PCRE2_UCHAR othercase[2];
#else
PCRE2_UCHAR othercase[1];
#endif

repeat = 1;
while (TRUE)
  {
  if (*rec_count == 0)
    return 0;
  (*rec_count)--;

  last = TRUE;
  any = FALSE;
  class = FALSE;
  caseless = FALSE;

  switch (*cc)
    {
    case OP_CHARI:
    caseless = TRUE;
    /* Fall through */
    case OP_CHAR:
    last = FALSE;
    cc++;
    break;

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

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    case OP_ASSERT_NA:
    case OP_ASSERTBACK_NA:
    cc = bracketend(cc);
    continue;

    case OP_PLUSI:
    case OP_MINPLUSI:
    case OP_POSPLUSI:
    caseless = TRUE;
    /* Fall through */
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_POSPLUS:
    cc++;
    break;

    case OP_EXACTI:
    caseless = TRUE;
    /* Fall through */
    case OP_EXACT:
    repeat = GET2(cc, 1);
    last = FALSE;
    cc += 1 + IMM2_SIZE;
    break;

    case OP_QUERYI:
    case OP_MINQUERYI:
    case OP_POSQUERYI:
    caseless = TRUE;
    /* Fall through */
    case OP_QUERY:
    case OP_MINQUERY:
    case OP_POSQUERY:
    len = 1;
    cc++;
#ifdef SUPPORT_UNICODE
    if (common->utf && HAS_EXTRALEN(*cc)) len += GET_EXTRALEN(*cc);
#endif
    max_chars = scan_prefix(common, cc + len, chars, max_chars, rec_count);
    if (max_chars == 0)
      return consumed;
    last = FALSE;
    break;

    case OP_KET:
    cc += 1 + LINK_SIZE;
    continue;

    case OP_ALT:
    cc += GET(cc, 1);
    continue;

    case OP_ONCE:
    case OP_BRA:
    case OP_BRAPOS:
    case OP_CBRA:
    case OP_CBRAPOS:
    alternative = cc + GET(cc, 1);
    while (*alternative == OP_ALT)
      {
      max_chars = scan_prefix(common, alternative + 1 + LINK_SIZE, chars, max_chars, rec_count);
      if (max_chars == 0)
        return consumed;
      alternative += GET(alternative, 1);
      }

    if (*cc == OP_CBRA || *cc == OP_CBRAPOS)
      cc += IMM2_SIZE;
    cc += 1 + LINK_SIZE;
    continue;

    case OP_CLASS:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
    if (common->utf && !is_char7_bitset((const sljit_u8 *)(cc + 1), FALSE))
      return consumed;
#endif
    class = TRUE;
    break;

    case OP_NCLASS:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf) return consumed;
#endif
    class = TRUE;
    break;

#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
    case OP_XCLASS:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf) return consumed;
#endif
    any = TRUE;
    cc += GET(cc, 1);
    break;
#endif

    case OP_DIGIT:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
    if (common->utf && !is_char7_bitset((const sljit_u8 *)common->ctypes - cbit_length + cbit_digit, FALSE))
      return consumed;
#endif
    any = TRUE;
    cc++;
    break;

    case OP_WHITESPACE:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
    if (common->utf && !is_char7_bitset((const sljit_u8 *)common->ctypes - cbit_length + cbit_space, FALSE))
      return consumed;
#endif
    any = TRUE;
    cc++;
    break;

    case OP_WORDCHAR:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH == 8
    if (common->utf && !is_char7_bitset((const sljit_u8 *)common->ctypes - cbit_length + cbit_word, FALSE))
      return consumed;
#endif
    any = TRUE;
    cc++;
    break;

    case OP_NOT:
    case OP_NOTI:
    cc++;
    /* Fall through. */
    case OP_NOT_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf) return consumed;
#endif
    any = TRUE;
    cc++;
    break;

#ifdef SUPPORT_UNICODE
    case OP_NOTPROP:
    case OP_PROP:
#if PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf) return consumed;
#endif
    any = TRUE;
    cc += 1 + 2;
    break;
#endif

    case OP_TYPEEXACT:
    repeat = GET2(cc, 1);
    cc += 1 + IMM2_SIZE;
    continue;

    case OP_NOTEXACT:
    case OP_NOTEXACTI:
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 32
    if (common->utf) return consumed;
#endif
    any = TRUE;
    repeat = GET2(cc, 1);
    cc += 1 + IMM2_SIZE + 1;
    break;

    default:
    return consumed;
    }

  if (any)
    {
    do
      {
      chars->count = 255;

      consumed++;
      if (--max_chars == 0)
        return consumed;
      chars++;
      }
    while (--repeat > 0);

    repeat = 1;
    continue;
    }

  if (class)
    {
    bytes = (sljit_u8*) (cc + 1);
    cc += 1 + 32 / sizeof(PCRE2_UCHAR);

    switch (*cc)
      {
      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRPOSSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      case OP_CRPOSQUERY:
      max_chars = scan_prefix(common, cc + 1, chars, max_chars, rec_count);
      if (max_chars == 0)
        return consumed;
      break;

      default:
      case OP_CRPLUS:
      case OP_CRMINPLUS:
      case OP_CRPOSPLUS:
      break;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      case OP_CRPOSRANGE:
      repeat = GET2(cc, 1);
      if (repeat <= 0)
        return consumed;
      break;
      }

    do
      {
      if (bytes[31] & 0x80)
        chars->count = 255;
      else if (chars->count != 255)
        {
        bytes_end = bytes + 32;
        chr = 0;
        do
          {
          byte = *bytes++;
          SLJIT_ASSERT((chr & 0x7) == 0);
          if (byte == 0)
            chr += 8;
          else
            {
            do
              {
              if ((byte & 0x1) != 0)
                add_prefix_char(chr, chars, TRUE);
              byte >>= 1;
              chr++;
              }
            while (byte != 0);
            chr = (chr + 7) & ~7;
            }
          }
        while (chars->count != 255 && bytes < bytes_end);
        bytes = bytes_end - 32;
        }

      consumed++;
      if (--max_chars == 0)
        return consumed;
      chars++;
      }
    while (--repeat > 0);

    switch (*cc)
      {
      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRPOSSTAR:
      return consumed;

      case OP_CRQUERY:
      case OP_CRMINQUERY:
      case OP_CRPOSQUERY:
      cc++;
      break;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      case OP_CRPOSRANGE:
      if (GET2(cc, 1) != GET2(cc, 1 + IMM2_SIZE))
        return consumed;
      cc += 1 + 2 * IMM2_SIZE;
      break;
      }

    repeat = 1;
    continue;
    }

  len = 1;
#ifdef SUPPORT_UNICODE
  if (common->utf && HAS_EXTRALEN(*cc)) len += GET_EXTRALEN(*cc);
#endif

  if (caseless && char_has_othercase(common, cc))
    {
#ifdef SUPPORT_UNICODE
    if (common->utf)
      {
      GETCHAR(chr, cc);
      if ((int)PRIV(ord2utf)(char_othercase(common, chr), othercase) != len)
        return consumed;
      }
    else
#endif
      {
      chr = *cc;
#ifdef SUPPORT_UNICODE
      if (common->ucp && chr > 127)
        othercase[0] = UCD_OTHERCASE(chr);
      else
#endif
        othercase[0] = TABLE_GET(chr, common->fcc, chr);
      }
    }
  else
    {
    caseless = FALSE;
    othercase[0] = 0; /* Stops compiler warning - PH */
    }

  len_save = len;
  cc_save = cc;
  while (TRUE)
    {
    oc = othercase;
    do
      {
      len--;
      consumed++;

      chr = *cc;
      add_prefix_char(*cc, chars, len == 0);

      if (caseless)
        add_prefix_char(*oc, chars, len == 0);

      if (--max_chars == 0)
        return consumed;
      chars++;
      cc++;
      oc++;
      }
    while (len > 0);

    if (--repeat == 0)
      break;

    len = len_save;
    cc = cc_save;
    }

  repeat = 1;
  if (last)
    return consumed;
  }
}