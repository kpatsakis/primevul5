dataiterator_strdup(Dataiterator *di)
{
  int l = -1;

  if (!di->kv.str || di->kv.str == di->dupstr)
    return;
  switch (di->key->type)
    {
    case_CHKSUM_TYPES:
    case REPOKEY_TYPE_DIRSTRARRAY:
      if (di->kv.num)	/* was it stringified into tmp space? */
        l = strlen(di->kv.str) + 1;
      break;
    default:
      break;
    }
  if (l < 0 && di->key->storage == KEY_STORAGE_VERTICAL_OFFSET)
    {
      switch (di->key->type)
	{
	case REPOKEY_TYPE_STR:
	case REPOKEY_TYPE_DIRSTRARRAY:
	  l = strlen(di->kv.str) + 1;
	  break;
	case_CHKSUM_TYPES:
	  l = solv_chksum_len(di->key->type);
	  break;
	case REPOKEY_TYPE_BINARY:
	  l = di->kv.num;
	  break;
	}
    }
  if (l >= 0)
    {
      if (!di->dupstrn || di->dupstrn < l)
	{
	  di->dupstrn = l + 16;
	  di->dupstr = solv_realloc(di->dupstr, di->dupstrn);
	}
      if (l)
        memcpy(di->dupstr, di->kv.str, l);
      di->kv.str = di->dupstr;
    }
}