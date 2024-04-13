err_badarraysub (s)
     const char *s;
{
  report_error ("%s: %s", s, _(bash_badsub_errmsg));
}