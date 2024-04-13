repodata_set_binary(Repodata *data, Id solvid, Id keyname, void *buf, int len)
{
  Repokey key;
  unsigned char *dp;

  if (len < 0)
    return;
  key.name = keyname;
  key.type = REPOKEY_TYPE_BINARY;
  key.size = 0;
  key.storage = KEY_STORAGE_INCORE;
  data->attrdata = solv_extend(data->attrdata, data->attrdatalen, len + 5, 1, REPODATA_ATTRDATA_BLOCK);
  dp = data->attrdata + data->attrdatalen;
  if (len >= (1 << 14))
    {
      if (len >= (1 << 28))
        *dp++ = (len >> 28) | 128;
      if (len >= (1 << 21))
        *dp++ = (len >> 21) | 128;
      *dp++ = (len >> 14) | 128;
    }
  if (len >= (1 << 7))
    *dp++ = (len >> 7) | 128;
  *dp++ = len & 127;
  if (len)
    memcpy(dp, buf, len);
  repodata_set(data, solvid, &key, data->attrdatalen);
  data->attrdatalen = dp + len - data->attrdata;
}