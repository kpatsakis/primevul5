int LibRaw::canon_has_lowbits()
{
  uchar test[0x4000];
  int ret = 1, i;

  fseek(ifp, 0, SEEK_SET);
  fread(test, 1, sizeof test, ifp);
  for (i = 540; i < sizeof test - 1; i++)
    if (test[i] == 0xff)
    {
      if (test[i + 1])
        return 1;
      ret = 0;
    }
  return ret;
}