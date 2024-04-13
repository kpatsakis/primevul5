ulong STDCALL mysql_hex_string(char *to, const char *from, unsigned long len)
{
  char *start= to;
  char hexdigits[]= "0123456789ABCDEF";

  while (len--)
  {
    *to++= hexdigits[((unsigned char)*from) >> 4];
    *to++= hexdigits[((unsigned char)*from) & 0x0F];
    from++;
  }
  *to= 0;
  return (ulong)(to - start);
}