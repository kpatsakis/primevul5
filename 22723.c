onig_free_shared_cclass_table(void)
{
  THREAD_ATOMIC_START;
  if (IS_NOT_NULL(OnigTypeCClassTable)) {
    onig_st_foreach(OnigTypeCClassTable, i_free_shared_class, 0);
    onig_st_free_table(OnigTypeCClassTable);
    OnigTypeCClassTable = NULL;
  }
  THREAD_ATOMIC_END;

  return 0;
}