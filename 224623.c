build_hashcmd (self)
     SHELL_VAR *self;
{
  HASH_TABLE *h;
  int i;
  char *k, *v;
  BUCKET_CONTENTS *item;

  h = assoc_cell (self);
  if (h)
    assoc_dispose (h);

  if (hashed_filenames == 0 || HASH_ENTRIES (hashed_filenames) == 0)
    {
      var_setvalue (self, (char *)NULL);
      return self;
    }

  h = assoc_create (hashed_filenames->nbuckets);
  for (i = 0; i < hashed_filenames->nbuckets; i++)
    {
      for (item = hash_items (i, hashed_filenames); item; item = item->next)
	{
	  k = savestring (item->key);
	  v = pathdata(item)->path;
	  assoc_insert (h, k, v);
	}
    }

  var_setvalue (self, (char *)h);
  return self;
}