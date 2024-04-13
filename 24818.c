bool Item_func_like::with_sargable_pattern() const
{
  if (negated)
    return false;

  if (!args[1]->const_item() || args[1]->is_expensive())
    return false;

  String* res2= args[1]->val_str((String *) &cmp_value2);
  if (!res2)
    return false;

  if (!res2->length()) // Can optimize empty wildcard: column LIKE ''
    return true;

  DBUG_ASSERT(res2->ptr());
  char first= res2->ptr()[0];
  return first != wild_many && first != wild_one;
}