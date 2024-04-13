repodata_free_schemahash(Repodata *data)
{
  data->schematahash = solv_free(data->schematahash);
  /* shrink arrays */
  data->schemata = solv_realloc2(data->schemata, data->nschemata, sizeof(Id));
  data->schemadata = solv_realloc2(data->schemadata, data->schemadatalen, sizeof(Id));
}