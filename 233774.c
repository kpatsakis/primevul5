suppressible_error (char const *mesg, int errnum)
{
  if (! suppress_errors)
    error (0, errnum, "%s", mesg);
  errseen = 1;
}