pr_sgr_start (char const *s)
{
  if (*s)
    print_start_colorize (sgr_start, s);
}