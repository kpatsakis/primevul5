Item *Item_cond::compile(THD *thd, Item_analyzer analyzer, uchar **arg_p,
                         Item_transformer transformer, uchar *arg_t)
{
  if (!(this->*analyzer)(arg_p))
    return 0;
  
  List_iterator<Item> li(list);
  Item *item;
  while ((item= li++))
  {
    /* 
      The same parameter value of arg_p must be passed
      to analyze any argument of the condition formula.
    */   
    uchar *arg_v= *arg_p;
    Item *new_item= item->compile(thd, analyzer, &arg_v, transformer, arg_t);
    if (new_item && new_item != item)
      thd->change_item_tree(li.ref(), new_item);
  }
  return Item_func::transform(thd, transformer, arg_t);
}