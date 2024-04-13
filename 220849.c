data_skip_key(Repodata *data, unsigned char *dp, Repokey *key)
{
  int nentries, schema;
  switch(key->type)
    {
    case REPOKEY_TYPE_FIXARRAY:
      dp = data_read_id(dp, &nentries);
      if (!nentries)
	return dp;
      dp = data_read_id(dp, &schema);
      while (nentries--)
	dp = data_skip_schema(data, dp, schema);
      return dp;
    case REPOKEY_TYPE_FLEXARRAY:
      dp = data_read_id(dp, &nentries);
      while (nentries--)
	{
	  dp = data_read_id(dp, &schema);
	  dp = data_skip_schema(data, dp, schema);
	}
      return dp;
    default:
      if (key->storage == KEY_STORAGE_INCORE)
        dp = data_skip(dp, key->type);
      else if (key->storage == KEY_STORAGE_VERTICAL_OFFSET)
	{
	  dp = data_skip(dp, REPOKEY_TYPE_ID);
	  dp = data_skip(dp, REPOKEY_TYPE_ID);
	}
      return dp;
    }
}