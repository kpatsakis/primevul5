repodata_lookup_packed_dirstrarray(Repodata *data, Id solvid, Id keyname)
{
  static unsigned char wrongtype[2] = { 0x00 /* dir id 0 */, 0 /* "" */ };
  unsigned char *dp;
  Id schema, *keyp, *kp;
  Repokey *key;

  if (!data->incoredata || !data->incoreoffset[solvid - data->start])
    return 0;
  dp = data->incoredata + data->incoreoffset[solvid - data->start];
  dp = data_read_id(dp, &schema);
  keyp = data->schemadata + data->schemata[schema];
  for (kp = keyp; *kp; kp++)
    if (data->keys[*kp].name == keyname)
      break;
  if (!*kp)
    return 0;
  key = data->keys + *kp;
  if (key->type != REPOKEY_TYPE_DIRSTRARRAY)
    return wrongtype;
  dp = forward_to_key(data, *kp, keyp, dp);
  if (key->storage == KEY_STORAGE_INCORE)
    return dp;
  if (key->storage == KEY_STORAGE_VERTICAL_OFFSET && dp)
    {
      Id off, len;
      dp = data_read_id(dp, &off);
      data_read_id(dp, &len);
      return get_vertical_data(data, key, off, len);
    }
  return 0;
}