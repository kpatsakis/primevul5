repodata_lookup_type(Repodata *data, Id solvid, Id keyname)
{
  Id schema, *keyp, *kp;
  if (!maybe_load_repodata(data, keyname))
    return 0;
  if (!solvid2data(data, solvid, &schema))
    return 0;
  keyp = data->schemadata + data->schemata[schema];
  for (kp = keyp; *kp; kp++)
    if (data->keys[*kp].name == keyname)
      return data->keys[*kp].type;
  return 0;
}