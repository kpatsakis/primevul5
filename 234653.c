JOIN_TAB::sort_table()
{
  int rc;
  DBUG_PRINT("info",("Sorting for index"));
  THD_STAGE_INFO(join->thd, stage_creating_sort_index);
  DBUG_ASSERT(join->ordered_index_usage != (filesort->order == join->order ?
                                            JOIN::ordered_index_order_by :
                                            JOIN::ordered_index_group_by));
  rc= create_sort_index(join->thd, join, this, NULL);
  return (rc != 0);
}