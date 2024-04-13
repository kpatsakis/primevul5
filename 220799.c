repodata_add_array(Repodata *data, Id handle, Id keyname, Id keytype, int entrysize)
{
  int oldsize;
  Id *ida, *pp, **ppp;

  /* check if it is the same as last time, this speeds things up a lot */
  if (handle == data->lasthandle && data->keys[data->lastkey].name == keyname && data->keys[data->lastkey].type == keytype && data->attriddatalen == data->lastdatalen)
    {
      /* great! just append the new data */
      data->attriddata = solv_extend(data->attriddata, data->attriddatalen, entrysize, sizeof(Id), REPODATA_ATTRIDDATA_BLOCK);
      data->attriddatalen--;	/* overwrite terminating 0  */
      data->lastdatalen += entrysize;
      return;
    }

  ppp = repodata_get_attrp(data, handle);
  pp = *ppp;
  if (pp)
    {
      for (; *pp; pp += 2)
        if (data->keys[*pp].name == keyname)
          break;
    }
  if (!pp || !*pp || data->keys[*pp].type != keytype)
    {
      /* not found. allocate new key */
      Repokey key;
      Id keyid;
      key.name = keyname;
      key.type = keytype;
      key.size = 0;
      key.storage = KEY_STORAGE_INCORE;
      data->attriddata = solv_extend(data->attriddata, data->attriddatalen, entrysize + 1, sizeof(Id), REPODATA_ATTRIDDATA_BLOCK);
      keyid = repodata_key2id(data, &key, 1);
      repodata_insert_keyid(data, handle, keyid, data->attriddatalen, 1);
      data->lasthandle = handle;
      data->lastkey = keyid;
      data->lastdatalen = data->attriddatalen + entrysize + 1;
      return;
    }
  oldsize = 0;
  for (ida = data->attriddata + pp[1]; *ida; ida += entrysize)
    oldsize += entrysize;
  if (ida + 1 == data->attriddata + data->attriddatalen)
    {
      /* this was the last entry, just append it */
      data->attriddata = solv_extend(data->attriddata, data->attriddatalen, entrysize, sizeof(Id), REPODATA_ATTRIDDATA_BLOCK);
      data->attriddatalen--;	/* overwrite terminating 0  */
    }
  else
    {
      /* too bad. move to back. */
      data->attriddata = solv_extend(data->attriddata, data->attriddatalen,  oldsize + entrysize + 1, sizeof(Id), REPODATA_ATTRIDDATA_BLOCK);
      memcpy(data->attriddata + data->attriddatalen, data->attriddata + pp[1], oldsize * sizeof(Id));
      pp[1] = data->attriddatalen;
      data->attriddatalen += oldsize;
    }
  data->lasthandle = handle;
  data->lastkey = *pp;
  data->lastdatalen = data->attriddatalen + entrysize + 1;
}