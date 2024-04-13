onigenc_unicode_property_name_to_ctype(OnigEncoding enc, UChar* name, UChar* end)
{
  int len;
  int ctype;
  UChar buf[PROPERTY_NAME_MAX_SIZE];
  UChar *p;
  OnigCodePoint code;

  len = 0;
  for (p = name; p < end; p += enclen(enc, p, end)) {
    code = ONIGENC_MBC_TO_CODE(enc, p, end);
    if (code == ' ' || code == '-' || code == '_')
      continue;
    if (code >= 0x80)
      return ONIGERR_INVALID_CHAR_PROPERTY_NAME;

    buf[len++] = (UChar )TOLOWER((unsigned char)code);
    if (len >= PROPERTY_NAME_MAX_SIZE)
      return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
  }

  buf[len] = 0;

  if ((ctype = uniname2ctype(buf, len)) < 0) {
    return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
  }

  return ctype;
}