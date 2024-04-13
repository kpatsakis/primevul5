mark_common_columns(THD *thd, TABLE_LIST *table_ref_1, TABLE_LIST *table_ref_2,
                    List<String> *using_fields, uint *found_using_fields)
{
  Field_iterator_table_ref it_1, it_2;
  Natural_join_column *nj_col_1, *nj_col_2;
  Query_arena *arena, backup;
  bool result= TRUE;
  bool first_outer_loop= TRUE;
  /*
    Leaf table references to which new natural join columns are added
    if the leaves are != NULL.
  */
  TABLE_LIST *leaf_1= (table_ref_1->nested_join &&
                       !table_ref_1->is_natural_join) ?
                      NULL : table_ref_1;
  TABLE_LIST *leaf_2= (table_ref_2->nested_join &&
                       !table_ref_2->is_natural_join) ?
                      NULL : table_ref_2;

  DBUG_ENTER("mark_common_columns");
  DBUG_PRINT("info", ("operand_1: %s  operand_2: %s",
                      table_ref_1->alias, table_ref_2->alias));

  *found_using_fields= 0;
  arena= thd->activate_stmt_arena_if_needed(&backup);

  for (it_1.set(table_ref_1); !it_1.end_of_fields(); it_1.next())
  {
    bool found= FALSE;
    const char *field_name_1;
    /* true if field_name_1 is a member of using_fields */
    bool is_using_column_1;
    if (!(nj_col_1= it_1.get_or_create_column_ref(thd, leaf_1)))
      goto err;
    field_name_1= nj_col_1->name();
    is_using_column_1= using_fields && 
      test_if_string_in_list(field_name_1, using_fields);
    DBUG_PRINT ("info", ("field_name_1=%s.%s", 
                         nj_col_1->table_name() ? nj_col_1->table_name() : "", 
                         field_name_1));

    /*
      Find a field with the same name in table_ref_2.

      Note that for the second loop, it_2.set() will iterate over
      table_ref_2->join_columns and not generate any new elements or
      lists.
    */
    nj_col_2= NULL;
    for (it_2.set(table_ref_2); !it_2.end_of_fields(); it_2.next())
    {
      Natural_join_column *cur_nj_col_2;
      const char *cur_field_name_2;
      if (!(cur_nj_col_2= it_2.get_or_create_column_ref(thd, leaf_2)))
        goto err;
      cur_field_name_2= cur_nj_col_2->name();
      DBUG_PRINT ("info", ("cur_field_name_2=%s.%s", 
                           cur_nj_col_2->table_name() ? 
                             cur_nj_col_2->table_name() : "", 
                           cur_field_name_2));

      /*
        Compare the two columns and check for duplicate common fields.
        A common field is duplicate either if it was already found in
        table_ref_2 (then found == TRUE), or if a field in table_ref_2
        was already matched by some previous field in table_ref_1
        (then cur_nj_col_2->is_common == TRUE).
        Note that it is too early to check the columns outside of the
        USING list for ambiguity because they are not actually "referenced"
        here. These columns must be checked only on unqualified reference 
        by name (e.g. in SELECT list).
      */
      if (!my_strcasecmp(system_charset_info, field_name_1, cur_field_name_2))
      {
        DBUG_PRINT ("info", ("match c1.is_common=%d", nj_col_1->is_common));
        if (cur_nj_col_2->is_common ||
            (found && (!using_fields || is_using_column_1)))
        {
          my_error(ER_NON_UNIQ_ERROR, MYF(0), field_name_1, thd->where);
          goto err;
        }
        nj_col_2= cur_nj_col_2;
        found= TRUE;
      }
    }
    if (first_outer_loop && leaf_2)
    {
      /*
        Make sure that the next inner loop "knows" that all columns
        are materialized already.
      */
      leaf_2->is_join_columns_complete= TRUE;
      first_outer_loop= FALSE;
    }
    if (!found)
      continue;                                 // No matching field

    /*
      field_1 and field_2 have the same names. Check if they are in the USING
      clause (if present), mark them as common fields, and add a new
      equi-join condition to the ON clause.
    */
    if (nj_col_2 && (!using_fields ||is_using_column_1))
    {
      /*
        Create non-fixed fully qualified field and let fix_fields to
        resolve it.
      */
      Item *item_1=   nj_col_1->create_item(thd);
      Item *item_2=   nj_col_2->create_item(thd);
      Field *field_1= nj_col_1->field();
      Field *field_2= nj_col_2->field();
      Item_ident *item_ident_1, *item_ident_2;
      Item_func_eq *eq_cond;

      if (!item_1 || !item_2)
        goto err;                               // out of memory

      /*
        The following assert checks that the two created items are of
        type Item_ident.
      */
      DBUG_ASSERT(!thd->lex->current_select->no_wrap_view_item);
      /*
        In the case of no_wrap_view_item == 0, the created items must be
        of sub-classes of Item_ident.
      */
      DBUG_ASSERT(item_1->type() == Item::FIELD_ITEM ||
                  item_1->type() == Item::REF_ITEM);
      DBUG_ASSERT(item_2->type() == Item::FIELD_ITEM ||
                  item_2->type() == Item::REF_ITEM);

      /*
        We need to cast item_1,2 to Item_ident, because we need to hook name
        resolution contexts specific to each item.
      */
      item_ident_1= (Item_ident*) item_1;
      item_ident_2= (Item_ident*) item_2;
      /*
        Create and hook special name resolution contexts to each item in the
        new join condition . We need this to both speed-up subsequent name
        resolution of these items, and to enable proper name resolution of
        the items during the execute phase of PS.
      */
      if (set_new_item_local_context(thd, item_ident_1, nj_col_1->table_ref) ||
          set_new_item_local_context(thd, item_ident_2, nj_col_2->table_ref))
        goto err;

      if (!(eq_cond= new (thd->mem_root) Item_func_eq(thd, item_ident_1, item_ident_2)))
        goto err;                               /* Out of memory. */

      if (field_1 && field_1->vcol_info)
        field_1->table->mark_virtual_col(field_1);
      if (field_2 && field_2->vcol_info)
        field_2->table->mark_virtual_col(field_2);

      /*
        Add the new equi-join condition to the ON clause. Notice that
        fix_fields() is applied to all ON conditions in setup_conds()
        so we don't do it here.
      */
      add_join_on(thd, (table_ref_1->outer_join & JOIN_TYPE_RIGHT ?
                        table_ref_1 : table_ref_2),
                  eq_cond);

      nj_col_1->is_common= nj_col_2->is_common= TRUE;
      DBUG_PRINT ("info", ("%s.%s and %s.%s are common", 
                           nj_col_1->table_name() ? 
                             nj_col_1->table_name() : "", 
                           nj_col_1->name(),
                           nj_col_2->table_name() ? 
                             nj_col_2->table_name() : "", 
                           nj_col_2->name()));

      if (field_1)
      {
        TABLE *table_1= nj_col_1->table_ref->table;
        /* Mark field_1 used for table cache. */
        bitmap_set_bit(table_1->read_set, field_1->field_index);
        table_1->covering_keys.intersect(field_1->part_of_key);
      }
      if (field_2)
      {
        TABLE *table_2= nj_col_2->table_ref->table;
        /* Mark field_2 used for table cache. */
        bitmap_set_bit(table_2->read_set, field_2->field_index);
        table_2->covering_keys.intersect(field_2->part_of_key);
      }

      if (using_fields != NULL)
        ++(*found_using_fields);
    }
  }
  if (leaf_1)
    leaf_1->is_join_columns_complete= TRUE;

  /*
    Everything is OK.
    Notice that at this point there may be some column names in the USING
    clause that are not among the common columns. This is an SQL error and
    we check for this error in store_natural_using_join_columns() when
    (found_using_fields < length(join_using_fields)).
  */
  result= FALSE;

err:
  if (arena)
    thd->restore_active_arena(arena, &backup);
  DBUG_RETURN(result);
}