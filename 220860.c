repodata_set_idarray(Repodata *data, Id solvid, Id keyname, Queue *q)
{
  Repokey key;
  int i;

  key.name = keyname;
  key.type = REPOKEY_TYPE_IDARRAY;
  key.size = 0;
  key.storage = KEY_STORAGE_INCORE;
  repodata_set(data, solvid, &key, data->attriddatalen);
  data->attriddata = solv_extend(data->attriddata, data->attriddatalen, q->count + 1, sizeof(Id), REPODATA_ATTRIDDATA_BLOCK);
  for (i = 0; i < q->count; i++)
    data->attriddata[data->attriddatalen++] = q->elements[i];
  data->attriddata[data->attriddatalen++] = 0;
}