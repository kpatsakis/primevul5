int LibRaw::ljpeg_diff(ushort *huff)
{
  int len, diff;
  if (!huff)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;

  len = gethuff(huff);
  if (len == 16 && (!dng_version || dng_version >= 0x1010000))
    return -32768;
  diff = getbits(len);
  if ((diff & (1 << (len - 1))) == 0)
    diff -= (1 << len) - 1;
  return diff;
}