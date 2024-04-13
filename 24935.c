Item *Item_equal::multiple_equality_transformer(THD *thd, uchar *arg)
{
  List<Item> equalities;
  if (create_pushable_equalities(thd, &equalities, 0, 0, false))
    return 0;

  switch (equalities.elements)
  {
  case 0:
    return 0;
  case 1:
    return equalities.head();
    break;
  default:
    return new (thd->mem_root) Item_cond_and(thd, equalities);
    break;
  }
}