sv_tz (name)
     char *name;
{
  if (chkexport (name))
    tzset ();
}