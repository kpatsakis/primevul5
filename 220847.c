repodata_load(Repodata *data)
{
  if (data->state != REPODATA_STUB)
    return;
  if (data->loadcallback)
    data->loadcallback(data);
  else
    data->state = REPODATA_ERROR;
}