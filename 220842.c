dataiterator_setpos(Dataiterator *di)
{
  if (di->kv.eof == 2)
    {
      pool_clear_pos(di->pool);
      return;
    }
  di->pool->pos.solvid = di->solvid;
  di->pool->pos.repo = di->repo;
  di->pool->pos.repodataid = di->data - di->repo->repodata;
  di->pool->pos.schema = di->kv.id;
  di->pool->pos.dp = (unsigned char *)di->kv.str - di->data->incoredata;
}