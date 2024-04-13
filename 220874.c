data_addblob(struct extdata *xd, unsigned char *blob, int len)
{
  xd->buf = solv_extend(xd->buf, xd->len, len, 1, EXTDATA_BLOCK);
  memcpy(xd->buf + xd->len, blob, len);
  xd->len += len;
}