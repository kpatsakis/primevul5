Item *in_double::create_item(THD *thd)
{ 
  return new (thd->mem_root) Item_float(thd, 0.0, 0);
}