repodata_setpos_kv(Repodata *data, KeyValue *kv)
{
  Pool *pool = data->repo->pool;
  if (!kv)
    pool_clear_pos(pool);
  else
    {
      pool->pos.repo = data->repo;
      pool->pos.repodataid = data - data->repo->repodata;
      pool->pos.dp = (unsigned char *)kv->str - data->incoredata;
      pool->pos.schema = kv->id;
    }
}