print_sep (char sep)
{
  pr_sgr_start_if (sep_color);
  fputc (sep, stdout);
  pr_sgr_end_if (sep_color);
}