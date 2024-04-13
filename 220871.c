repodata_unset(Repodata *data, Id solvid, Id keyname)
{
  Repokey key;
  key.name = keyname;
  key.type = REPOKEY_TYPE_DELETED;
  key.size = 0;
  key.storage = KEY_STORAGE_INCORE;
  repodata_set(data, solvid, &key, 0);
}