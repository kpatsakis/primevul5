dataiterator_init_clone(Dataiterator *di, Dataiterator *from)
{
  *di = *from;
  if (di->dupstr)
    {
      if (di->dupstr == di->kv.str)
        di->dupstr = solv_memdup(di->dupstr, di->dupstrn);
      else
	{
	  di->dupstr = 0;
	  di->dupstrn = 0;
	}
    }
  memset(&di->matcher, 0, sizeof(di->matcher));
  if (from->matcher.match)
    datamatcher_init(&di->matcher, from->matcher.match, from->matcher.flags);
  if (di->nparents)
    {
      /* fix pointers */
      int i;
      for (i = 1; i < di->nparents; i++)
	di->parents[i].kv.parent = &di->parents[i - 1].kv;
      di->kv.parent = &di->parents[di->nparents - 1].kv;
    }
  if (di->oldkeyskip)
    di->oldkeyskip = solv_memdup2(di->oldkeyskip, 3 + di->oldkeyskip[0], sizeof(Id));
  if (di->keyskip)
    di->keyskip = di->oldkeyskip;
}