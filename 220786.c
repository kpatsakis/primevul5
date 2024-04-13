repodata_lookup_kv_uninternalized(Repodata *data, Id solvid, Id keyname, KeyValue *kv)
{
  Id *ap;
  if (!data->attrs || solvid < data->start || solvid >= data->end)
    return 0;
  ap = data->attrs[solvid - data->start];
  if (!ap)
    return 0;
  for (; *ap; ap += 2)
    {
      Repokey *key = data->keys + *ap;
      if (key->name != keyname)
	continue;
      data_fetch_uninternalized(data, key, ap[1], kv);
      return key;
    }
  return 0;
}