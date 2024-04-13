static void add_not_null_conds(JOIN *join)
{
  JOIN_TAB *tab;
  DBUG_ENTER("add_not_null_conds");
  
  for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
    if (tab->type == JT_REF || tab->type == JT_EQ_REF || 
        tab->type == JT_REF_OR_NULL)
    {
      for (uint keypart= 0; keypart < tab->ref.key_parts; keypart++)
      {
        if (tab->ref.null_rejecting & ((key_part_map)1 << keypart))
        {
          Item *item= tab->ref.items[keypart];
          Item *notnull;
          Item *real= item->real_item();
	  if (real->const_item() && real->type() != Item::FIELD_ITEM && 
              !real->is_expensive())
          {
            /*
              It could be constant instead of field after constant
              propagation.
            */
            continue;
          }
          DBUG_ASSERT(real->type() == Item::FIELD_ITEM);
          Item_field *not_null_item= (Item_field*)real;
          JOIN_TAB *referred_tab= not_null_item->field->table->reginfo.join_tab;
          /*
            For UPDATE queries such as:
            UPDATE t1 SET t1.f2=(SELECT MAX(t2.f4) FROM t2 WHERE t2.f3=t1.f1);
            not_null_item is the t1.f1, but it's referred_tab is 0.
          */
          if (!(notnull= new (join->thd->mem_root)
                Item_func_isnotnull(join->thd, item)))
            DBUG_VOID_RETURN;
          /*
            We need to do full fix_fields() call here in order to have correct
            notnull->const_item(). This is needed e.g. by test_quick_select 
            when it is called from make_join_select after this function is 
            called.
          */
          if (notnull->fix_fields(join->thd, &notnull))
            DBUG_VOID_RETURN;

          DBUG_EXECUTE("where",print_where(notnull,
                                            (referred_tab ?
                                            referred_tab->table->alias.c_ptr() :
                                            "outer_ref_cond"),
                                            QT_ORDINARY););
          if (!tab->first_inner)
          {
            COND *new_cond= (referred_tab && referred_tab->join == join) ?
                              referred_tab->select_cond :
                              join->outer_ref_cond;
            add_cond_and_fix(join->thd, &new_cond, notnull);
            if (referred_tab && referred_tab->join == join)
              referred_tab->set_select_cond(new_cond, __LINE__);
            else 
              join->outer_ref_cond= new_cond;
          }
          else
            add_cond_and_fix(join->thd, tab->first_inner->on_expr_ref, notnull);
        }
      }
    }
  }
  DBUG_VOID_RETURN;
}