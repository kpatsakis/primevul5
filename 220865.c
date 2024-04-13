repodata_add_fixarray(Repodata *data, Id solvid, Id keyname, Id handle)
{
  repodata_add_array(data, solvid, keyname, REPOKEY_TYPE_FIXARRAY, 1);
  data->attriddata[data->attriddatalen++] = handle;
  data->attriddata[data->attriddatalen++] = 0;
}