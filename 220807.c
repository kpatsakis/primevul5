repodata_set_constantid(Repodata *data, Id solvid, Id keyname, Id id)
{
  Repokey key;
  key.name = keyname;
  key.type = REPOKEY_TYPE_CONSTANTID;
  key.size = id;
  key.storage = KEY_STORAGE_INCORE;
  repodata_set(data, solvid, &key, 0);
}