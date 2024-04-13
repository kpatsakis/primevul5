forward_to_key(Repodata *data, Id keyid, Id *keyp, unsigned char *dp)
{
  Id k;

  if (!keyid)
    return 0;
  if (data->mainschemaoffsets && dp == data->incoredata + data->mainschemaoffsets[0] && keyp == data->schemadata + data->schemata[data->mainschema])
    {
      int i;
      for (i = 0; (k = *keyp++) != 0; i++)
        if (k == keyid)
	  return data->incoredata + data->mainschemaoffsets[i];
      return 0;
    }
  while ((k = *keyp++) != 0)
    {
      if (k == keyid)
	return dp;
      if (data->keys[k].storage == KEY_STORAGE_VERTICAL_OFFSET)
	{
	  dp = data_skip(dp, REPOKEY_TYPE_ID);	/* skip offset */
	  dp = data_skip(dp, REPOKEY_TYPE_ID);	/* skip length */
	  continue;
	}
      if (data->keys[k].storage != KEY_STORAGE_INCORE)
	continue;
      dp = data_skip_key(data, dp, data->keys + k);
    }
  return 0;
}