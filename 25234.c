Item *in_longlong::create_item(THD *thd)
{ 
  /* 
     We're created a signed INT, this may not be correct in 
     general case (see BUG#19342).
  */
  return new (thd->mem_root) Item_int(thd, (longlong)0);
}