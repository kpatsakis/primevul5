Item_func_isnull::remove_eq_conds(THD *thd, Item::cond_result *cond_value,
                                  bool top_level_arg)
{
  Item *real_item= args[0]->real_item();
  if (real_item->type() == Item::FIELD_ITEM)
  {
    Field *field= ((Item_field*) real_item)->field;

    if (((field->type() == MYSQL_TYPE_DATE) ||
         (field->type() == MYSQL_TYPE_DATETIME)) &&
         (field->flags & NOT_NULL_FLAG))
    {
      /* fix to replace 'NULL' dates with '0' (shreeve@uci.edu) */
      /*
        See BUG#12594011
        Documentation says that
        SELECT datetime_notnull d FROM t1 WHERE d IS NULL
        shall return rows where d=='0000-00-00'

        Thus, for DATE and DATETIME columns defined as NOT NULL,
        "date_notnull IS NULL" has to be modified to
        "date_notnull IS NULL OR date_notnull == 0" (if outer join)
        "date_notnull == 0"                         (otherwise)

      */

      Item *item0= new(thd->mem_root) Item_int(thd, (longlong) 0, 1);
      Item *eq_cond= new(thd->mem_root) Item_func_eq(thd, args[0], item0);
      if (!eq_cond)
        return this;

      COND *cond= this;
      if (field->table->pos_in_table_list->is_inner_table_of_outer_join())
      {
        // outer join: transform "col IS NULL" to "col IS NULL or col=0"
        Item *or_cond= new(thd->mem_root) Item_cond_or(thd, eq_cond, this);
        if (!or_cond)
          return this;
        cond= or_cond;
      }
      else
      {
        // not outer join: transform "col IS NULL" to "col=0"
        cond= eq_cond;
      }

      cond->fix_fields(thd, &cond);
      /*
        Note: although args[0] is a field, cond can still be a constant
        (in case field is a part of a dependent subquery).

        Note: we call cond->Item::remove_eq_conds() non-virtually (statically)
        for performance purpose.
        A non-qualified call, i.e. just cond->remove_eq_conds(),
        would call Item_bool_func2::remove_eq_conds() instead, which would
        try to do some extra job to detect if args[0] and args[1] are
        equivalent items. We know they are not (we have field=0 here).
      */
      return cond->Item::remove_eq_conds(thd, cond_value, false);
    }

    /*
      Handles this special case for some ODBC applications:
      The are requesting the row that was just updated with a auto_increment
      value with this construct:

      SELECT * from table_name where auto_increment_column IS NULL
      This will be changed to:
      SELECT * from table_name where auto_increment_column = LAST_INSERT_ID

      Note, this substitution is done if the NULL test is the only condition!
      If the NULL test is a part of a more complex condition, it is not
      substituted and is treated normally:
        WHERE auto_increment IS NULL AND something_else
    */

    if (top_level_arg) // "auto_increment_column IS NULL" is the only condition
    {
      if (field->flags & AUTO_INCREMENT_FLAG && !field->table->maybe_null &&
          (thd->variables.option_bits & OPTION_AUTO_IS_NULL) &&
          (thd->first_successful_insert_id_in_prev_stmt > 0 &&
           thd->substitute_null_with_insert_id))
      {
  #ifdef HAVE_QUERY_CACHE
        query_cache_abort(thd, &thd->query_cache_tls);
  #endif
        COND *new_cond, *cond= this;
        if ((new_cond= new (thd->mem_root) Item_func_eq(thd, args[0],
                                        new (thd->mem_root) Item_int(thd, "last_insert_id()",
                                                     thd->read_first_successful_insert_id_in_prev_stmt(),
                                                     MY_INT64_NUM_DECIMAL_DIGITS))))
        {
          cond= new_cond;
          /*
            Item_func_eq can't be fixed after creation so we do not check
            cond->fixed, also it do not need tables so we use 0 as second
            argument.
          */
          cond->fix_fields(thd, &cond);
        }
        /*
          IS NULL should be mapped to LAST_INSERT_ID only for first row, so
          clear for next row
        */
        thd->substitute_null_with_insert_id= FALSE;

        *cond_value= Item::COND_OK;
        return cond;
      }
    }
  }
  return Item::remove_eq_conds(thd, cond_value, top_level_arg);
}