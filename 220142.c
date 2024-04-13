void LibRaw::samsung3_load_raw()
{
  int opt, init, mag, pmode, row, tab, col, pred, diff, i, c;
  ushort lent[3][2], len[4], *prow[2];
  order = 0x4949;
  fseek(ifp, 9, SEEK_CUR);
  opt = fgetc(ifp);
  init = (get2(), get2());
  for (row = 0; row < raw_height; row++)
  {
    checkCancel();
    fseek(ifp, (data_offset - ftell(ifp)) & 15, SEEK_CUR);
    ph1_bits(-1);
    mag = 0;
    pmode = 7;
    FORC(6)((ushort *)lent)[c] = row < 2 ? 7 : 4;
    prow[row & 1] = &RAW(row - 1, 1 - ((row & 1) << 1)); // green
    prow[~row & 1] = &RAW(row - 2, 0);                   // red and blue
    for (tab = 0; tab + 15 < raw_width; tab += 16)
    {
      if (~opt & 4 && !(tab & 63))
      {
        i = ph1_bits(2);
        mag = i < 3 ? mag - '2' + "204"[i] : ph1_bits(12);
      }
      if (opt & 2)
        pmode = 7 - 4 * ph1_bits(1);
      else if (!ph1_bits(1))
        pmode = ph1_bits(3);
      if (opt & 1 || !ph1_bits(1))
      {
        FORC4 len[c] = ph1_bits(2);
        FORC4
        {
          i = ((row & 1) << 1 | (c & 1)) % 3;
          if (i < 0)
            throw LIBRAW_EXCEPTION_IO_CORRUPT;
          len[c] = len[c] < 3 ? lent[i][0] - '1' + "120"[len[c]] : ph1_bits(4);
          lent[i][0] = lent[i][1];
          lent[i][1] = len[c];
        }
      }
      FORC(16)
      {
        col = tab + (((c & 7) << 1) ^ (c >> 3) ^ (row & 1));
        if (col < 0)
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
        if (pmode < 0)
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
        if (pmode != 7 && row >= 2 && (col - '4' + "0224468"[pmode]) < 0)
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
        pred = (pmode == 7 || row < 2)
                   ? (tab ? RAW(row, tab - 2 + (col & 1)) : init)
                   : (prow[col & 1][col - '4' + "0224468"[pmode]] +
                      prow[col & 1][col - '4' + "0244668"[pmode]] + 1) >>
                         1;
        diff = ph1_bits(i = len[c >> 2]);
        if (diff >> (i - 1))
          diff -= 1 << i;
        diff = diff * (mag * 2 + 1) + mag;
        RAW(row, col) = pred + diff;
      }
    }
  }
}