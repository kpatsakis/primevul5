dataiterator_clonepos(Dataiterator *di, Dataiterator *from)
{
  di->state = from->state;
  di->flags &= ~SEARCH_THISSOLVID;
  di->flags |= (from->flags & SEARCH_THISSOLVID);
  di->repo = from->repo;
  di->data = from->data;
  di->dp = from->dp;
  di->ddp = from->ddp;
  di->idp = from->idp;
  di->keyp = from->keyp;
  di->key = from->key;
  di->kv = from->kv;
  di->repodataid = from->repodataid;
  di->solvid = from->solvid;
  di->repoid = from->repoid;
  di->rootlevel = from->rootlevel;
  memcpy(di->parents, from->parents, sizeof(from->parents));
  di->nparents = from->nparents;
  if (di->nparents)
    {
      int i;
      for (i = 1; i < di->nparents; i++)
	di->parents[i].kv.parent = &di->parents[i - 1].kv;
      di->kv.parent = &di->parents[di->nparents - 1].kv;
    }
  di->dupstr = 0;
  di->dupstrn = 0;
  if (from->dupstr && from->dupstr == from->kv.str)
    {
      di->dupstrn = from->dupstrn;
      di->dupstr = solv_memdup(from->dupstr, from->dupstrn);
    }
}