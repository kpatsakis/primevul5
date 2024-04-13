print_filename (void)
{
  pr_sgr_start_if (filename_color);
  fputs (filename, stdout);
  pr_sgr_end_if (filename_color);
}