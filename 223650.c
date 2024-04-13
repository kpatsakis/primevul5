hivex_close (hive_h *h)
{
  int r;

  DEBUG (1, "hivex_close");

  free (h->bitmap);
  if (!h->writable)
    munmap (h->addr, h->size);
  else
    free (h->addr);
  if (h->fd >= 0)
    r = close (h->fd);
  else
    r = 0;
  free (h->filename);
  for (int t=0; t<nr_recode_types; t++) {
    if (h->iconv_cache[t].handle != NULL) {
      iconv_close (h->iconv_cache[t].handle);
      h->iconv_cache[t].handle = NULL;
    }
  }
  free (h);

  return r;
}