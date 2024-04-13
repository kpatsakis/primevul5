repodata_empty(Repodata *data, int localpool)
{
  void (*loadcallback)(Repodata *) = data->loadcallback;
  int state = data->state;
  repodata_freedata(data);
  repodata_initdata(data, data->repo, localpool);
  data->state = state;
  data->loadcallback = loadcallback;
}