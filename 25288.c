Item *Item_func_xor::neg_transformer(THD *thd)
{
  Item *neg_operand;
  Item_func_xor *new_item;
  if ((neg_operand= args[0]->neg_transformer(thd)))
    // args[0] has neg_tranformer
    new_item= new(thd->mem_root) Item_func_xor(thd, neg_operand, args[1]);
  else if ((neg_operand= args[1]->neg_transformer(thd)))
    // args[1] has neg_tranformer
    new_item= new(thd->mem_root) Item_func_xor(thd, args[0], neg_operand);
  else
  {
    neg_operand= new(thd->mem_root) Item_func_not(thd, args[0]);
    new_item= new(thd->mem_root) Item_func_xor(thd, neg_operand, args[1]);
  }
  return new_item;
}