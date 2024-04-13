repodata_add_dirnumnum(Repodata *data, Id solvid, Id keyname, Id dir, Id num, Id num2)
{
  assert(dir);
#if 0
fprintf(stderr, "repodata_add_dirnumnum %d %d %d %d (%d)\n", solvid, dir, num, num2, data->attriddatalen);
#endif
  repodata_add_array(data, solvid, keyname, REPOKEY_TYPE_DIRNUMNUMARRAY, 3);
  data->attriddata[data->attriddatalen++] = dir;
  data->attriddata[data->attriddatalen++] = num;
  data->attriddata[data->attriddatalen++] = num2;
  data->attriddata[data->attriddatalen++] = 0;
}