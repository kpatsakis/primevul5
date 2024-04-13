get_data(Repodata *data, Repokey *key, unsigned char **dpp, int advance)
{
  unsigned char *dp = *dpp;

  if (!dp)
    return 0;
  if (key->storage == KEY_STORAGE_INCORE)
    {
      if (advance)
        *dpp = data_skip_key(data, dp, key);
      return dp;
    }
  else if (key->storage == KEY_STORAGE_VERTICAL_OFFSET)
    {
      Id off, len;
      dp = data_read_id(dp, &off);
      dp = data_read_id(dp, &len);
      if (advance)
        *dpp = dp;
      return get_vertical_data(data, key, off, len);
    }
  return 0;
}