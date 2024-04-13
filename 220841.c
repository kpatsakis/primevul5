repodata_set_num(Repodata *data, Id solvid, Id keyname, unsigned long long num)
{
  Repokey key;
  key.name = keyname;
  key.type = REPOKEY_TYPE_NUM;
  key.size = 0;
  key.storage = KEY_STORAGE_INCORE;
  if (num >= 0x80000000)
    {
      data->attrnum64data = solv_extend(data->attrnum64data, data->attrnum64datalen, 1, sizeof(unsigned long long), REPODATA_ATTRNUM64DATA_BLOCK);
      data->attrnum64data[data->attrnum64datalen] = num;
      num = 0x80000000 | data->attrnum64datalen++;
    }
  repodata_set(data, solvid, &key, (Id)num);
}