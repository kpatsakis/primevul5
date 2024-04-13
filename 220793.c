repodata_lookup_str(Repodata *data, Id solvid, Id keyname)
{
  unsigned char *dp;
  Repokey *key;
  Id id;

  dp = find_key_data(data, solvid, keyname, &key);
  if (!dp)
    return 0;
  if (key->type == REPOKEY_TYPE_STR)
    return (const char *)dp;
  if (key->type == REPOKEY_TYPE_CONSTANTID)
    id = key->size;
  else if (key->type == REPOKEY_TYPE_ID)
    dp = data_read_id(dp, &id);
  else
    return 0;
  if (data->localpool)
    return stringpool_id2str(&data->spool, id);
  return pool_id2str(data->repo->pool, id);
}