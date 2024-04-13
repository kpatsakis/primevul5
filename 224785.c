build_aliasvar (self)
     SHELL_VAR *self;
{
  HASH_TABLE *h;
  int i;
  char *k, *v;
  BUCKET_CONTENTS *item;

  h = assoc_cell (self);
  if (h)
    assoc_dispose (h);

  if (aliases == 0 || HASH_ENTRIES (aliases) == 0)
    {
      var_setvalue (self, (char *)NULL);
      return self;
    }

  h = assoc_create (aliases->nbuckets);
  for (i = 0; i < aliases->nbuckets; i++)
    {
      for (item = hash_items (i, aliases); item; item = item->next)
	{
	  k = savestring (item->key);
	  v = ((alias_t *)(item->data))->value;
	  assoc_insert (h, k, v);
	}
    }

  var_setvalue (self, (char *)h);
  return self;
}