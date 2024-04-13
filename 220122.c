void LibRaw::nokia_load_raw()
{
  uchar *data, *dp;
  int rev, dwide, row, col, c;
  double sum[] = {0, 0};

  rev = 3 * (order == 0x4949);
  dwide = (raw_width * 5 + 1) / 4;
  data = (uchar *)malloc(dwide * 2);
  merror(data, "nokia_load_raw()");
  try
  {
    for (row = 0; row < raw_height; row++)
    {
      checkCancel();
      if (fread(data + dwide, 1, dwide, ifp) < dwide)
        derror();
      FORC(dwide) data[c] = data[dwide + (c ^ rev)];
      for (dp = data, col = 0; col < raw_width; dp += 5, col += 4)
        FORC4 RAW(row, col + c) = (dp[c] << 2) | (dp[4] >> (c << 1) & 3);
    }
  }
  catch (...)
  {
    free(data);
    throw;
  }
  free(data);
  maximum = 0x3ff;
  if (strncmp(make, "OmniVision", 10))
    return;
  row = raw_height / 2;
  FORC(width - 1)
  {
    sum[c & 1] += SQR(RAW(row, c) - RAW(row + 1, c + 1));
    sum[~c & 1] += SQR(RAW(row + 1, c) - RAW(row, c + 1));
  }
  if (sum[1] > sum[0])
    filters = 0x4b4b4b4b;
}