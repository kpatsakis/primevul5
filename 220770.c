repodata_free(Repodata *data)
{
  Repo *repo = data->repo;
  int i = data - repo->repodata;
  if (i == 0)
    return;
  repodata_freedata(data);
  if (i < repo->nrepodata - 1)
    {
      /* whoa! this changes the repodataids! */
      memmove(repo->repodata + i, repo->repodata + i + 1, (repo->nrepodata - 1 - i) * sizeof(Repodata));
      for (; i < repo->nrepodata - 1; i++)
	repo->repodata[i].repodataid = i;
    }
  repo->nrepodata--;
  if (repo->nrepodata == 1)
    {
      repo->repodata = solv_free(repo->repodata);
      repo->nrepodata = 0;
    }
}