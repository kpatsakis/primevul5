find_key_data(Repodata *data, Id solvid, Id keyname, Repokey **keypp)
{
  unsigned char *dp;
  Id schema, *keyp, *kp;
  Repokey *key;

  if (!maybe_load_repodata(data, keyname))
    return 0;
  dp = solvid2data(data, solvid, &schema);
  if (!dp)
    return 0;
  keyp = data->schemadata + data->schemata[schema];
  for (kp = keyp; *kp; kp++)
    if (data->keys[*kp].name == keyname)
      break;
  if (!*kp)
    return 0;
  *keypp = key = data->keys + *kp;
  if (key->type == REPOKEY_TYPE_DELETED)
    return 0;
  if (key->type == REPOKEY_TYPE_VOID || key->type == REPOKEY_TYPE_CONSTANT || key->type == REPOKEY_TYPE_CONSTANTID)
    return dp;	/* no need to forward... */
  if (key->storage != KEY_STORAGE_INCORE && key->storage != KEY_STORAGE_VERTICAL_OFFSET)
    return 0;	/* get_data will not work, no need to forward */
  dp = forward_to_key(data, *kp, keyp, dp);
  if (!dp)
    return 0;
  return get_data(data, key, &dp, 0);
}