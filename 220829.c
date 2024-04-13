repodata_search_array(Repodata *data, Id solvid, Id keyname, int flags, Repokey *key, KeyValue *kv, int (*callback)(void *cbdata, Solvable *s, Repodata *data, Repokey *key, KeyValue *kv), void *cbdata)
{
  Solvable *s = solvid > 0 ? data->repo->pool->solvables + solvid : 0;
  unsigned char *dp = (unsigned char *)kv->str;
  int stop;
  Id schema = 0;

  if (!dp || kv->entry != -1)
    return 0;
  while (++kv->entry < (int)kv->num)
    {
      if (kv->entry)
	dp = data_skip_schema(data, dp, schema);
      if (kv->entry == 0 || key->type == REPOKEY_TYPE_FLEXARRAY)
	dp = data_read_id(dp, &schema);
      kv->id = schema;
      kv->str = (const char *)dp;
      kv->eof = kv->entry == kv->num - 1 ? 1 : 0;
      stop = callback(cbdata, s, data, key, kv);
      if (stop && stop != SEARCH_ENTERSUB)
	return stop;
      if ((flags & SEARCH_SUB) != 0 || stop == SEARCH_ENTERSUB)
        repodata_search_keyskip(data, solvid, keyname, flags | SEARCH_SUBSCHEMA, (Id *)kv, callback, cbdata);
    }
  if ((flags & SEARCH_ARRAYSENTINEL) != 0)
    {
      if (kv->entry)
	dp = data_skip_schema(data, dp, schema);
      kv->id = 0;
      kv->str = (const char *)dp;
      kv->eof = 2;
      return callback(cbdata, s, data, key, kv);
    }
  return 0;
}