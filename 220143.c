void LibRaw::canon_rmf_load_raw()
{
  int row, col, bits, orow, ocol, c;

  int *words = (int *)malloc(sizeof(int) * (raw_width / 3 + 1));
  merror(words, "canon_rmf_load_raw");
  for (row = 0; row < raw_height; row++)
  {
    checkCancel();
    fread(words, sizeof(int), raw_width / 3, ifp);
    for (col = 0; col < raw_width - 2; col += 3)
    {
      bits = words[col / 3];
      FORC3
      {
        orow = row;
        if ((ocol = col + c - 4) < 0)
        {
          ocol += raw_width;
          if ((orow -= 2) < 0)
            orow += raw_height;
        }
        RAW(orow, ocol) = curve[bits >> (10 * c + 2) & 0x3ff];
      }
    }
  }
  free(words);
  maximum = curve[0x3ff];
}