bool test_if_ref(Item *root_cond, Item_field *left_item,Item *right_item)
{
  Field *field=left_item->field;
  JOIN_TAB *join_tab= field->table->reginfo.join_tab;
  // No need to change const test
  if (!field->table->const_table && join_tab &&
      !join_tab->is_ref_for_hash_join() &&
      (!join_tab->first_inner ||
       *join_tab->first_inner->on_expr_ref == root_cond))
  {
    /*
      If ref access uses "Full scan on NULL key" (i.e. it actually alternates
      between ref access and full table scan), then no equality can be
      guaranteed to be true.
    */
    if (join_tab->ref.is_access_triggered())
      return FALSE;

    Item *ref_item=part_of_refkey(field->table,field);
    if (ref_item && (ref_item->eq(right_item,1) || 
		     ref_item->real_item()->eq(right_item,1)))
    {
      right_item= right_item->real_item();
      if (right_item->type() == Item::FIELD_ITEM)
	return (field->eq_def(((Item_field *) right_item)->field));
      /* remove equalities injected by IN->EXISTS transformation */
      else if (right_item->type() == Item::CACHE_ITEM)
        return ((Item_cache *)right_item)->eq_def (field);
      if (right_item->const_item() && !(right_item->is_null()))
      {
	/*
	  We can remove binary fields and numerical fields except float,
	  as float comparison isn't 100 % safe
	  We have to keep normal strings to be able to check for end spaces
	*/
	if (field->binary() &&
	    field->real_type() != MYSQL_TYPE_STRING &&
	    field->real_type() != MYSQL_TYPE_VARCHAR &&
	    (field->type() != MYSQL_TYPE_FLOAT || field->decimals() == 0))
	{
	  return !right_item->save_in_field_no_warnings(field, 1);
	}
      }
    }
  }
  return 0;					// keep test
}