bool is_locked_view(THD *thd, TABLE_LIST *t)
{
  DBUG_ENTER("check_locked_view");
  /*
   Is this table a view and not a base table?
   (it is work around to allow to open view with locked tables,
   real fix will be made after definition cache will be made)

   Since opening of view which was not explicitly locked by LOCK
   TABLES breaks metadata locking protocol (potentially can lead
   to deadlocks) it should be disallowed.
  */
  if (thd->mdl_context.is_lock_owner(MDL_key::TABLE,
                                     t->db, t->table_name,
                                     MDL_SHARED))
  {
    char path[FN_REFLEN + 1];
    build_table_filename(path, sizeof(path) - 1,
                         t->db, t->table_name, reg_ext, 0);
    /*
      Note that we can't be 100% sure that it is a view since it's
      possible that we either simply have not found unused TABLE
      instance in THD::open_tables list or were unable to open table
      during prelocking process (in this case in theory we still
      should hold shared metadata lock on it).
    */
    if (dd_frm_is_view(thd, path))
    {
      /*
        If parent_l of the table_list is non null then a merge table
        has this view as child table, which is not supported.
      */
      if (t->parent_l)
      {
        my_error(ER_WRONG_MRG_TABLE, MYF(0));
        DBUG_RETURN(FALSE);
      }

      if (!tdc_open_view(thd, t, CHECK_METADATA_VERSION))
      {
        DBUG_ASSERT(t->view != 0);
        DBUG_RETURN(TRUE); // VIEW
      }
    }
  }

  DBUG_RETURN(FALSE);
}