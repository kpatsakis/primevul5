repodata_key2id(Repodata *data, Repokey *key, int create)
{
  Id keyid;

  for (keyid = 1; keyid < data->nkeys; keyid++)
    if (data->keys[keyid].name == key->name && data->keys[keyid].type == key->type)
      {
        if ((key->type == REPOKEY_TYPE_CONSTANT || key->type == REPOKEY_TYPE_CONSTANTID) && key->size != data->keys[keyid].size)
          continue;
        break;
      }
  if (keyid == data->nkeys)
    {
      if (!create)
	return 0;
      /* allocate new key */
      data->keys = solv_realloc2(data->keys, data->nkeys + 1, sizeof(Repokey));
      data->keys[data->nkeys++] = *key;
      if (data->verticaloffset)
        {
          data->verticaloffset = solv_realloc2(data->verticaloffset, data->nkeys, sizeof(Id));
          data->verticaloffset[data->nkeys - 1] = 0;
        }
      data->keybits[(key->name >> 3) & (sizeof(data->keybits) - 1)] |= 1 << (key->name & 7);
    }
  return keyid;
}