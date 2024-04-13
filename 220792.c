compact_attrdata(Repodata *data, int entry, int nentry)
{
  int i;
  unsigned int attrdatastart = data->attrdatalen;
  unsigned int attriddatastart = data->attriddatalen;
  if (attrdatastart < 1024 * 1024 * 4 && attriddatastart < 1024 * 1024)
    return;
  for (i = entry; i < nentry; i++)
    {
      Id v, *attrs = data->attrs[i];
      if (!attrs)
	continue;
      for (; *attrs; attrs += 2)
	{
	  switch (data->keys[*attrs].type)
	    {
	    case REPOKEY_TYPE_STR:
	    case REPOKEY_TYPE_BINARY:
	    case_CHKSUM_TYPES:
	      if ((unsigned int)attrs[1] < attrdatastart)
		 attrdatastart = attrs[1];
	      break;
	    case REPOKEY_TYPE_DIRSTRARRAY:
	      for (v = attrs[1]; data->attriddata[v] ; v += 2)
		if ((unsigned int)data->attriddata[v + 1] < attrdatastart)
		  attrdatastart = data->attriddata[v + 1];
	      /* FALLTHROUGH */
	    case REPOKEY_TYPE_IDARRAY:
	    case REPOKEY_TYPE_DIRNUMNUMARRAY:
	      if ((unsigned int)attrs[1] < attriddatastart)
		attriddatastart = attrs[1];
	      break;
	    case REPOKEY_TYPE_FIXARRAY:
	    case REPOKEY_TYPE_FLEXARRAY:
	      return;
	    default:
	      break;
	    }
	}
    }
#if 0
  printf("compact_attrdata %d %d\n", entry, nentry);
  printf("attrdatastart: %d\n", attrdatastart);
  printf("attriddatastart: %d\n", attriddatastart);
#endif
  if (attrdatastart < 1024 * 1024 * 4 && attriddatastart < 1024 * 1024)
    return;
  for (i = entry; i < nentry; i++)
    {
      Id v, *attrs = data->attrs[i];
      if (!attrs)
	continue;
      for (; *attrs; attrs += 2)
	{
	  switch (data->keys[*attrs].type)
	    {
	    case REPOKEY_TYPE_STR:
	    case REPOKEY_TYPE_BINARY:
	    case_CHKSUM_TYPES:
	      attrs[1] -= attrdatastart;
	      break;
	    case REPOKEY_TYPE_DIRSTRARRAY:
	      for (v = attrs[1]; data->attriddata[v] ; v += 2)
		data->attriddata[v + 1] -= attrdatastart;
	      /* FALLTHROUGH */
	    case REPOKEY_TYPE_IDARRAY:
	    case REPOKEY_TYPE_DIRNUMNUMARRAY:
	      attrs[1] -= attriddatastart;
	      break;
	    default:
	      break;
	    }
	}
    }
  if (attrdatastart)
    {
      data->attrdatalen -= attrdatastart;
      memmove(data->attrdata, data->attrdata + attrdatastart, data->attrdatalen);
      data->attrdata = solv_extend_resize(data->attrdata, data->attrdatalen, 1, REPODATA_ATTRDATA_BLOCK);
    }
  if (attriddatastart)
    {
      data->attriddatalen -= attriddatastart;
      memmove(data->attriddata, data->attriddata + attriddatastart, data->attriddatalen * sizeof(Id));
      data->attriddata = solv_extend_resize(data->attriddata, data->attriddatalen, sizeof(Id), REPODATA_ATTRIDDATA_BLOCK);
    }
}