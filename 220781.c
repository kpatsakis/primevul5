repodata_dir2str(Repodata *data, Id did, const char *suf)
{
  Pool *pool = data->repo->pool;
  int l = 0;
  Id parent, comp;
  const char *comps;
  char *p;

  if (!did)
    return suf ? suf : "";
  if (did == 1 && !suf)
    return "/";
  parent = did;
  while (parent)
    {
      comp = dirpool_compid(&data->dirpool, parent);
      comps = stringpool_id2str(data->localpool ? &data->spool : &pool->ss, comp);
      l += strlen(comps);
      parent = dirpool_parent(&data->dirpool, parent);
      if (parent)
	l++;
    }
  if (suf)
    l += strlen(suf) + 1;
  p = pool_alloctmpspace(pool, l + 1) + l;
  *p = 0;
  if (suf)
    {
      p -= strlen(suf);
      strcpy(p, suf);
      *--p = '/';
    }
  parent = did;
  while (parent)
    {
      comp = dirpool_compid(&data->dirpool, parent);
      comps = stringpool_id2str(data->localpool ? &data->spool : &pool->ss, comp);
      l = strlen(comps);
      p -= l;
      strncpy(p, comps, l);
      parent = dirpool_parent(&data->dirpool, parent);
      if (parent)
        *--p = '/';
    }
  return p;
}