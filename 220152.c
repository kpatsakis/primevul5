void LibRaw::sony_load_raw()
{
  uchar head[40];
  ushort *pixel;
  unsigned i, key, row, col;

  fseek(ifp, 200896, SEEK_SET);
  fseek(ifp, (unsigned)fgetc(ifp) * 4 - 1, SEEK_CUR);
  order = 0x4d4d;
  key = get4();

  fseek(ifp, 164600, SEEK_SET);
  fread(head, 1, 40, ifp);
  sony_decrypt((unsigned *)head, 10, 1, key);
  for (i = 26; i-- > 22;)
    key = key << 8 | head[i];

  fseek(ifp, data_offset, SEEK_SET);
  for (row = 0; row < raw_height; row++)
  {
    checkCancel();
    pixel = raw_image + row * raw_width;
    if (fread(pixel, 2, raw_width, ifp) < raw_width)
      derror();
    sony_decrypt((unsigned *)pixel, raw_width / 2, !row, key);
    for (col = 0; col < raw_width; col++)
      if ((pixel[col] = ntohs(pixel[col])) >> 14)
        derror();
  }
  maximum = 0x3ff0;
}