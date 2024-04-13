void free_rows(MYSQL_DATA *cur)
{
  if (cur)
  {
    ma_free_root(&cur->alloc,MYF(0));
    free(cur);
  }
}