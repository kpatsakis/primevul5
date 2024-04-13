dataiterator_set_keyname(Dataiterator *di, Id keyname)
{
  di->nkeynames = 0;
  di->keyname = keyname;
  di->keynames[0] = keyname;
}