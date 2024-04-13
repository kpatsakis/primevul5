repodata_add_idarray(Repodata *data, Id solvid, Id keyname, Id id)
{
#if 0
fprintf(stderr, "repodata_add_idarray %d %d (%d)\n", solvid, id, data->attriddatalen);
#endif
  repodata_add_array(data, solvid, keyname, REPOKEY_TYPE_IDARRAY, 1);
  data->attriddata[data->attriddatalen++] = id;
  data->attriddata[data->attriddatalen++] = 0;
}