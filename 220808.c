dataiterator_prepend_keyname(Dataiterator *di, Id keyname)
{
  int i;

  if (di->nkeynames >= sizeof(di->keynames)/sizeof(*di->keynames) - 2)
    {
      di->state = di_bye;	/* sorry */
      return;
    }
  for (i = di->nkeynames + 1; i > 0; i--)
    di->keynames[i] = di->keynames[i - 1];
  di->keynames[0] = di->keyname = keyname;
  di->nkeynames++;
}