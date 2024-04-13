void LibRaw::process_Sony_0x9406(uchar *buf, ushort len)
{
  if (len < 6)
    return;
  uchar bufx = buf[0];
  if ((bufx != 0x01) && (bufx != 0x08) && (bufx != 0x1b))
    return;
  bufx = buf[2];
  if ((bufx != 0x08) && (bufx != 0x1b))
    return;

  imCommon.BatteryTemperature =
      (float)(SonySubstitution[buf[5]] - 32) / 1.8f;

  return;
}