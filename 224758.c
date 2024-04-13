dup_error (oldd, newd)
     int oldd, newd;
{
  sys_error (_("cannot duplicate fd %d to fd %d"), oldd, newd);
}