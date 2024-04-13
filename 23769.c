kwsfree (kwset_t kwset)
{
  obstack_free (&kwset->obstack, NULL);
  free (kwset);
}