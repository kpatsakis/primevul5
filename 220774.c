repodata_globalize_id(Repodata *data, Id id, int create)
{
  if (!id || !data || !data->localpool)
    return id;
  return pool_str2id(data->repo->pool, stringpool_id2str(&data->spool, id), create);
}