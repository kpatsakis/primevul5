void LibRaw::sony_arw2_load_raw()
{
  uchar *data, *dp;
  ushort pix[16];
  int row, col, val, max, min, imax, imin, sh, bit, i;

  data = (uchar *)malloc(raw_width + 1);
  merror(data, "sony_arw2_load_raw()");
  try
  {
    for (row = 0; row < height; row++)
    {
      checkCancel();
      fread(data, 1, raw_width, ifp);
      for (dp = data, col = 0; col < raw_width - 30; dp += 16)
      {
        max = 0x7ff & (val = sget4(dp));
        min = 0x7ff & val >> 11;
        imax = 0x0f & val >> 22;
        imin = 0x0f & val >> 26;
        for (sh = 0; sh < 4 && 0x80 << sh <= max - min; sh++)
          ;
        /* flag checks if outside of loop */
        if (!(imgdata.params.raw_processing_options &
              LIBRAW_PROCESSING_SONYARW2_ALLFLAGS) // no flag set
            || (imgdata.params.raw_processing_options &
                LIBRAW_PROCESSING_SONYARW2_DELTATOVALUE))
        {
          for (bit = 30, i = 0; i < 16; i++)
            if (i == imax)
              pix[i] = max;
            else if (i == imin)
              pix[i] = min;
            else
            {
              pix[i] =
                  ((sget2(dp + (bit >> 3)) >> (bit & 7) & 0x7f) << sh) + min;
              if (pix[i] > 0x7ff)
                pix[i] = 0x7ff;
              bit += 7;
            }
        }
        else if (imgdata.params.raw_processing_options &
                 LIBRAW_PROCESSING_SONYARW2_BASEONLY)
        {
          for (bit = 30, i = 0; i < 16; i++)
            if (i == imax)
              pix[i] = max;
            else if (i == imin)
              pix[i] = min;
            else
              pix[i] = 0;
        }
        else if (imgdata.params.raw_processing_options &
                 LIBRAW_PROCESSING_SONYARW2_DELTAONLY)
        {
          for (bit = 30, i = 0; i < 16; i++)
            if (i == imax)
              pix[i] = 0;
            else if (i == imin)
              pix[i] = 0;
            else
            {
              pix[i] =
                  ((sget2(dp + (bit >> 3)) >> (bit & 7) & 0x7f) << sh) + min;
              if (pix[i] > 0x7ff)
                pix[i] = 0x7ff;
              bit += 7;
            }
        }
        else if (imgdata.params.raw_processing_options &
                 LIBRAW_PROCESSING_SONYARW2_DELTAZEROBASE)
        {
          for (bit = 30, i = 0; i < 16; i++)
            if (i == imax)
              pix[i] = 0;
            else if (i == imin)
              pix[i] = 0;
            else
            {
              pix[i] = ((sget2(dp + (bit >> 3)) >> (bit & 7) & 0x7f) << sh);
              if (pix[i] > 0x7ff)
                pix[i] = 0x7ff;
              bit += 7;
            }
        }

        if (imgdata.params.raw_processing_options &
            LIBRAW_PROCESSING_SONYARW2_DELTATOVALUE)
        {
          for (i = 0; i < 16; i++, col += 2)
          {
            unsigned slope =
                pix[i] < 1001 ? 2
                              : curve[pix[i] << 1] - curve[(pix[i] << 1) - 2];
            unsigned step = 1 << sh;
            RAW(row, col) =
                curve[pix[i] << 1] >
                        black + imgdata.params.sony_arw2_posterization_thr
                    ? LIM(((slope * step * 1000) /
                           (curve[pix[i] << 1] - black)),
                          0, 10000)
                    : 0;
          }
        }
        else
          for (i = 0; i < 16; i++, col += 2)
            RAW(row, col) = curve[pix[i] << 1];
        col -= col & 1 ? 1 : 31;
      }
    }
  }
  catch (...)
  {
    free(data);
    throw;
  }
  if (imgdata.params.raw_processing_options &
      LIBRAW_PROCESSING_SONYARW2_DELTATOVALUE)
    maximum = 10000;
  free(data);
}