dataiterator_jump_to_repo(Dataiterator *di, Repo *repo)
{
  di->nparents = 0;
  di->kv.parent = 0;
  di->rootlevel = 0;
  di->repo = repo;
  di->repoid = 0;	/* 0 means stay at repo */
  di->repodataid = 1;
  di->solvid = 0;
  di->flags &= ~SEARCH_THISSOLVID;
  di->state = di_enterrepo;
}