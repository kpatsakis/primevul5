dataiterator_skip_solvable(Dataiterator *di)
{
  di->nparents = 0;
  di->kv.parent = 0;
  di->rootlevel = 0;
  di->keyname = di->keynames[0];
  di->state = di_nextsolvable;
}