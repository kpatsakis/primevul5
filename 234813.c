fix_inner_refs(THD *thd, List<Item> &all_fields, SELECT_LEX *select,
               Ref_ptr_array ref_pointer_array)
{
  Item_outer_ref *ref;

  /*
    Mark the references from  the inner_refs_list that are occurred in
    the group by expressions. Those references will contain direct
    references to the referred fields. The markers are set in 
    the found_in_group_by field of the references from the list.
  */
  List_iterator_fast <Item_outer_ref> ref_it(select->inner_refs_list);
  for (ORDER *group= select->join->group_list; group;  group= group->next)
  {
    (*group->item)->walk(&Item::check_inner_refs_processor, TRUE, &ref_it);
  } 
    
  while ((ref= ref_it++))
  {
    bool direct_ref= false;
    Item *item= ref->outer_ref;
    Item **item_ref= ref->ref;
    Item_ref *new_ref;
    /*
      TODO: this field item already might be present in the select list.
      In this case instead of adding new field item we could use an
      existing one. The change will lead to less operations for copying fields,
      smaller temporary tables and less data passed through filesort.
    */
    if (!ref_pointer_array.is_null() && !ref->found_in_select_list)
    {
      int el= all_fields.elements;
      ref_pointer_array[el]= item;
      /* Add the field item to the select list of the current select. */
      all_fields.push_front(item, thd->mem_root);
      /*
        If it's needed reset each Item_ref item that refers this field with
        a new reference taken from ref_pointer_array.
      */
      item_ref= &ref_pointer_array[el];
    }

    if (ref->in_sum_func)
    {
      Item_sum *sum_func;
      if (ref->in_sum_func->nest_level > select->nest_level)
        direct_ref= TRUE;
      else
      {
        for (sum_func= ref->in_sum_func; sum_func &&
             sum_func->aggr_level >= select->nest_level;
             sum_func= sum_func->in_sum_func)
        {
          if (sum_func->aggr_level == select->nest_level)
          {
            direct_ref= TRUE;
            break;
          }
        }
      }
    }
    else if (ref->found_in_group_by)
      direct_ref= TRUE;

    new_ref= direct_ref ?
              new (thd->mem_root) Item_direct_ref(thd, ref->context, item_ref, ref->table_name,
                          &ref->field_name, ref->alias_name_used) :
              new (thd->mem_root) Item_ref(thd, ref->context, item_ref, ref->table_name,
                          &ref->field_name, ref->alias_name_used);
    if (!new_ref)
      return TRUE;
    ref->outer_ref= new_ref;
    ref->ref= &ref->outer_ref;

    if (ref->fix_fields_if_needed(thd, 0))
      return TRUE;
    thd->lex->used_tables|= item->used_tables();
    thd->lex->current_select->select_list_tables|= item->used_tables();
  }
  return false;
}