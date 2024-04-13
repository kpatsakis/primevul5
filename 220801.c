repodata_add_poolstr_array(Repodata *data, Id solvid, Id keyname,
			   const char *str)
{
  Id id;
  if (data->localpool)
    id = stringpool_str2id(&data->spool, str, 1);
  else
    id = pool_str2id(data->repo->pool, str, 1);
  repodata_add_idarray(data, solvid, keyname, id);
}