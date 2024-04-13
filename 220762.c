repodata_search_keyskip(Repodata *data, Id solvid, Id keyname, int flags, Id *keyskip, int (*callback)(void *cbdata, Solvable *s, Repodata *data, Repokey *key, KeyValue *kv), void *cbdata)
{
  Id schema;
  Repokey *key;
  Id keyid, *kp, *keyp;
  unsigned char *dp, *ddp;
  int onekey = 0;
  int stop;
  KeyValue kv;
  Solvable *s;

  if (!maybe_load_repodata(data, keyname))
    return;
  if ((flags & SEARCH_SUBSCHEMA) != 0)
    {
      flags ^= SEARCH_SUBSCHEMA;
      kv.parent = (KeyValue *)keyskip;
      keyskip = 0;
      schema = kv.parent->id;
      dp = (unsigned char *)kv.parent->str;
    }
  else
    {
      schema = 0;
      dp = solvid2data(data, solvid, &schema);
      if (!dp)
	return;
      kv.parent = 0;
    }
  s = solvid > 0 ? data->repo->pool->solvables + solvid : 0;
  keyp = data->schemadata + data->schemata[schema];
  if (keyname)
    {
      /* search for a specific key */
      for (kp = keyp; *kp; kp++)
	if (data->keys[*kp].name == keyname)
	  break;
      if (!*kp)
	return;
      dp = forward_to_key(data, *kp, keyp, dp);
      if (!dp)
	return;
      keyp = kp;
      onekey = 1;
    }
  while ((keyid = *keyp++) != 0)
    {
      stop = 0;
      key = data->keys + keyid;
      ddp = get_data(data, key, &dp, *keyp && !onekey ? 1 : 0);

      if (keyskip && (key->name >= keyskip[0] || keyskip[3 + key->name] != keyskip[1] + data->repodataid))
	{
	  if (onekey)
	    return;
	  continue;
	}
      if (key->type == REPOKEY_TYPE_DELETED && !(flags & SEARCH_KEEP_TYPE_DELETED))
	{
	  if (onekey)
	    return;
	  continue;
	}
      if (key->type == REPOKEY_TYPE_FLEXARRAY || key->type == REPOKEY_TYPE_FIXARRAY)
	{
	  kv.entry = -1;
	  ddp = data_read_id(ddp, (Id *)&kv.num);
	  kv.str = (const char *)ddp;
	  stop = repodata_search_array(data, solvid, 0, flags, key, &kv, callback, cbdata);
	  if (onekey || stop > SEARCH_NEXT_KEY)
	    return;
	  continue;
	}
      kv.entry = 0;
      do
	{
	  ddp = data_fetch(ddp, &kv, key);
	  if (!ddp)
	    break;
	  stop = callback(cbdata, s, data, key, &kv);
	  kv.entry++;
	}
      while (!kv.eof && !stop);
      if (onekey || stop > SEARCH_NEXT_KEY)
	return;
    }
}