void LibRaw::nikon_read_curve()
{
  ushort ver0, ver1, vpred[2][2], hpred[2], csize;
  int i, step, max;

  fseek(ifp, meta_offset, SEEK_SET);
  ver0 = fgetc(ifp);
  ver1 = fgetc(ifp);
  if (ver0 == 0x49 || ver1 == 0x58)
    fseek(ifp, 2110, SEEK_CUR);
  read_shorts(vpred[0], 4);
  max = 1 << tiff_bps & 0x7fff;
  if ((csize = get2()) > 1)
    step = max / (csize - 1);
  if (ver0 == 0x44 && (ver1 == 0x20 || (ver1 == 0x40 && step > 3)) && step > 0)
  {
    if (ver1 == 0x40)
    {
      step /= 4;
      max /= 4;
    }
    for (i = 0; i < csize; i++)
      curve[i * step] = get2();
    for (i = 0; i < max; i++)
      curve[i] = (curve[i - i % step] * (step - i % step) +
                  curve[i - i % step + step] * (i % step)) /
                 step;
  }
  else if (ver0 != 0x46 && csize <= 0x4001)
    read_shorts(curve, max = csize);
}