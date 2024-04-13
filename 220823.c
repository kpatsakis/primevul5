get_vertical_data(Repodata *data, Repokey *key, Id off, Id len)
{
  unsigned char *dp;
  if (len <= 0)
    return 0;
  if (off >= data->lastverticaloffset)
    {
      off -= data->lastverticaloffset;
      if ((unsigned int)off + len > data->vincorelen)
	return 0;
      return data->vincore + off;
    }
  if ((unsigned int)off + len > key->size)
    return 0;
  /* we now have the offset, go into vertical */
  off += data->verticaloffset[key - data->keys];
  /* fprintf(stderr, "key %d page %d\n", key->name, off / REPOPAGE_BLOBSIZE); */
  dp = repopagestore_load_page_range(&data->store, off / REPOPAGE_BLOBSIZE, (off + len - 1) / REPOPAGE_BLOBSIZE);
  data->storestate++;
  if (dp)
    dp += off % REPOPAGE_BLOBSIZE;
  return dp;
}