repodata_set(Repodata *data, Id solvid, Repokey *key, Id val)
{
  Id keyid;

  keyid = repodata_key2id(data, key, 1);
  repodata_insert_keyid(data, solvid, keyid, val, 1);
}