unsigned LibRaw::pana_data(int nb, unsigned *bytes)
{
#ifndef LIBRAW_NOTHREADS
#define vpos tls->pana_data.vpos
#define buf tls->pana_data.buf
#else
  static uchar buf[0x4002];
  static int vpos;
#endif
  int byte;

  if (!nb && !bytes)
    return vpos = 0;

  if (!vpos)
  {
    fread(buf + load_flags, 1, 0x4000 - load_flags, ifp);
    fread(buf, 1, load_flags, ifp);
  }

  if (pana_encoding == 5)
  {
    for (byte = 0; byte < 16; byte++)
    {
      bytes[byte] = buf[vpos++];
      vpos &= 0x3FFF;
    }
  }
  else
  {
    vpos = (vpos - nb) & 0x1ffff;
    byte = vpos >> 3 ^ 0x3ff0;
    return (buf[byte] | buf[byte + 1] << 8) >> (vpos & 7) & ~((~0u) << nb);
  }
  return 0;
#ifndef LIBRAW_NOTHREADS
#undef vpos
#undef buf
#endif
}