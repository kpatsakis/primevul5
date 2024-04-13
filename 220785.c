dataiterator_set_match(Dataiterator *di, const char *match, int flags)
{
  di->flags = (flags & ~SEARCH_THISSOLVID) | (di->flags & SEARCH_THISSOLVID);
  datamatcher_free(&di->matcher);
  memset(&di->matcher, 0, sizeof(di->matcher));
  if (match)
    {
      int error;
      if ((error = datamatcher_init(&di->matcher, match, flags)) != 0)
	{
	  di->state = di_bye;
	  return error;
	}
    }
  return 0;
}