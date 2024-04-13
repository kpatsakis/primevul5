repodata_str2dir(Repodata *data, const char *dir, int create)
{
  Id id, parent;
#ifdef DIRCACHE_SIZE
  const char *dirs;
#endif
  const char *dire;

  if (!*dir)
    return data->dirpool.ndirs ? 0 : dirpool_add_dir(&data->dirpool, 0, 0, create);
  while (*dir == '/' && dir[1] == '/')
    dir++;
  if (*dir == '/' && !dir[1])
    return data->dirpool.ndirs ? 1 : dirpool_add_dir(&data->dirpool, 0, 1, create);
  parent = 0;
#ifdef DIRCACHE_SIZE
  dirs = dir;
  if (data->dircache)
    {
      int l;
      struct dircache *dircache = data->dircache;
      l = strlen(dir);
      while (l > 0)
	{
	  if (l < DIRCACHE_SIZE && dircache->ids[l] && !memcmp(dircache->str + l * (l - 1) / 2, dir, l))
	    {
	      parent = dircache->ids[l];
	      dir += l;
	      if (!*dir)
		return parent;
	      while (*dir == '/')
		dir++;
	      break;
	    }
	  while (--l)
	    if (dir[l] == '/')
	      break;
	}
    }
#endif
  while (*dir)
    {
      dire = strchrnul(dir, '/');
      if (data->localpool)
        id = stringpool_strn2id(&data->spool, dir, dire - dir, create);
      else
	id = pool_strn2id(data->repo->pool, dir, dire - dir, create);
      if (!id)
	return 0;
      parent = dirpool_add_dir(&data->dirpool, parent, id, create);
      if (!parent)
	return 0;
#ifdef DIRCACHE_SIZE
      if (!data->dircache)
	data->dircache = solv_calloc(1, sizeof(struct dircache));
      if (data->dircache)
	{
	  int l = dire - dirs;
	  if (l < DIRCACHE_SIZE)
	    {
	      data->dircache->ids[l] = parent;
	      memcpy(data->dircache->str + l * (l - 1) / 2, dirs, l);
	    }
	}
#endif
      if (!*dire)
	break;
      dir = dire + 1;
      while (*dir == '/')
	dir++;
    }
  return parent;
}