repodata_set_bin_checksum(Repodata *data, Id solvid, Id keyname, Id type,
		      const unsigned char *str)
{
  Repokey key;
  int l;

  if (!(l = solv_chksum_len(type)))
    return;
  key.name = keyname;
  key.type = type;
  key.size = 0;
  key.storage = KEY_STORAGE_INCORE;
  data->attrdata = solv_extend(data->attrdata, data->attrdatalen, l, 1, REPODATA_ATTRDATA_BLOCK);
  memcpy(data->attrdata + data->attrdatalen, str, l);
  repodata_set(data, solvid, &key, data->attrdatalen);
  data->attrdatalen += l;
}