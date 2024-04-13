repodata_disable_paging(Repodata *data)
{
  if (maybe_load_repodata(data, 0))
    {
      repopagestore_disable_paging(&data->store);
      data->storestate++;
    }
}