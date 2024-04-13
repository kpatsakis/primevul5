repodata_lookup_num(Repodata *data, Id solvid, Id keyname, unsigned long long notfound)
{
  unsigned char *dp;
  Repokey *key;
  unsigned int high, low;

  dp = find_key_data(data, solvid, keyname, &key);
  if (!dp)
    return notfound;
  switch (key->type)
    {
    case REPOKEY_TYPE_NUM:
      data_read_num64(dp, &low, &high);
      return (unsigned long long)high << 32 | low;
    case REPOKEY_TYPE_CONSTANT:
      return key->size;
    default:
      return notfound;
    }
}