repodata_lookup_void(Repodata *data, Id solvid, Id keyname)
{
  return repodata_lookup_type(data, solvid, keyname) == REPOKEY_TYPE_VOID ? 1 : 0;
}