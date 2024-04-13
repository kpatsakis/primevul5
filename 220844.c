repodata_search_arrayelement(Repodata *data, Id solvid, Id keyname, int flags, KeyValue *kv, int (*callback)(void *cbdata, Solvable *s, Repodata *data, Repokey *key, KeyValue *kv), void *cbdata)
{
  repodata_search_keyskip(data, solvid, keyname, flags | SEARCH_SUBSCHEMA, (Id *)kv, callback, cbdata);
}