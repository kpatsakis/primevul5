change_to_use_tmp_fields(THD *thd, Ref_ptr_array ref_pointer_array,
			 List<Item> &res_selected_fields,
			 List<Item> &res_all_fields,
			 uint elements, List<Item> &all_fields)
{
  List_iterator_fast<Item> it(all_fields);
  Item *item_field,*item;
  DBUG_ENTER("change_to_use_tmp_fields");

  res_selected_fields.empty();
  res_all_fields.empty();

  uint border= all_fields.elements - elements;
  for (uint i= 0; (item= it++); i++)
  {
    Field *field;
    if ((item->with_sum_func && item->type() != Item::SUM_FUNC_ITEM) ||
       item->with_window_func)
      item_field= item;
    else if (item->type() == Item::FIELD_ITEM)
    {
      if (!(item_field= item->get_tmp_table_item(thd)))
        DBUG_RETURN(true);
    }
    else if (item->type() == Item::FUNC_ITEM &&
             ((Item_func*)item)->functype() == Item_func::SUSERVAR_FUNC)
    {
      field= item->get_tmp_table_field();
      if (field != NULL)
      {
        /*
          Replace "@:=<expression>" with "@:=<tmp table
          column>". Otherwise, we would re-evaluate <expression>, and
          if expression were a subquery, this would access
          already-unlocked tables.
         */
        Item_func_set_user_var* suv=
          new (thd->mem_root) Item_func_set_user_var(thd, (Item_func_set_user_var*) item);
        Item_field *new_field= new (thd->mem_root) Item_temptable_field(thd, field);
        if (!suv || !new_field)
          DBUG_RETURN(true);                  // Fatal error
        List<Item> list;
        list.push_back(new_field, thd->mem_root);
        suv->set_arguments(thd, list);
        item_field= suv;
      }
      else
        item_field= item;
    }
    else if ((field= item->get_tmp_table_field()))
    {
      if (item->type() == Item::SUM_FUNC_ITEM && field->table->group)
        item_field= ((Item_sum*) item)->result_item(thd, field);
      else
        item_field= (Item *) new (thd->mem_root) Item_temptable_field(thd, field);
      if (!item_field)
        DBUG_RETURN(true);                    // Fatal error

      if (item->real_item()->type() != Item::FIELD_ITEM)
        field->orig_table= 0;
      item_field->name= item->name;
      if (item->type() == Item::REF_ITEM)
      {
        Item_field *ifield= (Item_field *) item_field;
        Item_ref *iref= (Item_ref *) item;
        ifield->table_name= iref->table_name;
        ifield->db_name= iref->db_name;
      }
#ifndef DBUG_OFF
      if (!item_field->name.str)
      {
        char buff[256];
        String str(buff,sizeof(buff),&my_charset_bin);
        str.length(0);
        str.extra_allocation(1024);
        item->print(&str, QT_ORDINARY);
        item_field->name.str= thd->strmake(str.ptr(), str.length());
        item_field->name.length= str.length();
      }
#endif
    }
    else
      item_field= item;

    res_all_fields.push_back(item_field, thd->mem_root);
    ref_pointer_array[((i < border)? all_fields.elements-i-1 : i-border)]=
      item_field;
  }

  List_iterator_fast<Item> itr(res_all_fields);
  for (uint i= 0; i < border; i++)
    itr++;
  itr.sublist(res_selected_fields, elements);
  DBUG_RETURN(false);
}