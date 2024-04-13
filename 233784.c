print_offset (uintmax_t pos, int min_width, const char *color)
{
  /* Do not rely on printf to print pos, since uintmax_t may be longer
     than long, and long long is not portable.  */

  char buf[sizeof pos * CHAR_BIT];
  char *p = buf + sizeof buf;

  do
    {
      *--p = '0' + pos % 10;
      --min_width;
    }
  while ((pos /= 10) != 0);

  /* Do this to maximize the probability of alignment across lines.  */
  if (align_tabs)
    while (--min_width >= 0)
      *--p = ' ';

  pr_sgr_start_if (color);
  fwrite (p, 1, buf + sizeof buf - p, stdout);
  pr_sgr_end_if (color);
}