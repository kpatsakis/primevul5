void Item_equal::update_const(THD *thd)
{
  List_iterator<Item> it(equal_items);
  if (with_const)
    it++;
  Item *item;
  while ((item= it++))
  {
    if (item->const_item() && !item->is_expensive() &&
        /*
          Don't propagate constant status of outer-joined column.
          Such a constant status here is a result of:
            a) empty outer-joined table: in this case such a column has a
               value of NULL; but at the same time other arguments of
               Item_equal don't have to be NULLs and the value of the whole
               multiple equivalence expression doesn't have to be NULL or FALSE
               because of the outer join nature;
          or
            b) outer-joined table contains only 1 row: the result of
               this column is equal to a row field value *or* NULL.
          Both values are inacceptable as Item_equal constants.
        */
        !item->is_outer_field())
    {
      if (item == equal_items.head())
        with_const= TRUE;
      else
      {
        it.remove();
        add_const(thd, item);
      }
    } 
  }
}