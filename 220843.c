maybe_load_repodata_stub(Repodata *data, Id keyname)
{
  if (data->state != REPODATA_STUB)
    {
      data->state = REPODATA_ERROR;
      return 0;
    }
  if (keyname)
    {
      int i;
      for (i = 1; i < data->nkeys; i++)
	if (keyname == data->keys[i].name)
	  break;
      if (i == data->nkeys)
	return 0;
    }
  repodata_load(data);
  return data->state == REPODATA_AVAILABLE ? 1 : 0;
}