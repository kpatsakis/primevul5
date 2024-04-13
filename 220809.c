repodata_memused(Repodata *data)
{
  return data->incoredatalen + data->vincorelen;
}