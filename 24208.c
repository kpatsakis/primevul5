int setup_wild(THD *thd, TABLE_LIST *tables, List<Item> &fields,
	       List<Item> *sum_func_list,
	       uint wild_num, uint *hidden_bit_fields)
{
  if (!wild_num)
    return(0);

  Item *item;
  List_iterator<Item> it(fields);
  Query_arena *arena, backup;
  DBUG_ENTER("setup_wild");

  /*
    Don't use arena if we are not in prepared statements or stored procedures
    For PS/SP we have to use arena to remember the changes
  */
  arena= thd->activate_stmt_arena_if_needed(&backup);

  thd->lex->current_select->cur_pos_in_select_list= 0;
  while (wild_num && (item= it++))
  {
    if (item->type() == Item::FIELD_ITEM &&
        ((Item_field*) item)->field_name &&
	((Item_field*) item)->field_name[0] == '*' &&
	!((Item_field*) item)->field)
    {
      uint elem= fields.elements;
      bool any_privileges= ((Item_field *) item)->any_privileges;
      Item_subselect *subsel= thd->lex->current_select->master_unit()->item;
      if (subsel &&
          subsel->substype() == Item_subselect::EXISTS_SUBS)
      {
        /*
          It is EXISTS(SELECT * ...) and we can replace * by any constant.

          Item_int do not need fix_fields() because it is basic constant.
        */
        it.replace(new (thd->mem_root) Item_int(thd, "Not_used", (longlong) 1,
                                MY_INT64_NUM_DECIMAL_DIGITS));
      }
      else if (insert_fields(thd, ((Item_field*) item)->context,
                             ((Item_field*) item)->db_name,
                             ((Item_field*) item)->table_name, &it,
                             any_privileges, hidden_bit_fields))
      {
	if (arena)
	  thd->restore_active_arena(arena, &backup);
	DBUG_RETURN(-1);
      }
      if (sum_func_list)
      {
	/*
	  sum_func_list is a list that has the fields list as a tail.
	  Because of this we have to update the element count also for this
	  list after expanding the '*' entry.
	*/
	sum_func_list->elements+= fields.elements - elem;
      }
      wild_num--;
    }
    else
      thd->lex->current_select->cur_pos_in_select_list++;
  }
  thd->lex->current_select->cur_pos_in_select_list= UNDEF_POS;
  if (arena)
  {
    /* make * substituting permanent */
    SELECT_LEX *select_lex= thd->lex->current_select;
    select_lex->with_wild= 0;
#ifdef HAVE_valgrind
    if (&select_lex->item_list != &fields)      // Avoid warning
#endif
    /*   
      The assignment below is translated to memcpy() call (at least on some
      platforms). memcpy() expects that source and destination areas do not
      overlap. That problem was detected by valgrind. 
    */
    if (&select_lex->item_list != &fields)
      select_lex->item_list= fields;

    thd->restore_active_arena(arena, &backup);
  }
  DBUG_RETURN(0);
}