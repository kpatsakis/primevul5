find_order_in_list(THD *thd, Ref_ptr_array ref_pointer_array,
                   TABLE_LIST *tables,
                   ORDER *order, List<Item> &fields, List<Item> &all_fields,
                   bool is_group_field, bool add_to_all_fields,
                   bool from_window_spec)
{
  Item *order_item= *order->item; /* The item from the GROUP/ORDER caluse. */
  Item::Type order_item_type;
  Item **select_item; /* The corresponding item from the SELECT clause. */
  Field *from_field;  /* The corresponding field from the FROM clause. */
  uint counter;
  enum_resolution_type resolution;

  /*
    Local SP variables may be int but are expressions, not positions.
    (And they can't be used before fix_fields is called for them).
  */
  if (order_item->type() == Item::INT_ITEM && order_item->basic_const_item() &&
      !from_window_spec)
  {						/* Order by position */
    uint count;
    if (order->counter_used)
      count= order->counter; // counter was once resolved
    else
      count= (uint) order_item->val_int();
    if (!count || count > fields.elements)
    {
      my_error(ER_BAD_FIELD_ERROR, MYF(0),
               order_item->full_name(), thd->where);
      return TRUE;
    }
    thd->change_item_tree((Item **)&order->item, (Item *)&ref_pointer_array[count - 1]);
    order->in_field_list= 1;
    order->counter= count;
    order->counter_used= 1;
    return FALSE;
  }
  /* Lookup the current GROUP/ORDER field in the SELECT clause. */
  select_item= find_item_in_list(order_item, fields, &counter,
                                 REPORT_EXCEPT_NOT_FOUND, &resolution);
  if (!select_item)
    return TRUE; /* The item is not unique, or some other error occurred. */


  /* Check whether the resolved field is not ambiguos. */
  if (select_item != not_found_item)
  {
    Item *view_ref= NULL;
    /*
      If we have found field not by its alias in select list but by its
      original field name, we should additionally check if we have conflict
      for this name (in case if we would perform lookup in all tables).
    */
    if (resolution == RESOLVED_BEHIND_ALIAS &&
        order_item->fix_fields_if_needed_for_order_by(thd, order->item))
      return TRUE;

    /* Lookup the current GROUP field in the FROM clause. */
    order_item_type= order_item->type();
    from_field= (Field*) not_found_field;
    if ((is_group_field && order_item_type == Item::FIELD_ITEM) ||
        order_item_type == Item::REF_ITEM)
    {
      from_field= find_field_in_tables(thd, (Item_ident*) order_item, tables,
                                       NULL, &view_ref, IGNORE_ERRORS, FALSE,
                                       FALSE);
      if (!from_field)
        from_field= (Field*) not_found_field;
    }

    if (from_field == not_found_field ||
        (from_field != view_ref_found ?
         /* it is field of base table => check that fields are same */
         ((*select_item)->type() == Item::FIELD_ITEM &&
          ((Item_field*) (*select_item))->field->eq(from_field)) :
         /*
           in is field of view table => check that references on translation
           table are same
         */
         ((*select_item)->type() == Item::REF_ITEM &&
          view_ref->type() == Item::REF_ITEM &&
          ((Item_ref *) (*select_item))->ref ==
          ((Item_ref *) view_ref)->ref)))
    {
      /*
        If there is no such field in the FROM clause, or it is the same field
        as the one found in the SELECT clause, then use the Item created for
        the SELECT field. As a result if there was a derived field that
        'shadowed' a table field with the same name, the table field will be
        chosen over the derived field.
      */
      order->item= &ref_pointer_array[counter];
      order->in_field_list=1;
      return FALSE;
    }
    else
    {
      /*
        There is a field with the same name in the FROM clause. This
        is the field that will be chosen. In this case we issue a
        warning so the user knows that the field from the FROM clause
        overshadows the column reference from the SELECT list.
      */
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                          ER_NON_UNIQ_ERROR,
                          ER_THD(thd, ER_NON_UNIQ_ERROR),
                          ((Item_ident*) order_item)->field_name.str,
                          thd->where);
    }
  }
  else if (from_window_spec)
  {
    Item **found_item= find_item_in_list(order_item, all_fields, &counter,
                                         REPORT_EXCEPT_NOT_FOUND, &resolution,
                                         all_fields.elements - fields.elements);
    if (found_item != not_found_item)
    {
      order->item= &ref_pointer_array[all_fields.elements-1-counter];
      order->in_field_list= 0;
      return FALSE;
    }
  }

  order->in_field_list=0;
  /*
    The call to order_item->fix_fields() means that here we resolve
    'order_item' to a column from a table in the list 'tables', or to
    a column in some outer query. Exactly because of the second case
    we come to this point even if (select_item == not_found_item),
    inspite of that fix_fields() calls find_item_in_list() one more
    time.

    We check order_item->fixed because Item_func_group_concat can put
    arguments for which fix_fields already was called.    
  */
  if (order_item->fix_fields_if_needed_for_order_by(thd, order->item) ||
      thd->is_error())
    return TRUE; /* Wrong field. */
  order_item= *order->item; // Item can change during fix_fields()

  if (!add_to_all_fields)
    return FALSE;

  uint el= all_fields.elements;
 /* Add new field to field list. */
  all_fields.push_front(order_item, thd->mem_root);
  ref_pointer_array[el]= order_item;
  /*
     If the order_item is a SUM_FUNC_ITEM, when fix_fields is called
     ref_by is set to order->item which is the address of order_item.
     But this needs to be address of order_item in the all_fields list.
     As a result, when it gets replaced with Item_aggregate_ref
     object in Item::split_sum_func2, we will be able to retrieve the
     newly created object.
  */
  if (order_item->type() == Item::SUM_FUNC_ITEM)
    ((Item_sum *)order_item)->ref_by= all_fields.head_ref();

  order->item= &ref_pointer_array[el];
  return FALSE;
}