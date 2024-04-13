repodata_search_uninternalized(Repodata *data, Id solvid, Id keyname, int flags, int (*callback)(void *cbdata, Solvable *s, Repodata *data, Repokey *key, KeyValue *kv), void *cbdata)
{
  Id *ap;
  int stop;
  Solvable *s;
  KeyValue kv;

  if (!data->attrs || solvid < data->start || solvid >= data->end)
    return;
  ap = data->attrs[solvid - data->start];
  if (!ap)
    return;
  for (; *ap; ap += 2)
    {
      Repokey *key = data->keys + *ap;
      if (keyname && key->name != keyname)
	continue;
      s = solvid > 0 ? data->repo->pool->solvables + solvid : 0;
      kv.entry = 0;
      do
	{
	  data_fetch_uninternalized(data, key, ap[1], &kv);
	  stop = callback(cbdata, s, data, key, &kv);
	  kv.entry++;
	}
      while (!kv.eof && !stop);
      if (keyname || stop > SEARCH_NEXT_KEY)
	return;
    }
}