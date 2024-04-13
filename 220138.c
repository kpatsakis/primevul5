void LibRaw::redcine_load_raw()
{
#ifndef NO_JASPER
  int c, row, col;
  jas_stream_t *in;
  jas_image_t *jimg;
  jas_matrix_t *jmat;
  jas_seqent_t *data;
  ushort *img, *pix;

  jas_init();
  in = (jas_stream_t *)ifp->make_jas_stream();
  if (!in)
    throw LIBRAW_EXCEPTION_DECODE_JPEG2000;
  jas_stream_seek(in, data_offset + 20, SEEK_SET);
  jimg = jas_image_decode(in, -1, 0);
  if (!jimg)
  {
    jas_stream_close(in);
    throw LIBRAW_EXCEPTION_DECODE_JPEG2000;
  }
  jmat = jas_matrix_create(height / 2, width / 2);
  merror(jmat, "redcine_load_raw()");
  img = (ushort *)calloc((height + 2), (width + 2) * 2);
  merror(img, "redcine_load_raw()");
  bool fastexitflag = false;
  try
  {
    FORC4
    {
      checkCancel();
      jas_image_readcmpt(jimg, c, 0, 0, width / 2, height / 2, jmat);
      data = jas_matrix_getref(jmat, 0, 0);
      for (row = c >> 1; row < height; row += 2)
        for (col = c & 1; col < width; col += 2)
          img[(row + 1) * (width + 2) + col + 1] =
              data[(row / 2) * (width / 2) + col / 2];
    }
    for (col = 1; col <= width; col++)
    {
      img[col] = img[2 * (width + 2) + col];
      img[(height + 1) * (width + 2) + col] =
          img[(height - 1) * (width + 2) + col];
    }
    for (row = 0; row < height + 2; row++)
    {
      img[row * (width + 2)] = img[row * (width + 2) + 2];
      img[(row + 1) * (width + 2) - 1] = img[(row + 1) * (width + 2) - 3];
    }
    for (row = 1; row <= height; row++)
    {
      checkCancel();
      pix = img + row * (width + 2) + (col = 1 + (FC(row, 1) & 1));
      for (; col <= width; col += 2, pix += 2)
      {
        c = (((pix[0] - 0x800) << 3) + pix[-(width + 2)] + pix[width + 2] +
             pix[-1] + pix[1]) >>
            2;
        pix[0] = LIM(c, 0, 4095);
      }
    }
    for (row = 0; row < height; row++)
    {
      checkCancel();
      for (col = 0; col < width; col++)
        RAW(row, col) = curve[img[(row + 1) * (width + 2) + col + 1]];
    }
  }
  catch (...)
  {
    fastexitflag = true;
  }
  free(img);
  jas_matrix_destroy(jmat);
  jas_image_destroy(jimg);
  jas_stream_close(in);
  if (fastexitflag)
    throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
#endif
}