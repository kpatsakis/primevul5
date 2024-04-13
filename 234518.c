inline void add_cond_and_fix(THD *thd, Item **e1, Item *e2)
{
  if (*e1)
  {
    if (!e2)
      return;
    Item *res;
    if ((res= new (thd->mem_root) Item_cond_and(thd, *e1, e2)))
    {
      res->fix_fields(thd, 0);
      res->update_used_tables();
      *e1= res;
    }
  }
  else
    *e1= e2;
}