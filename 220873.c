dataiterator_jump_to_solvid(Dataiterator *di, Id solvid)
{
  di->nparents = 0;
  di->kv.parent = 0;
  di->rootlevel = 0;
  di->keyname = di->keynames[0];
  if (solvid == SOLVID_POS)
    {
      di->repo = di->pool->pos.repo;
      if (!di->repo)
	{
	  di->state = di_bye;
	  return;
	}
      di->repoid = 0;
      if (!di->pool->pos.repodataid && di->pool->pos.solvid == SOLVID_META) {
	solvid = SOLVID_META;		/* META pos hack */
      } else {
        di->data = di->repo->repodata + di->pool->pos.repodataid;
        di->repodataid = 0;
      }
    }
  else if (solvid > 0)
    {
      di->repo = di->pool->solvables[solvid].repo;
      di->repoid = 0;
    }
  if (di->repoid > 0)
    {
      if (!di->pool->urepos)
	{
	  di->state = di_bye;
	  return;
	}
      di->repoid = 1;
      di->repo = di->pool->repos[di->repoid];
    }
  if (solvid != SOLVID_POS)
    di->repodataid = 1;
  di->solvid = solvid;
  if (solvid)
    di->flags |= SEARCH_THISSOLVID;
  di->state = di_enterrepo;
}