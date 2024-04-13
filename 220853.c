repodata_localize_id(Repodata *data, Id id, int create)
{
  if (!id || !data || !data->localpool)
    return id;
  return stringpool_str2id(&data->spool, pool_id2str(data->repo->pool, id), create);
}