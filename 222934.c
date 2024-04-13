static int compare_uid(const void *a, const void *b)
{
  const struct Email *ea = *(struct Email const *const *) a;
  const struct Email *eb = *(struct Email const *const *) b;
  return imap_edata_get((struct Email *) ea)->uid -
         imap_edata_get((struct Email *) eb)->uid;
}