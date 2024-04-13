repodata_lookup_idarray(Repodata *data, Id solvid, Id keyname, Queue *q)
{
  unsigned char *dp;
  Repokey *key;
  Id id;
  int eof = 0;

  queue_empty(q);
  dp = find_key_data(data, solvid, keyname, &key);
  if (!dp)
    return 0;
  switch (key->type)
    {
    case REPOKEY_TYPE_CONSTANTID:
      queue_push(q, key->size);
      break;
    case REPOKEY_TYPE_ID:
      dp = data_read_id(dp, &id);
      queue_push(q, id);
      break;
    case REPOKEY_TYPE_IDARRAY:
      for (;;)
	{
	  dp = data_read_ideof(dp, &id, &eof);
	  queue_push(q, id);
	  if (eof)
	    break;
	}
      break;
    default:
      return 0;
    }
  return 1;
}