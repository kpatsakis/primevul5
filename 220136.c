void LibRaw::samsung_load_raw()
{
  int row, col, c, i, dir, op[4], len[4];
  if (raw_width > 32768 ||
      raw_height > 32768) // definitely too much for old samsung
    throw LIBRAW_EXCEPTION_IO_BADFILE;
  unsigned maxpixels = raw_width * (raw_height + 7);

  order = 0x4949;
  for (row = 0; row < raw_height; row++)
  {
    checkCancel();
    fseek(ifp, strip_offset + row * 4, SEEK_SET);
    fseek(ifp, data_offset + get4(), SEEK_SET);
    ph1_bits(-1);
    FORC4 len[c] = row < 2 ? 7 : 4;
    for (col = 0; col < raw_width; col += 16)
    {
      dir = ph1_bits(1);
      FORC4 op[c] = ph1_bits(2);
      FORC4 switch (op[c])
      {
      case 3:
        len[c] = ph1_bits(4);
        break;
      case 2:
        len[c]--;
        break;
      case 1:
        len[c]++;
      }
      for (c = 0; c < 16; c += 2)
      {
        i = len[((c & 1) << 1) | (c >> 3)];
        unsigned idest = RAWINDEX(row, col + c);
        unsigned isrc = (dir ? RAWINDEX(row + (~c | -2), col + c)
                             : col ? RAWINDEX(row, col + (c | -2)) : 0);
        if (idest < maxpixels &&
            isrc <
                maxpixels) // less than zero is handled by unsigned conversion
          RAW(row, col + c) = ((signed)ph1_bits(i) << (32 - i) >> (32 - i)) +
                              (dir ? RAW(row + (~c | -2), col + c)
                                   : col ? RAW(row, col + (c | -2)) : 128);
        else
          derror();
        if (c == 14)
          c = -1;
      }
    }
  }
  for (row = 0; row < raw_height - 1; row += 2)
    for (col = 0; col < raw_width - 1; col += 2)
      SWAP(RAW(row, col + 1), RAW(row + 1, col));
}