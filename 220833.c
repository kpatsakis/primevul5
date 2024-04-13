repodata_translate_dir_slow(Repodata *data, Repodata *fromdata, Id dir, int create, Id *cache)
{
  Id parent, compid;
  if (!dir)
    {
      /* make sure that the dirpool has an entry */
      if (create && !data->dirpool.ndirs)
        dirpool_add_dir(&data->dirpool, 0, 0, create);
      return 0;
    }
  parent = dirpool_parent(&fromdata->dirpool, dir);
  if (parent)
    {
      if (!(parent = repodata_translate_dir(data, fromdata, parent, create, cache)))
	return 0;
    }
  compid = dirpool_compid(&fromdata->dirpool, dir);
  if (compid > 1 && (data->localpool || fromdata->localpool))
    {
      if (!(compid = repodata_translate_id(data, fromdata, compid, create)))
	return 0;
    }
  if (!(compid = dirpool_add_dir(&data->dirpool, parent, compid, create)))
    return 0;
  if (cache)
    {
      cache[(dir & 255) * 2] = dir;
      cache[(dir & 255) * 2 + 1] = compid;
    }
  return compid;
}