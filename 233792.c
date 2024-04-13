pr_sgr_start_if (char const *s)
{
  if (color_option)
    pr_sgr_start (s);
}