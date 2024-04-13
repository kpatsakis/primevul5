static uint32_t luminocity(uint32_t x)
{
  int r = (x >> 16) & 0xff;
  int g = (x >>  8) & 0xff;
  int b = (x >>  0) & 0xff;
  int y = (r*19661+g*38666+b*7209 + 32829)>>16;
  return y << 24;
}
