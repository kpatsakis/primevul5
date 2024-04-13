repodata_add_stub(Repodata **datap)
{
  Repodata *data = *datap;
  Repo *repo = data->repo;
  Id repodataid = data - repo->repodata;
  Repodata *sdata = repo_add_repodata(repo, 0);
  data = repo->repodata + repodataid;
  if (data->end > data->start)
    repodata_extend_block(sdata, data->start, data->end - data->start);
  sdata->state = REPODATA_STUB;
  sdata->loadcallback = repodata_stub_loader;
  *datap = data;
  return sdata;
}