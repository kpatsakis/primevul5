repodata_fill_keyskip(Repodata *data, Id solvid, Id *keyskip)
{
  const Id *keyp;
  Id maxkeyname, value;
  keyp = repodata_lookup_schemakeys(data, solvid);
  if (!keyp)
    return keyskip;	/* no keys for this solvid */
  if (!keyskip)
    keyskip = alloc_keyskip();
  maxkeyname = keyskip[0];
  value = keyskip[1] + data->repodataid;
  for (; *keyp; keyp++)
    {
      Id keyname = data->keys[*keyp].name;
      if (keyname >= maxkeyname)
	{
	  int newmax = (keyname | 255) + 1; 
	  keyskip = solv_realloc2(keyskip, 3 + newmax, sizeof(Id));
	  memset(keyskip + (3 + maxkeyname), 0, (newmax - maxkeyname) * sizeof(Id));
	  keyskip[0] = maxkeyname = newmax;
	}
      keyskip[3 + keyname] = value;
    }
  return keyskip;
}