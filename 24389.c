bool st_select_lex::add_item_to_list(THD *thd, Item *item)
{
  DBUG_ENTER("st_select_lex::add_item_to_list");
  DBUG_PRINT("info", ("Item: %p", item));
  DBUG_RETURN(item_list.push_back(item, thd->mem_root));
}