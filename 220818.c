repodata_translate_id(Repodata *data, Repodata *fromdata, Id id, int create)
{
  const char *s;
  if (!id || !data || !fromdata)
    return id;
  if (data == fromdata || (!data->localpool && !fromdata->localpool))
    return id;
  if (fromdata->localpool)
    s = stringpool_id2str(&fromdata->spool, id);
  else
    s = pool_id2str(data->repo->pool, id);
  if (data->localpool)
    return stringpool_str2id(&data->spool, s, create);
  else
    return pool_str2id(data->repo->pool, s, create);
}