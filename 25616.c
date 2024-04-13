bool JOIN::rollup_init()
{
  uint i,j;
  Item **ref_array;

  tmp_table_param.quick_group= 0;	// Can't create groups in tmp table
  rollup.state= ROLLUP::STATE_INITED;

  /*
    Create pointers to the different sum function groups
    These are updated by rollup_make_fields()
  */
  tmp_table_param.group_parts= send_group_parts;

  Item_null_result **null_items=
    static_cast<Item_null_result**>(thd->alloc(sizeof(Item*)*send_group_parts));

  rollup.null_items= Item_null_array(null_items, send_group_parts);
  rollup.ref_pointer_arrays=
    static_cast<Ref_ptr_array*>
    (thd->alloc((sizeof(Ref_ptr_array) +
                 all_fields.elements * sizeof(Item*)) * send_group_parts));
  rollup.fields=
    static_cast<List<Item>*>(thd->alloc(sizeof(List<Item>) * send_group_parts));

  if (!null_items || !rollup.ref_pointer_arrays || !rollup.fields)
    return true;

  ref_array= (Item**) (rollup.ref_pointer_arrays+send_group_parts);


  /*
    Prepare space for field list for the different levels
    These will be filled up in rollup_make_fields()
  */
  for (i= 0 ; i < send_group_parts ; i++)
  {
    rollup.null_items[i]= new (thd->mem_root) Item_null_result(thd);
    List<Item> *rollup_fields= &rollup.fields[i];
    rollup_fields->empty();
    rollup.ref_pointer_arrays[i]= Ref_ptr_array(ref_array, all_fields.elements);
    ref_array+= all_fields.elements;
  }
  for (i= 0 ; i < send_group_parts; i++)
  {
    for (j=0 ; j < fields_list.elements ; j++)
      rollup.fields[i].push_back(rollup.null_items[i], thd->mem_root);
  }
  List_iterator<Item> it(all_fields);
  Item *item;
  while ((item= it++))
  {
    ORDER *group_tmp;
    bool found_in_group= 0;

    for (group_tmp= group_list; group_tmp; group_tmp= group_tmp->next)
    {
      if (*group_tmp->item == item)
      {
        item->maybe_null= 1;
        item->in_rollup= 1;
        found_in_group= 1;
        break;
      }
    }
    if (item->type() == Item::FUNC_ITEM && !found_in_group)
    {
      bool changed= FALSE;
      if (change_group_ref(thd, (Item_func *) item, group_list, &changed))
        return 1;
      /*
        We have to prevent creation of a field in a temporary table for
        an expression that contains GROUP BY attributes.
        Marking the expression item as 'with_sum_func' will ensure this.
      */ 
      if (changed)
        item->with_sum_func= 1;
    }
  }
  return 0;
}