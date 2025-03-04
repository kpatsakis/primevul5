static inline uint32_t ReadWebPLSBWord(
  const unsigned char *magick_restrict data)
{
  const unsigned char
    *p;

  uint32_t
    value;

  p=data;
  value=(uint32_t) (*p++);
  value|=((uint32_t) (*p++)) << 8;
  value|=((uint32_t) (*p++)) << 16;
  value|=((uint32_t) (*p++)) << 24;
  return(value);
}