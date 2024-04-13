setup_new_fields(THD *thd, List<Item> &fields,
		 List<Item> &all_fields, ORDER *new_field)
{
  Item	  **item;
  uint counter;
  enum_resolution_type not_used;
  DBUG_ENTER("setup_new_fields");

  thd->column_usage= MARK_COLUMNS_READ;       // Not really needed, but...
  for (; new_field ; new_field= new_field->next)
  {
    if ((item= find_item_in_list(*new_field->item, fields, &counter,
				 IGNORE_ERRORS, &not_used)))
      new_field->item=item;			/* Change to shared Item */
    else
    {
      thd->where="procedure list";
      if ((*new_field->item)->fix_fields(thd, new_field->item))
	DBUG_RETURN(1); /* purecov: inspected */
      all_fields.push_front(*new_field->item, thd->mem_root);
      new_field->item=all_fields.head_ref();
    }
  }
  DBUG_RETURN(0);
}