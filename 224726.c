sv_optind (name)
     char *name;
{
  char *tt;
  int s;

  tt = get_string_value ("OPTIND");
  if (tt && *tt)
    {
      s = atoi (tt);

      /* According to POSIX, setting OPTIND=1 resets the internal state
	 of getopt (). */
      if (s < 0 || s == 1)
	s = 0;
    }
  else
    s = 0;
  getopts_reset (s);
}