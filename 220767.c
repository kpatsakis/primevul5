repodata_merge_some_attrs(Repodata *data, Id dest, Id src, Map *keyidmap, int overwrite)
{
  Id *keyp;
  if (dest == src || !data->attrs || !(keyp = data->attrs[src - data->start]))
    return;
  for (; *keyp; keyp += 2)
    if (!keyidmap || MAPTST(keyidmap, keyp[0]))
      repodata_insert_keyid(data, dest, keyp[0], keyp[1], overwrite);
}