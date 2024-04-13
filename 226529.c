void LibRaw::process_Sony_0x0116(uchar *buf, ushort len, unsigned long long id)
{
  int i = 0;

  if (((id == SonyID_DSLR_A900)      ||
       (id == SonyID_DSLR_A900_APSC) ||
       (id == SonyID_DSLR_A850)      ||
       (id == SonyID_DSLR_A850_APSC)) &&
      (len >= 2))
    i = 1;
  else if ((id >= SonyID_DSLR_A550) && (len >= 3))
    i = 2;
  else
    return;

  imCommon.BatteryTemperature = (float)(buf[i] - 32) / 1.8f;
}