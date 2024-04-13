dataiterator_init(Dataiterator *di, Pool *pool, Repo *repo, Id p, Id keyname, const char *match, int flags)
{
  memset(di, 0, sizeof(*di));
  di->pool = pool;
  di->flags = flags & ~SEARCH_THISSOLVID;
  if (!pool || (repo && repo->pool != pool))
    {
      di->state = di_bye;
      return -1;
    }
  if (match)
    {
      int error;
      if ((error = datamatcher_init(&di->matcher, match, flags)) != 0)
	{
	  di->state = di_bye;
	  return error;
	}
    }
  di->keyname = keyname;
  di->keynames[0] = keyname;
  dataiterator_set_search(di, repo, p);
  return 0;
}