repodata_free_dircache(Repodata *data)
{
  data->dircache = solv_free(data->dircache);
}