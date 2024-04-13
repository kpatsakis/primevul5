repodata_stub_loader(Repodata *data)
{
  Repo *repo = data->repo;
  Pool *pool = repo->pool;
  int r, i;
  struct s_Pool_tmpspace oldtmpspace;
  Datapos oldpos;

  if (!pool->loadcallback)
    {
      data->state = REPODATA_ERROR;
      return;
    }
  data->state = REPODATA_LOADING;

  /* save tmp space and pos */
  oldtmpspace = pool->tmpspace;
  memset(&pool->tmpspace, 0, sizeof(pool->tmpspace));
  oldpos = pool->pos;

  r = pool->loadcallback(pool, data, pool->loadcallbackdata);

  /* restore tmp space and pos */
  for (i = 0; i < POOL_TMPSPACEBUF; i++)
    solv_free(pool->tmpspace.buf[i]);
  pool->tmpspace = oldtmpspace;
  if (r && oldpos.repo == repo && oldpos.repodataid == data->repodataid)
    memset(&oldpos, 0, sizeof(oldpos));
  pool->pos = oldpos;

  data->state = r ? REPODATA_AVAILABLE : REPODATA_ERROR;
}