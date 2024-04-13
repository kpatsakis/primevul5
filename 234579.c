change_cond_ref_to_const(THD *thd, I_List<COND_CMP> *save_list,
                         Item *and_father, Item *cond,
                         Item_bool_func2 *field_value_owner,
                         Item *field, Item *value)
{
  if (cond->type() == Item::COND_ITEM)
  {
    bool and_level= ((Item_cond*) cond)->functype() ==
      Item_func::COND_AND_FUNC;
    List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
    Item *item;
    while ((item=li++))
      change_cond_ref_to_const(thd, save_list,and_level ? cond : item, item,
			       field_value_owner, field, value);
    return;
  }
  if (cond->eq_cmp_result() == Item::COND_OK)
    return;					// Not a boolean function

  Item_bool_func2 *func=  (Item_bool_func2*) cond;
  Item **args= func->arguments();
  Item *left_item=  args[0];
  Item *right_item= args[1];
  Item_func::Functype functype=  func->functype();

  if (can_change_cond_ref_to_const(func, right_item, left_item,
                                   field_value_owner, field, value))
  {
    Item *tmp=value->clone_item(thd);
    if (tmp)
    {
      tmp->collation.set(right_item->collation);
      thd->change_item_tree(args + 1, tmp);
      func->update_used_tables();
      if ((functype == Item_func::EQ_FUNC || functype == Item_func::EQUAL_FUNC)
	  && and_father != cond && !left_item->const_item())
      {
	cond->marker=1;
	COND_CMP *tmp2;
        /* Will work, even if malloc would fail */
        if ((tmp2= new (thd->mem_root) COND_CMP(and_father, func)))
	  save_list->push_back(tmp2);
      }
      /*
        LIKE can be optimized for BINARY/VARBINARY/BLOB columns, e.g.:

        from: WHERE CONCAT(c1)='const1' AND CONCAT(c1) LIKE 'const2'
          to: WHERE CONCAT(c1)='const1' AND 'const1' LIKE 'const2'

        So make sure to use set_cmp_func() only for non-LIKE operators.
      */
      if (functype != Item_func::LIKE_FUNC)
        ((Item_bool_rowready_func2*) func)->set_cmp_func();
    }
  }
  else if (can_change_cond_ref_to_const(func, left_item, right_item,
                                        field_value_owner, field, value))
  {
    Item *tmp= value->clone_item(thd);
    if (tmp)
    {
      tmp->collation.set(left_item->collation);
      thd->change_item_tree(args, tmp);
      value= tmp;
      func->update_used_tables();
      if ((functype == Item_func::EQ_FUNC || functype == Item_func::EQUAL_FUNC)
	  && and_father != cond && !right_item->const_item())
      {
        args[0]= args[1];                       // For easy check
        thd->change_item_tree(args + 1, value);
	cond->marker=1;
	COND_CMP *tmp2;
        /* Will work, even if malloc would fail */
        if ((tmp2=new (thd->mem_root) COND_CMP(and_father, func)))
	  save_list->push_back(tmp2);
      }
      if (functype != Item_func::LIKE_FUNC)
        ((Item_bool_rowready_func2*) func)->set_cmp_func();
    }
  }
}