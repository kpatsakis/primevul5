dataiterator_match(Dataiterator *di, Datamatcher *ma)
{
  const char *str;
  if (!(str = repodata_stringify(di->pool, di->data, di->key, &di->kv, di->flags)))
    return 0;
  return ma ? datamatcher_match(ma, str) : 1;
}