static TABLE *item_rowid_table(Item *item)
{
  if (item->type() != Item::FUNC_ITEM)
    return NULL;
  Item_func *func= (Item_func *)item;
  if (func->functype() != Item_func::TEMPTABLE_ROWID)
    return NULL;
  Item_temptable_rowid *itr= (Item_temptable_rowid *)func;
  return itr->table;
}