static size_t get_store_length(size_t length)
{
  #define MAX_STORE_SIZE 9
  unsigned char buffer[MAX_STORE_SIZE], *p;

  /* We just store the length and substract offset of our buffer
     to determine the length */
  p= mysql_net_store_length(buffer, length);
  return p - buffer;
}