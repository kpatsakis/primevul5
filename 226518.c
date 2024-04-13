void LibRaw::process_Sony_0x9403(uchar *buf, ushort len)
{
  if (len < 6)
    return;
  uchar bufx = SonySubstitution[buf[4]];
  if ((bufx == 0x00) || (bufx == 0x94))
    return;

  imCommon.SensorTemperature = (float)((short)SonySubstitution[buf[5]]);

  return;
}