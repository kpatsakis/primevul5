err_readonly (s)
     const char *s;
{
  report_error (_("%s: readonly variable"), s);
}