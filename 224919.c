u32toutf16 (c, s)
     u_bits32_t c;
     unsigned short *s;
{
  int l;

  l = 0;
  if (c < 0x0d800)
    {
      s[0] = (unsigned short) (c & 0xFFFF);
      l = 1;
    }
  else if (c >= 0x0e000 && c <= 0x010ffff)
    {
      c -= 0x010000;
      s[0] = (unsigned short)((c >> 10) + 0xd800);
      s[1] = (unsigned short)((c & 0x3ff) + 0xdc00);
      l = 2;
    }
  s[l] = 0;
  return l;
}