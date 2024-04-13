repodata_lookup_binary(Repodata *data, Id solvid, Id keyname, int *lenp)
{
  unsigned char *dp;
  Repokey *key;
  Id len;

  dp = find_key_data(data, solvid, keyname, &key);
  if (!dp || key->type != REPOKEY_TYPE_BINARY)
    {
      *lenp = 0;
      return 0;
    }
  dp = data_read_id(dp, &len);
  *lenp = len;
  return dp;
}