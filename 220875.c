repodata_initdata(Repodata *data, Repo *repo, int localpool)
{
  memset(data, 0, sizeof (*data));
  data->repodataid = data - repo->repodata;
  data->repo = repo;
  data->localpool = localpool;
  if (localpool)
    stringpool_init_empty(&data->spool);
  /* dirpool_init(&data->dirpool);	just zeros out again */
  data->keys = solv_calloc(1, sizeof(Repokey));
  data->nkeys = 1;
  data->schemata = solv_calloc(1, sizeof(Id));
  data->schemadata = solv_calloc(1, sizeof(Id));
  data->nschemata = 1;
  data->schemadatalen = 1;
  repopagestore_init(&data->store);
}