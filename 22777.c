uniname2ctype(const UChar *name, unsigned int len)
{
  const struct uniname2ctype_struct *p = uniname2ctype_p((const char *)name, len);
  if (p) return p->ctype;
  return -1;
}