inline void JOIN::eval_select_list_used_tables()
{
  select_list_used_tables= 0;
  Item *item;
  List_iterator_fast<Item> it(fields_list);
  while ((item= it++))
  {
    select_list_used_tables|= item->used_tables();
  }
  Item_outer_ref *ref;
  List_iterator_fast<Item_outer_ref> ref_it(select_lex->inner_refs_list);
  while ((ref= ref_it++))
  {
    item= ref->outer_ref;
    select_list_used_tables|= item->used_tables();
  }
}