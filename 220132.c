void LibRaw::pentax_load_raw()
{
  ushort bit[2][15], huff[4097];
  int dep, row, col, diff, c, i;
  ushort vpred[2][2] = {{0, 0}, {0, 0}}, hpred[2];

  fseek(ifp, meta_offset, SEEK_SET);
  dep = (get2() + 12) & 15;
  fseek(ifp, 12, SEEK_CUR);
  FORC(dep) bit[0][c] = get2();
  FORC(dep) bit[1][c] = fgetc(ifp);
  FORC(dep)
  for (i = bit[0][c]; i <= ((bit[0][c] + (4096 >> bit[1][c]) - 1) & 4095);)
    huff[++i] = bit[1][c] << 8 | c;
  huff[0] = 12;
  fseek(ifp, data_offset, SEEK_SET);
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