brand ()
{
  /* From "Random number generators: good ones are hard to find",
     Park and Miller, Communications of the ACM, vol. 31, no. 10,
     October 1988, p. 1195. filtered through FreeBSD */
  long h, l;

  /* Can't seed with 0. */
  if (rseed == 0)
    rseed = 123459876;
  h = rseed / 127773;
  l = rseed % 127773;
  rseed = 16807 * l - 2836 * h;
#if 0
  if (rseed < 0)
    rseed += 0x7fffffff;
#endif
  return ((unsigned int)(rseed & 32767));	/* was % 32768 */
}