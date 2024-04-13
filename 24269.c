store_natural_using_join_columns(THD *thd, TABLE_LIST *natural_using_join,
                                 TABLE_LIST *table_ref_1,
                                 TABLE_LIST *table_ref_2,
                                 List<String> *using_fields,
                                 uint found_using_fields)
{
  Field_iterator_table_ref it_1, it_2;
  Natural_join_column *nj_col_1, *nj_col_2;
  Query_arena *arena, backup;
  bool result= TRUE;
  List<Natural_join_column> *non_join_columns;
  List<Natural_join_column> *join_columns;
  DBUG_ENTER("store_natural_using_join_columns");

  DBUG_ASSERT(!natural_using_join->join_columns);

  arena= thd->activate_stmt_arena_if_needed(&backup);

  if (!(non_join_columns= new List<Natural_join_column>) ||
      !(join_columns= new List<Natural_join_column>))
    goto err;

  /* Append the columns of the first join operand. */
  for (it_1.set(table_ref_1); !it_1.end_of_fields(); it_1.next())
  {
    nj_col_1= it_1.get_natural_column_ref();
    if (nj_col_1->is_common)
    {
      join_columns->push_back(nj_col_1, thd->mem_root);
      /* Reset the common columns for the next call to mark_common_columns. */
      nj_col_1->is_common= FALSE;
    }
    else
      non_join_columns->push_back(nj_col_1, thd->mem_root);
  }

  /*
    Check that all columns in the USING clause are among the common
    columns. If this is not the case, report the first one that was
    not found in an error.
  */
  if (using_fields && found_using_fields < using_fields->elements)
  {
    String *using_field_name;
    List_iterator_fast<String> using_fields_it(*using_fields);
    while ((using_field_name= using_fields_it++))
    {
      const char *using_field_name_ptr= using_field_name->c_ptr();
      List_iterator_fast<Natural_join_column>
        it(*join_columns);
      Natural_join_column *common_field;

      for (;;)
      {
        /* If reached the end of fields, and none was found, report error. */
        if (!(common_field= it++))
        {
          my_error(ER_BAD_FIELD_ERROR, MYF(0), using_field_name_ptr,
                   current_thd->where);
          goto err;
        }
        if (!my_strcasecmp(system_charset_info,
                           common_field->name(), using_field_name_ptr))
          break;                                // Found match
      }
    }
  }

  /* Append the non-equi-join columns of the second join operand. */
  for (it_2.set(table_ref_2); !it_2.end_of_fields(); it_2.next())
  {
    nj_col_2= it_2.get_natural_column_ref();
    if (!nj_col_2->is_common)
      non_join_columns->push_back(nj_col_2, thd->mem_root);
    else
    {
      /* Reset the common columns for the next call to mark_common_columns. */
      nj_col_2->is_common= FALSE;
    }
  }

  if (non_join_columns->elements > 0)
    join_columns->append(non_join_columns);
  natural_using_join->join_columns= join_columns;
  natural_using_join->is_join_columns_complete= TRUE;

  result= FALSE;

  if (arena)
    thd->restore_active_arena(arena, &backup);
  DBUG_RETURN(result);

err:
  /*
     Actually we failed to build join columns list, so we have to
     clear it to avoid problems with half-build join on next run.
     The list was created in mark_common_columns().
   */
  table_ref_1->remove_join_columns();
  table_ref_2->remove_join_columns();

  if (arena)
    thd->restore_active_arena(arena, &backup);
  DBUG_RETURN(TRUE);
}