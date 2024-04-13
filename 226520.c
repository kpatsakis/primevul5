void LibRaw::process_Sony_0x9402(uchar *buf, ushort len)
{

  if (len < 23)
    return;

  imgdata.shootinginfo.FocusMode = SonySubstitution[buf[0x16]];

  if ((imSony.CameraType == LIBRAW_SONY_SLT) ||
      (imSony.CameraType == LIBRAW_SONY_ILCA))
    return;

  uchar bufx = buf[0x00];
  if ((bufx == 0x05) || (bufx == 0xff) || (buf[0x02] != 0xff))
    return;

  imCommon.AmbientTemperature =
      (float)((short)SonySubstitution[buf[0x04]]);

  return;
}