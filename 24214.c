bool setup_fields(THD *thd, Ref_ptr_array ref_pointer_array,
                  List<Item> &fields, enum_mark_columns mark_used_columns,
                  List<Item> *sum_func_list, List<Item> *pre_fix,
                  bool allow_sum_func)
{
  Item *item;
  enum_mark_columns save_mark_used_columns= thd->mark_used_columns;
  nesting_map save_allow_sum_func= thd->lex->allow_sum_func;
  List_iterator<Item> it(fields);
  bool save_is_item_list_lookup;
  bool make_pre_fix= (pre_fix && (pre_fix->elements == 0));
  DBUG_ENTER("setup_fields");
  DBUG_PRINT("enter", ("ref_pointer_array: %p", ref_pointer_array.array()));

  thd->mark_used_columns= mark_used_columns;
  DBUG_PRINT("info", ("thd->mark_used_columns: %d", thd->mark_used_columns));
  if (allow_sum_func)
    thd->lex->allow_sum_func|=
      (nesting_map)1 << thd->lex->current_select->nest_level;
  thd->where= THD::DEFAULT_WHERE;
  save_is_item_list_lookup= thd->lex->current_select->is_item_list_lookup;
  thd->lex->current_select->is_item_list_lookup= 0;

  /*
    To prevent fail on forward lookup we fill it with zerows,
    then if we got pointer on zero after find_item_in_list we will know
    that it is forward lookup.

    There is other way to solve problem: fill array with pointers to list,
    but it will be slower.

    TODO: remove it when (if) we made one list for allfields and
    ref_pointer_array
  */
  if (!ref_pointer_array.is_null())
  {
    DBUG_ASSERT(ref_pointer_array.size() >= fields.elements);
    memset(ref_pointer_array.array(), 0, sizeof(Item *) * fields.elements);
  }

  /*
    We call set_entry() there (before fix_fields() of the whole list of field
    items) because:
    1) the list of field items has same order as in the query, and the
       Item_func_get_user_var item may go before the Item_func_set_user_var:
          SELECT @a, @a := 10 FROM t;
    2) The entry->update_query_id value controls constantness of
       Item_func_get_user_var items, so in presence of Item_func_set_user_var
       items we have to refresh their entries before fixing of
       Item_func_get_user_var items.
  */
  List_iterator<Item_func_set_user_var> li(thd->lex->set_var_list);
  Item_func_set_user_var *var;
  while ((var= li++))
    var->set_entry(thd, FALSE);

  Ref_ptr_array ref= ref_pointer_array;
  thd->lex->current_select->cur_pos_in_select_list= 0;
  while ((item= it++))
  {
    if (make_pre_fix)
      pre_fix->push_back(item, thd->stmt_arena->mem_root);

    if ((!item->fixed && item->fix_fields(thd, it.ref())) ||
	(item= *(it.ref()))->check_cols(1))
    {
      thd->lex->current_select->is_item_list_lookup= save_is_item_list_lookup;
      thd->lex->allow_sum_func= save_allow_sum_func;
      thd->mark_used_columns= save_mark_used_columns;
      DBUG_PRINT("info", ("thd->mark_used_columns: %d", thd->mark_used_columns));
      DBUG_RETURN(TRUE); /* purecov: inspected */
    }
    if (!ref.is_null())
    {
      ref[0]= item;
      ref.pop_front();
    }
    /*
      split_sum_func() must be called for Window Function items, see
      Item_window_func::split_sum_func.
    */
    if (sum_func_list &&
         ((item->with_sum_func && item->type() != Item::SUM_FUNC_ITEM) ||
          item->with_window_func))
    {
      item->split_sum_func(thd, ref_pointer_array, *sum_func_list,
                           SPLIT_SUM_SELECT);
    }
    thd->lex->current_select->select_list_tables|= item->used_tables();
    thd->lex->used_tables|= item->used_tables();
    thd->lex->current_select->cur_pos_in_select_list++;
  }
  thd->lex->current_select->is_item_list_lookup= save_is_item_list_lookup;
  thd->lex->current_select->cur_pos_in_select_list= UNDEF_POS;

  thd->lex->allow_sum_func= save_allow_sum_func;
  thd->mark_used_columns= save_mark_used_columns;
  DBUG_PRINT("info", ("thd->mark_used_columns: %d", thd->mark_used_columns));
  DBUG_RETURN(MY_TEST(thd->is_error()));
}