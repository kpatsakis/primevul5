change_refs_to_tmp_fields(THD *thd, Ref_ptr_array ref_pointer_array,
			  List<Item> &res_selected_fields,
			  List<Item> &res_all_fields, uint elements,
			  List<Item> &all_fields)
{
  List_iterator_fast<Item> it(all_fields);
  Item *item, *new_item;
  res_selected_fields.empty();
  res_all_fields.empty();

  uint i, border= all_fields.elements - elements;
  for (i= 0; (item= it++); i++)
  {
    if (item->type() == Item::SUM_FUNC_ITEM && item->const_item())
      new_item= item;
    else
    {
      if (!(new_item= item->get_tmp_table_item(thd)))
        return 1;
    }

    if (res_all_fields.push_back(new_item, thd->mem_root))
      return 1;
    ref_pointer_array[((i < border)? all_fields.elements-i-1 : i-border)]=
      new_item;
  }

  List_iterator_fast<Item> itr(res_all_fields);
  for (i= 0; i < border; i++)
    itr++;
  itr.sublist(res_selected_fields, elements);

  return thd->is_fatal_error;
}