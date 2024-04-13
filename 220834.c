repodata_stringify(Pool *pool, Repodata *data, Repokey *key, KeyValue *kv, int flags)
{
  switch (key->type)
    {
    case REPOKEY_TYPE_ID:
    case REPOKEY_TYPE_CONSTANTID:
    case REPOKEY_TYPE_IDARRAY:
      if (data && data->localpool)
	kv->str = stringpool_id2str(&data->spool, kv->id);
      else
	kv->str = pool_id2str(pool, kv->id);
      if ((flags & SEARCH_SKIP_KIND) != 0 && key->storage == KEY_STORAGE_SOLVABLE && (key->name == SOLVABLE_NAME || key->type == REPOKEY_TYPE_IDARRAY))
	{
	  const char *s;
	  for (s = kv->str; *s >= 'a' && *s <= 'z'; s++)
	    ;
	  if (*s == ':' && s > kv->str)
	    kv->str = s + 1;
	}
      return kv->str;
    case REPOKEY_TYPE_STR:
      return kv->str;
    case REPOKEY_TYPE_DIRSTRARRAY:
      if (!(flags & SEARCH_FILES))
	return kv->str;	/* match just the basename */
      if (kv->num)
	return kv->str;	/* already stringified */
      /* Put the full filename into kv->str.  */
      kv->str = repodata_dir2str(data, kv->id, kv->str);
      kv->num = 1;	/* mark stringification */
      return kv->str;
    case_CHKSUM_TYPES:
      if (!(flags & SEARCH_CHECKSUMS))
	return 0;	/* skip em */
      if (kv->num)
	return kv->str;	/* already stringified */
      kv->str = repodata_chk2str(data, key->type, (const unsigned char *)kv->str);
      kv->num = 1;	/* mark stringification */
      return kv->str;
    default:
      return 0;
    }
}