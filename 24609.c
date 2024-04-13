Item *Item_bool_rowready_func2::neg_transformer(THD *thd)
{
  Item *item= negated_item(thd);
  return item;
}