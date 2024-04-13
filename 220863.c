calculate_keylink(Repodata *data)
{
  int i, j;
  Id *link;
  Id maxkeyname = 0, *keytable = 0;
  link = solv_calloc(data->nkeys, sizeof(Id));
  if (data->nkeys <= 2)
    return link;
  for (i = 1; i < data->nkeys; i++)
    {
      Id n = data->keys[i].name;
      if (n >= maxkeyname)
	{
	  keytable = solv_realloc2(keytable, n + 128, sizeof(Id));
	  memset(keytable + maxkeyname, 0, (n + 128 - maxkeyname) * sizeof(Id));
	  maxkeyname = n + 128;
	}
      j = keytable[n];
      if (j)
	link[i] = link[j];
      else
	j = i;
      link[j] = i;
      keytable[n] = i;
    }
  /* remove links that just point to themselfs */
  for (i = 1; i < data->nkeys; i++)
    if (link[i] == i)
      link[i] = 0;
  solv_free(keytable);
  return link;
}