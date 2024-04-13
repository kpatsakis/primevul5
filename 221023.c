dnp3_al_get_timestamp(nstime_t *timestamp, tvbuff_t *tvb, int data_pos)
{

  guint32 hi, lo;
  guint64 time_ms;

  lo = tvb_get_letohs(tvb, data_pos);
  hi = tvb_get_letohl(tvb, data_pos + 2);

  time_ms = (guint64)hi * 0x10000 + lo;

  timestamp->secs  = (long)(time_ms / 1000);
  timestamp->nsecs = (int)(time_ms % 1000) * 1000000;
}