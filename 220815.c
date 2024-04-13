repodata_add_flexarray(Repodata *data, Id solvid, Id keyname, Id handle)
{
  repodata_add_array(data, solvid, keyname, REPOKEY_TYPE_FLEXARRAY, 1);
  data->attriddata[data->attriddatalen++] = handle;
  data->attriddata[data->attriddatalen++] = 0;
}