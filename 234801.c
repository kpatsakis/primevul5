update_ref_and_keys(THD *thd, DYNAMIC_ARRAY *keyuse,JOIN_TAB *join_tab,
                    uint tables, COND *cond, table_map normal_tables,
                    SELECT_LEX *select_lex, SARGABLE_PARAM **sargables)
{
  uint	and_level,i;
  KEY_FIELD *key_fields, *end, *field;
  uint sz;
  uint m= MY_MAX(select_lex->max_equal_elems,1);
  DBUG_ENTER("update_ref_and_keys");
  DBUG_PRINT("enter", ("normal_tables: %llx", normal_tables));

  SELECT_LEX *sel=thd->lex->current_select; 
  sel->cond_count= 0;
  sel->between_count= 0; 
  if (cond)
    cond->walk(&Item::count_sargable_conds, 0, sel);
  for (i=0 ; i < tables ; i++)
  {
    if (*join_tab[i].on_expr_ref)
      (*join_tab[i].on_expr_ref)->walk(&Item::count_sargable_conds, 0, sel);
  }
  {
    List_iterator<TABLE_LIST> li(*join_tab->join->join_list);
    TABLE_LIST *table;
    while ((table= li++))
    {
      if (table->nested_join)
        count_cond_for_nj(sel, table);
    }
  }
  
  /* 
    We use the same piece of memory to store both  KEY_FIELD 
    and SARGABLE_PARAM structure.
    KEY_FIELD values are placed at the beginning this memory
    while  SARGABLE_PARAM values are put at the end.
    All predicates that are used to fill arrays of KEY_FIELD
    and SARGABLE_PARAM structures have at most 2 arguments
    except BETWEEN predicates that have 3 arguments and 
    IN predicates.
    This any predicate if it's not BETWEEN/IN can be used 
    directly to fill at most 2 array elements, either of KEY_FIELD
    or SARGABLE_PARAM type. For a BETWEEN predicate 3 elements
    can be filled as this predicate is considered as
    saragable with respect to each of its argument.
    An IN predicate can require at most 1 element as currently
    it is considered as sargable only for its first argument.
    Multiple equality can add  elements that are filled after
    substitution of field arguments by equal fields. There
    can be not more than select_lex->max_equal_elems such 
    substitutions.
  */ 
  sz= MY_MAX(sizeof(KEY_FIELD),sizeof(SARGABLE_PARAM))*
    ((sel->cond_count*2 + sel->between_count)*m+1);
  if (!(key_fields=(KEY_FIELD*)	thd->alloc(sz)))
    DBUG_RETURN(TRUE); /* purecov: inspected */
  and_level= 0;
  field= end= key_fields;
  *sargables= (SARGABLE_PARAM *) key_fields + 
                (sz - sizeof((*sargables)[0].field))/sizeof(SARGABLE_PARAM);
  /* set a barrier for the array of SARGABLE_PARAM */
  (*sargables)[0].field= 0; 

  if (my_init_dynamic_array2(keyuse, sizeof(KEYUSE),
                             thd->alloc(sizeof(KEYUSE) * 20), 20, 64,
                             MYF(MY_THREAD_SPECIFIC)))
    DBUG_RETURN(TRUE);

  if (cond)
  {
    KEY_FIELD *saved_field= field;
    cond->add_key_fields(join_tab->join, &end, &and_level, normal_tables,
                         sargables);
    for (; field != end ; field++)
    {

      /* Mark that we can optimize LEFT JOIN */
      if (field->val->type() == Item::NULL_ITEM &&
	  !field->field->real_maybe_null())
	field->field->table->reginfo.not_exists_optimize=1;
    }
    field= saved_field;
  }
  for (i=0 ; i < tables ; i++)
  {
    /*
      Block the creation of keys for inner tables of outer joins.
      Here only the outer joins that can not be converted to
      inner joins are left and all nests that can be eliminated
      are flattened.
      In the future when we introduce conditional accesses
      for inner tables in outer joins these keys will be taken
      into account as well.
    */ 
    if (*join_tab[i].on_expr_ref)
      (*join_tab[i].on_expr_ref)->add_key_fields(join_tab->join, &end,
                                                 &and_level, 
                                                 join_tab[i].table->map,
                                                 sargables);
  }

  /* Process ON conditions for the nested joins */
  {
    List_iterator<TABLE_LIST> li(*join_tab->join->join_list);
    TABLE_LIST *table;
    while ((table= li++))
    {
      if (table->nested_join)
        add_key_fields_for_nj(join_tab->join, table, &end, &and_level, 
                              sargables);
    }
  }

  /* fill keyuse with found key parts */
  for ( ; field != end ; field++)
  {
    if (add_key_part(keyuse,field))
      DBUG_RETURN(TRUE);
  }

  if (select_lex->ftfunc_list->elements)
  {
    if (add_ft_keys(keyuse,join_tab,cond,normal_tables))
      DBUG_RETURN(TRUE);
  }

  DBUG_RETURN(FALSE);
}