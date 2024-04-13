unsigned LibRaw::getbithuff(int nbits, ushort *huff)
{
#ifdef LIBRAW_NOTHREADS
  static unsigned bitbuf = 0;
  static int vbits = 0, reset = 0;
#else
#define bitbuf tls->getbits.bitbuf
#define vbits tls->getbits.vbits
#define reset tls->getbits.reset
#endif
  unsigned c;

  if (nbits > 25)
    return 0;
  if (nbits < 0)
    return bitbuf = vbits = reset = 0;
  if (nbits == 0 || vbits < 0)
    return 0;
  while (!reset && vbits < nbits && (c = fgetc(ifp)) != EOF &&
         !(reset = zero_after_ff && c == 0xff && fgetc(ifp)))
  {
    bitbuf = (bitbuf << 8) + (uchar)c;
    vbits += 8;
  }
  c = bitbuf << (32 - vbits) >> (32 - nbits);
  if (huff)
  {
    vbits -= huff[c] >> 8;
    c = (uchar)huff[c];
  }
  else
    vbits -= nbits;
  if (vbits < 0)
    derror();
  return c;
#ifndef LIBRAW_NOTHREADS
#undef bitbuf
#undef vbits
#undef reset
#endif
}