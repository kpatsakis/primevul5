data_addideof(struct extdata *xd, Id sx, int eof)
{
  unsigned int x = (unsigned int)sx;
  unsigned char *dp;

  xd->buf = solv_extend(xd->buf, xd->len, 5, 1, EXTDATA_BLOCK);
  dp = xd->buf + xd->len;

  if (x >= (1 << 13))
    {
      if (x >= (1 << 27))
        *dp++ = (x >> 27) | 128;
      if (x >= (1 << 20))
        *dp++ = (x >> 20) | 128;
      *dp++ = (x >> 13) | 128;
    }
  if (x >= (1 << 6))
    *dp++ = (x >> 6) | 128;
  *dp++ = eof ? (x & 63) : (x & 63) | 64;
  xd->len = dp - xd->buf;
}