data_addid64(struct extdata *xd, unsigned long long x)
{
  if (x >= 0x100000000)
    {
      if ((x >> 35) != 0)
	{
	  data_addid(xd, (Id)(x >> 35));
	  xd->buf[xd->len - 1] |= 128;
	}
      data_addid(xd, (Id)((unsigned int)x | 0x80000000));
      xd->buf[xd->len - 5] = (x >> 28) | 128;
    }
  else
    data_addid(xd, (Id)x);
}