repodata_merge_attrs(Repodata *data, Id dest, Id src)
{
  Id *keyp;
  if (dest == src || !data->attrs || !(keyp = data->attrs[src - data->start]))
    return;
  for (; *keyp; keyp += 2)
    repodata_insert_keyid(data, dest, keyp[0], keyp[1], 0);
}