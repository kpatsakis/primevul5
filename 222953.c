static size_t longest_common_prefix(char *dest, const char *src, size_t start, size_t dlen)
{
  size_t pos = start;

  while ((pos < dlen) && dest[pos] && (dest[pos] == src[pos]))
    pos++;
  dest[pos] = '\0';

  return pos;
}