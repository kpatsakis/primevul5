void read_user_name(char *name)
{
  if (geteuid() == 0)
    strcpy(name,"root");		/* allow use of surun */
  else
  {
#ifdef HAVE_GETPWUID
    struct passwd *skr;
    const char *str;
    if ((skr=getpwuid(geteuid())) != NULL)
    {
      str=skr->pw_name;
    } else if ((str=getlogin()) == NULL)
    {
      if (!(str=getenv("USER")) && !(str=getenv("LOGNAME")) &&
               !(str=getenv("LOGIN")))
        str="UNKNOWN_USER";
    }
    ma_strmake(name,str,USERNAME_LENGTH);
#elif defined(HAVE_CUSERID)
    (void) cuserid(name);
#else
    ma_strmake(name,"UNKNOWN_USER", USERNAME_LENGTH);
#endif
  }
  return;
}