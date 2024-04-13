find_field_in_table_ref(THD *thd, TABLE_LIST *table_list,
                        const char *name, uint length,
                        const char *item_name, const char *db_name,
                        const char *table_name, Item **ref,
                        bool check_privileges, bool allow_rowid,
                        uint *cached_field_index_ptr,
                        bool register_tree_change, TABLE_LIST **actual_table)
{
  Field *fld;
  DBUG_ENTER("find_field_in_table_ref");
  DBUG_ASSERT(table_list->alias);
  DBUG_ASSERT(name);
  DBUG_ASSERT(item_name);
  DBUG_PRINT("enter",
             ("table: '%s'  field name: '%s'  item name: '%s'  ref %p",
              table_list->alias, name, item_name, ref));

  /*
    Check that the table and database that qualify the current field name
    are the same as the table reference we are going to search for the field.

    Exclude from the test below nested joins because the columns in a
    nested join generally originate from different tables. Nested joins
    also have no table name, except when a nested join is a merge view
    or an information schema table.

    We include explicitly table references with a 'field_translation' table,
    because if there are views over natural joins we don't want to search
    inside the view, but we want to search directly in the view columns
    which are represented as a 'field_translation'.

    TODO: Ensure that table_name, db_name and tables->db always points to
          something !
  */
  if (/* Exclude nested joins. */
      (!table_list->nested_join ||
       /* Include merge views and information schema tables. */
       table_list->field_translation) &&
      /*
        Test if the field qualifiers match the table reference we plan
        to search.
      */
      table_name && table_name[0] &&
      (my_strcasecmp(table_alias_charset, table_list->alias, table_name) ||
       (db_name && db_name[0] && (!table_list->db || !table_list->db[0])) ||
       (db_name && db_name[0] && table_list->db && table_list->db[0] &&
        (table_list->schema_table ?
         my_strcasecmp(system_charset_info, db_name, table_list->db) :
         strcmp(db_name, table_list->db)))))
    DBUG_RETURN(0);

  *actual_table= NULL;

  if (table_list->field_translation)
  {
    /* 'table_list' is a view or an information schema table. */
    if ((fld= find_field_in_view(thd, table_list, name, length, item_name, ref,
                                 register_tree_change)))
      *actual_table= table_list;
  }
  else if (!table_list->nested_join)
  {
    /* 'table_list' is a stored table. */
    DBUG_ASSERT(table_list->table);
    if ((fld= find_field_in_table(thd, table_list->table, name, length,
                                  allow_rowid,
                                  cached_field_index_ptr)))
      *actual_table= table_list;
  }
  else
  {
    /*
      'table_list' is a NATURAL/USING join, or an operand of such join that
      is a nested join itself.

      If the field name we search for is qualified, then search for the field
      in the table references used by NATURAL/USING the join.
    */
    if (table_name && table_name[0])
    {
      List_iterator<TABLE_LIST> it(table_list->nested_join->join_list);
      TABLE_LIST *table;
      while ((table= it++))
      {
        if ((fld= find_field_in_table_ref(thd, table, name, length, item_name,
                                          db_name, table_name, ref,
                                          check_privileges, allow_rowid,
                                          cached_field_index_ptr,
                                          register_tree_change, actual_table)))
          DBUG_RETURN(fld);
      }
      DBUG_RETURN(0);
    }
    /*
      Non-qualified field, search directly in the result columns of the
      natural join. The condition of the outer IF is true for the top-most
      natural join, thus if the field is not qualified, we will search
      directly the top-most NATURAL/USING join.
    */
    fld= find_field_in_natural_join(thd, table_list, name, length, ref,
                                    register_tree_change, actual_table);
  }

  if (fld)
  {
#ifndef NO_EMBEDDED_ACCESS_CHECKS
    /* Check if there are sufficient access rights to the found field. */
    if (check_privileges &&
        !table_list->is_derived() &&
        check_column_grant_in_table_ref(thd, *actual_table, name, length))
      fld= WRONG_GRANT;
    else
#endif
      if (thd->mark_used_columns != MARK_COLUMNS_NONE)
      {
        /*
          Get rw_set correct for this field so that the handler
          knows that this field is involved in the query and gets
          retrieved/updated
         */
        Field *field_to_set= NULL;
        if (fld == view_ref_found)
        {
          if (!ref)
            DBUG_RETURN(fld);
          Item *it= (*ref)->real_item();
          if (it->type() == Item::FIELD_ITEM)
            field_to_set= ((Item_field*)it)->field;
          else
          {
            if (thd->mark_used_columns == MARK_COLUMNS_READ)
              it->walk(&Item::register_field_in_read_map, 0, 0);
            else
              it->walk(&Item::register_field_in_write_map, 0, 0);
          }
        }
        else
          field_to_set= fld;
        if (field_to_set)
        {
          TABLE *table= field_to_set->table;
          if (thd->mark_used_columns == MARK_COLUMNS_READ)
            field_to_set->register_field_in_read_map();
          else
            bitmap_set_bit(table->write_set, field_to_set->field_index);
        }
      }
  }
  DBUG_RETURN(fld);
}