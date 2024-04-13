Item *Item_func_nop_all::neg_transformer(THD *thd)
{
  /* "NOT (e $cmp$ ANY (SELECT ...)) -> e $rev_cmp$" ALL (SELECT ...) */
  Item_func_not_all *new_item= new (thd->mem_root) Item_func_not_all(thd, args[0]);
  Item_allany_subselect *allany= (Item_allany_subselect*)args[0];
  allany->create_comp_func(FALSE);
  allany->all= !allany->all;
  allany->upper_item= new_item;
  return new_item;
}