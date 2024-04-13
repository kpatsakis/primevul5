void LibRaw::lossless_jpeg_load_raw()
{
  int jwide, jhigh, jrow, jcol, val, jidx, i, j, row = 0, col = 0;
  struct jhead jh;
  ushort *rp;

  if (!ljpeg_start(&jh, 0))
    return;

  if (jh.wide < 1 || jh.high < 1 || jh.clrs < 1 || jh.bits < 1)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  jwide = jh.wide * jh.clrs;
  jhigh = jh.high;
  if (jh.clrs == 4 && jwide >= raw_width * 2)
    jhigh *= 2;

  try
  {
    for (jrow = 0; jrow < jh.high; jrow++)
    {
      checkCancel();
      rp = ljpeg_row(jrow, &jh);
      if (load_flags & 1)
        row = jrow & 1 ? height - 1 - jrow / 2 : jrow / 2;
      for (jcol = 0; jcol < jwide; jcol++)
      {
        val = curve[*rp++];
        if (cr2_slice[0])
        {
          jidx = jrow * jwide + jcol;
          i = jidx / (cr2_slice[1] * raw_height);
          if ((j = i >= cr2_slice[0]))
            i = cr2_slice[0];
          jidx -= i * (cr2_slice[1] * raw_height);
          row = jidx / cr2_slice[1 + j];
          col = jidx % cr2_slice[1 + j] + i * cr2_slice[1];
        }
        if (raw_width == 3984 && (col -= 2) < 0)
          col += (row--, raw_width);
        if (row > raw_height)
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
        if ((unsigned)row < raw_height)
          RAW(row, col) = val;
        if (++col >= raw_width)
          col = (row++, 0);
      }
    }
  }
  catch (...)
  {
    ljpeg_end(&jh);
    throw;
  }
  ljpeg_end(&jh);
}