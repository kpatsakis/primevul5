repodata_search(Repodata *data, Id solvid, Id keyname, int flags, int (*callback)(void *cbdata, Solvable *s, Repodata *data, Repokey *key, KeyValue *kv), void *cbdata)
{
  repodata_search_keyskip(data, solvid, keyname, flags, 0, callback, cbdata);
}