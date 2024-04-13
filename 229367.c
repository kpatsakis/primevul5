net_field_length_ll(uchar **packet)
{
  reg1 uchar *pos= *packet;
  if (*pos < 251)
  {
    (*packet)++;
    return (unsigned long long) *pos;
  }
  if (*pos == 251)
  {
    (*packet)++;
    return (unsigned long long) NULL_LENGTH;
  }
  if (*pos == 252)
  {
    (*packet)+=3;
    return (unsigned long long) uint2korr(pos+1);
  }
  if (*pos == 253)
  {
    (*packet)+=4;
    return (unsigned long long) uint3korr(pos+1);
  }
  (*packet)+=9;					/* Must be 254 when here */
#ifdef NO_CLIENT_LONGLONG
  return (unsigned long long) uint4korr(pos+1);
#else
  return (unsigned long long) uint8korr(pos+1);
#endif
}