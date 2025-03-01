void LibRaw::canon_load_raw()
{
  ushort *pixel, *prow, *huff[2];
  int nblocks, lowbits, i, c, row, r, save, val;
  int block, diffbuf[64], leaf, len, diff, carry = 0, pnum = 0, base[2];

  crw_init_tables(tiff_compress, huff);
  lowbits = canon_has_lowbits();
  if (!lowbits)
    maximum = 0x3ff;
  fseek(ifp, 540 + lowbits * raw_height * raw_width / 4, SEEK_SET);
  zero_after_ff = 1;
  getbits(-1);
  try
  {
    for (row = 0; row < raw_height; row += 8)
    {
      checkCancel();
      pixel = raw_image + row * raw_width;
      nblocks = MIN(8, raw_height - row) * raw_width >> 6;
      for (block = 0; block < nblocks; block++)
      {
        memset(diffbuf, 0, sizeof diffbuf);
        for (i = 0; i < 64; i++)
        {
          leaf = gethuff(huff[i > 0]);
          if (leaf == 0 && i)
            break;
          if (leaf == 0xff)
            continue;
          i += leaf >> 4;
          len = leaf & 15;
          if (len == 0)
            continue;
          diff = getbits(len);
          if ((diff & (1 << (len - 1))) == 0)
            diff -= (1 << len) - 1;
          if (i < 64)
            diffbuf[i] = diff;
        }
        diffbuf[0] += carry;
        carry = diffbuf[0];
        for (i = 0; i < 64; i++)
        {
          if (pnum++ % raw_width == 0)
            base[0] = base[1] = 512;
          if ((pixel[(block << 6) + i] = base[i & 1] += diffbuf[i]) >> 10)
            derror();
        }
      }
      if (lowbits)
      {
        save = ftell(ifp);
        fseek(ifp, 26 + row * raw_width / 4, SEEK_SET);
        for (prow = pixel, i = 0; i < raw_width * 2; i++)
        {
          c = fgetc(ifp);
          for (r = 0; r < 8; r += 2, prow++)
          {
            val = (*prow << 2) + ((c >> r) & 3);
            if (raw_width == 2672 && val < 512)
              val += 2;
            *prow = val;
          }
        }
        fseek(ifp, save, SEEK_SET);
      }
    }
  }
  catch (...)
  {
    FORC(2) free(huff[c]);
    throw;
  }
  FORC(2) free(huff[c]);
}