static my_bool tc_collect_used_shares(TDC_element *element,
                                      tc_collect_arg *arg)
{
  my_bool result= FALSE;

  DYNAMIC_ARRAY *shares= &arg->shares;
  mysql_mutex_lock(&element->LOCK_table_share);
  if (element->ref_count > 0 && !element->share->is_view)
  {
    DBUG_ASSERT(element->share);
    bool do_flush= 0;
    switch (arg->flush_type) {
    case FLUSH_ALL:
      do_flush= 1;
      break;
    case FLUSH_NON_TRANS_TABLES:
      if (!element->share->online_backup &&
          element->share->table_category == TABLE_CATEGORY_USER)
        do_flush= 1;
      break;
    case FLUSH_SYS_TABLES:
      if (!element->share->online_backup &&
          element->share->table_category != TABLE_CATEGORY_USER)
        do_flush= 1;
    }
    if (do_flush)
    {
      element->ref_count++;                       // Protect against delete
      if (push_dynamic(shares, (uchar*) &element->share))
        result= TRUE;
    }
  }
  mysql_mutex_unlock(&element->LOCK_table_share);
  return result;
}