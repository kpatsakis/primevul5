bool tdc_open_view(THD *thd, TABLE_LIST *table_list, uint flags)
{
  TABLE not_used;
  TABLE_SHARE *share;
  bool err= TRUE;

  if (!(share= tdc_acquire_share(thd, table_list, GTS_VIEW)))
    return TRUE;

  DBUG_ASSERT(share->is_view);

  if (flags & CHECK_METADATA_VERSION)
  {
    /*
      Check TABLE_SHARE-version of view only if we have been instructed to do
      so. We do not need to check the version if we're executing CREATE VIEW or
      ALTER VIEW statements.

      In the future, this functionality should be moved out from
      tdc_open_view(), and  tdc_open_view() should became a part of a clean
      table-definition-cache interface.
    */
    if (check_and_update_table_version(thd, table_list, share))
      goto ret;
  }

  err= mysql_make_view(thd, share, table_list, (flags & OPEN_VIEW_NO_PARSE));
ret:
  tdc_release_share(share);

  return err;
}