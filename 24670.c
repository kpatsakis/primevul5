longlong Item_func_like::val_int()
{
  DBUG_ASSERT(fixed == 1);
  DBUG_ASSERT(escape != ESCAPE_NOT_INITIALIZED);
  String* res= args[0]->val_str(&cmp_value1);
  if (args[0]->null_value)
  {
    null_value=1;
    return 0;
  }
  String* res2= args[1]->val_str(&cmp_value2);
  if (args[1]->null_value)
  {
    null_value=1;
    return 0;
  }
  null_value=0;
  if (canDoTurboBM)
    return turboBM_matches(res->ptr(), res->length()) ? !negated : negated;
  return my_wildcmp(cmp_collation.collation,
		    res->ptr(),res->ptr()+res->length(),
		    res2->ptr(),res2->ptr()+res2->length(),
		    escape,wild_one,wild_many) ? negated : !negated;
}