Item_cond::fix_fields(THD *thd, Item **ref)
{
  DBUG_ASSERT(fixed == 0);
  List_iterator<Item> li(list);
  Item *item;
  uchar buff[sizeof(char*)];			// Max local vars in function
  bool is_and_cond= functype() == Item_func::COND_AND_FUNC;
  not_null_tables_cache= 0;
  used_tables_and_const_cache_init();

  /*
    and_table_cache is the value that Item_cond_or() returns for
    not_null_tables()
  */
  and_tables_cache= ~(table_map) 0;

  if (check_stack_overrun(thd, STACK_MIN_SIZE, buff))
    return TRUE;				// Fatal error flag is set!
  /*
    The following optimization reduces the depth of an AND-OR tree.
    E.g. a WHERE clause like
      F1 AND (F2 AND (F2 AND F4))
    is parsed into a tree with the same nested structure as defined
    by braces. This optimization will transform such tree into
      AND (F1, F2, F3, F4).
    Trees of OR items are flattened as well:
      ((F1 OR F2) OR (F3 OR F4))   =>   OR (F1, F2, F3, F4)
    Items for removed AND/OR levels will dangle until the death of the
    entire statement.
    The optimization is currently prepared statements and stored procedures
    friendly as it doesn't allocate any memory and its effects are durable
    (i.e. do not depend on PS/SP arguments).
  */
  while ((item=li++))
  {
    while (item->type() == Item::COND_ITEM &&
	   ((Item_cond*) item)->functype() == functype() &&
           !((Item_cond*) item)->list.is_empty())
    {						// Identical function
      li.replace(((Item_cond*) item)->list);
      ((Item_cond*) item)->list.empty();
      item= *li.ref();				// new current item
    }
    if (abort_on_null)
      item->top_level_item();

    /*
      replace degraded condition:
        was:    <field>
        become: <field> = 1
    */
    Item::Type type= item->type();
    if (type == Item::FIELD_ITEM || type == Item::REF_ITEM)
    {
      Query_arena backup, *arena;
      Item *new_item;
      arena= thd->activate_stmt_arena_if_needed(&backup);
      if ((new_item= new (thd->mem_root) Item_func_ne(thd, item, new (thd->mem_root) Item_int(thd, 0, 1))))
        li.replace(item= new_item);
      if (arena)
        thd->restore_active_arena(arena, &backup);
    }

    if (item->fix_fields_if_needed_for_bool(thd, li.ref()))
      return TRUE; /* purecov: inspected */
    item= *li.ref(); // item can be substituted in fix_fields
    used_tables_cache|=     item->used_tables();
    if (item->const_item() && !item->with_param &&
        !item->is_expensive() && !cond_has_datetime_is_null(item))
    {
      if (item->eval_const_cond() == is_and_cond && top_level())
      {
        /* 
          a. This is "... AND true_cond AND ..."
          In this case, true_cond  has no effect on cond_and->not_null_tables()
          b. This is "... OR false_cond/null cond OR ..." 
          In this case, false_cond has no effect on cond_or->not_null_tables()
        */
      }
      else
      {
        /* 
          a. This is "... AND false_cond/null_cond AND ..."
          The whole condition is FALSE/UNKNOWN.
          b. This is  "... OR const_cond OR ..."
          In this case, cond_or->not_null_tables()=0, because the condition
          const_cond might evaluate to true (regardless of whether some tables
          were NULL-complemented).
        */
        not_null_tables_cache= (table_map) 0;
        and_tables_cache= (table_map) 0;
      }
      if (thd->is_error())
        return TRUE;
    }
    else
    {
      table_map tmp_table_map= item->not_null_tables();
      not_null_tables_cache|= tmp_table_map;
      and_tables_cache&= tmp_table_map;

      const_item_cache= FALSE;
    } 
  
    join_with_sum_func(item);
    with_param|=       item->with_param;
    with_field|=       item->with_field;
    m_with_subquery|=  item->with_subquery();
    with_window_func|= item->with_window_func;
    maybe_null|=       item->maybe_null;
  }
  if (fix_length_and_dec())
    return TRUE;
  fixed= 1;
  return FALSE;
}