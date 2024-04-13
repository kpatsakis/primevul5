dataiterator_set_search(Dataiterator *di, Repo *repo, Id p)
{
  di->repo = repo;
  di->repoid = 0;
  di->flags &= ~SEARCH_THISSOLVID;
  di->nparents = 0;
  di->rootlevel = 0;
  di->repodataid = 1;
  if (!di->pool->urepos)
    {
      di->state = di_bye;
      return;
    }
  if (!repo)
    {
      di->repoid = 1;
      di->repo = di->pool->repos[di->repoid];
    }
  di->state = di_enterrepo;
  if (p)
    dataiterator_jump_to_solvid(di, p);
}