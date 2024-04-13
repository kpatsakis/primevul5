void LibRaw::process_Sony_0x2010(uchar *buf, ushort len)
{
  if (!imSony.group2010)
    return;

  if ((imSony.real_iso_offset != 0xffff) &&
      (len >= (imSony.real_iso_offset + 2)) && (imCommon.real_ISO < 0.1f))
  {
    uchar s[2];
    s[0] = SonySubstitution[buf[imSony.real_iso_offset]];
    s[1] = SonySubstitution[buf[imSony.real_iso_offset + 1]];
    imCommon.real_ISO =
        100.0f * libraw_powf64l(2.0f, (16 - ((float)sget2(s)) / 256.0f));
  }

  if (len >= (imSony.MeteringMode_offset + 2))
  {
    imgdata.shootinginfo.MeteringMode =
        SonySubstitution[buf[imSony.MeteringMode_offset]];
    imgdata.shootinginfo.ExposureProgram =
        SonySubstitution[buf[imSony.ExposureProgram_offset]];
  }

  if (len >= (imSony.ReleaseMode2_offset + 2))
  {
    imgdata.shootinginfo.DriveMode =
        SonySubstitution[buf[imSony.ReleaseMode2_offset]];
  }
}