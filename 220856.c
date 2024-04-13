dataiterator_setpos_parent(Dataiterator *di)
{
  if (!di->kv.parent || di->kv.parent->eof == 2)
    {
      pool_clear_pos(di->pool);
      return;
    }
  di->pool->pos.solvid = di->solvid;
  di->pool->pos.repo = di->repo;
  di->pool->pos.repodataid = di->data - di->repo->repodata;
  di->pool->pos.schema = di->kv.parent->id;
  di->pool->pos.dp = (unsigned char *)di->kv.parent->str - di->data->incoredata;
}