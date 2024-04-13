dataiterator_find_keyname(Dataiterator *di, Id keyname)
{
  Id *keyp;
  Repokey *keys = di->data->keys, *key;
  unsigned char *dp;

  for (keyp = di->keyp; *keyp; keyp++)
    if (keys[*keyp].name == keyname)
      break;
  if (!*keyp)
    return 0;
  key = keys + *keyp;
  if (key->type == REPOKEY_TYPE_DELETED)
    return 0;
  if (key->storage != KEY_STORAGE_INCORE && key->storage != KEY_STORAGE_VERTICAL_OFFSET)
    return 0;		/* get_data will not work, no need to forward */
  dp = forward_to_key(di->data, *keyp, di->keyp, di->dp);
  if (!dp)
    return 0;
  di->keyp = keyp;
  return dp;
}