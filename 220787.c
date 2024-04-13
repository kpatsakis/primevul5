repodata_add_stubkey(Repodata *data, Id keyname, Id keytype)
{
  Repokey xkey;

  xkey.name = keyname;
  xkey.type = keytype;
  xkey.storage = KEY_STORAGE_INCORE;
  xkey.size = 0;
  repodata_key2id(data, &xkey, 1);
}