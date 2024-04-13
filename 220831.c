repodata_lookup_schemakeys(Repodata *data, Id solvid)
{
  Id schema;
  if (!maybe_load_repodata(data, 0))
    return 0;
  if (!solvid2data(data, solvid, &schema))
    return 0;
  return data->schemadata + data->schemata[schema];
}