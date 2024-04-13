repodata_set_poolstrn(Repodata *data, Id solvid, Id keyname, const char *str, int l)
{
  Id id;
  if (data->localpool)
    id = stringpool_strn2id(&data->spool, str, l, 1);
  else
    id = pool_strn2id(data->repo->pool, str, l, 1);
  repodata_set_id(data, solvid, keyname, id);
}