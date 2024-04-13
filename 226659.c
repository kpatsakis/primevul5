static uint32_t debug_read_uleb128(const uint8_t **pp)
{
  const uint8_t *p = *pp;
  uint32_t v = *p++;
  if (LJ_UNLIKELY(v >= 0x80)) {
    int sh = 0;
    v &= 0x7f;
    do { v |= ((*p & 0x7f) << (sh += 7)); } while (*p++ >= 0x80);
  }
  *pp = p;
  return v;
}