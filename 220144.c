ushort *LibRaw::ljpeg_row(int jrow, struct jhead *jh)
{
  int col, c, diff, pred, spred = 0;
  ushort mark = 0, *row[3];

  if (jrow * jh->wide % jh->restart == 0)
  {
    FORC(6) jh->vpred[c] = 1 << (jh->bits - 1);
    if (jrow)
    {
      fseek(ifp, -2, SEEK_CUR);
      do
        mark = (mark << 8) + (c = fgetc(ifp));
      while (c != EOF && mark >> 4 != 0xffd);
    }
    getbits(-1);
  }
  FORC3 row[c] = jh->row + jh->wide * jh->clrs * ((jrow + c) & 1);
  for (col = 0; col < jh->wide; col++)
    FORC(jh->clrs)
    {
      diff = ljpeg_diff(jh->huff[c]);
      if (jh->sraw && c <= jh->sraw && (col | c))
        pred = spred;
      else if (col)
        pred = row[0][-jh->clrs];
      else
        pred = (jh->vpred[c] += diff) - diff;
      if (jrow && col)
        switch (jh->psv)
        {
        case 1:
          break;
        case 2:
          pred = row[1][0];
          break;
        case 3:
          pred = row[1][-jh->clrs];
          break;
        case 4:
          pred = pred + row[1][0] - row[1][-jh->clrs];
          break;
        case 5:
          pred = pred + ((row[1][0] - row[1][-jh->clrs]) >> 1);
          break;
        case 6:
          pred = row[1][0] + ((pred - row[1][-jh->clrs]) >> 1);
          break;
        case 7:
          pred = (pred + row[1][0]) >> 1;
          break;
        default:
          pred = 0;
        }
      if ((**row = pred + diff) >> jh->bits)
        derror();
      if (c <= jh->sraw)
        spred = **row;
      row[0]++;
      row[1]++;
    }
  return row[2];
}