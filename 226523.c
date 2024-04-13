void LibRaw::parseSonyLensType2(uchar a, uchar b)
{
  ushort lid2;
  lid2 = (((ushort)a) << 8) | ((ushort)b);
//  printf ("==>> 2: lid2 %d\n", lid2);
  if (!lid2)
    return;
  if (lid2 < 0x100)
  {
    if ((ilm.AdapterID != 0x4900) && (ilm.AdapterID != 0xef00))
    {
      ilm.AdapterID = lid2;
      switch (lid2)
      {
      case 1:
      case 2:
      case 3:
      case 6:
        ilm.LensMount = LIBRAW_MOUNT_Minolta_A;
        break;
      case 44:
      case 78:
      case 184:
      case 234:
      case 239:
        ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
        break;
      }
    }
  }
  else
    ilm.LensID = lid2;

  if ((lid2 >= 50481) &&
      (lid2 < 50500)) {
    strcpy(ilm.Adapter, "MC-11");
    ilm.AdapterID = 0x4900;
  } else if ((lid2 > 0xef00) &&
             (lid2 < 0xffff) &&
             (lid2 != 0xff00)) {
    ilm.AdapterID = 0xef00;
    ilm.LensID -= ilm.AdapterID;
    ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
  }

  return;
}