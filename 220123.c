void LibRaw::samsung2_load_raw()
{
  static const ushort tab[14] = {0x304, 0x307, 0x206, 0x205, 0x403,
                                 0x600, 0x709, 0x80a, 0x90b, 0xa0c,
                                 0xa0d, 0x501, 0x408, 0x402};
  ushort huff[1026], vpred[2][2] = {{0, 0}, {0, 0}}, hpred[2];
  int i, c, n, row, col, diff;

  huff[0] = 10;
  for (n = i = 0; i < 14; i++)
    FORC(1024 >> (tab[i] >> 8)) huff[++n] = tab[i];
  getbits(-1);
  for (row = 0; row < raw_height; row++)
  {
    checkCancel();
    for (col = 0; col < raw_width; col++)
    {
      diff = ljpeg_diff(huff);
      if (col < 2)
        hpred[col] = vpred[row & 1][col] += diff;
      else
        hpred[col & 1] += diff;
      RAW(row, col) = hpred[col & 1];
      if (hpred[col & 1] >> tiff_bps)
        derror();
    }
  }
}