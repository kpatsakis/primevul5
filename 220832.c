repodata_lookup_bin_checksum(Repodata *data, Id solvid, Id keyname, Id *typep)
{
  unsigned char *dp;
  Repokey *key;

  dp = find_key_data(data, solvid, keyname, &key);
  if (!dp)
    return 0;
  switch (key->type)
    {
    case_CHKSUM_TYPES:
      break;
    default:
      return 0;
    }
  *typep = key->type;
  return dp;
}