void update_non_unique_table_error(TABLE_LIST *update,
                                   const char *operation,
                                   TABLE_LIST *duplicate)
{
  update= update->top_table();
  duplicate= duplicate->top_table();
  if (!update->view || !duplicate->view ||
      update->view == duplicate->view ||
      update->view_name.length != duplicate->view_name.length ||
      update->view_db.length != duplicate->view_db.length ||
      my_strcasecmp(table_alias_charset,
                    update->view_name.str, duplicate->view_name.str) != 0 ||
      my_strcasecmp(table_alias_charset,
                    update->view_db.str, duplicate->view_db.str) != 0)
  {
    /*
      it is not the same view repeated (but it can be parts of the same copy
      of view), so we have to hide underlying tables.
    */
    if (update->view)
    {
      /* Issue the ER_NON_INSERTABLE_TABLE error for an INSERT */
      if (update->view == duplicate->view)
        my_error(!strncmp(operation, "INSERT", 6) ?
                 ER_NON_INSERTABLE_TABLE : ER_NON_UPDATABLE_TABLE, MYF(0),
                 update->alias, operation);
      else
        my_error(ER_VIEW_PREVENT_UPDATE, MYF(0),
                 (duplicate->view ? duplicate->alias : update->alias),
                 operation, update->alias);
      return;
    }
    if (duplicate->view)
    {
      my_error(ER_VIEW_PREVENT_UPDATE, MYF(0), duplicate->alias, operation,
               update->alias);
      return;
    }
  }
  my_error(ER_UPDATE_TABLE_USED, MYF(0), update->alias, operation);
}