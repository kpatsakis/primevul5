bool Item_equal::count_sargable_conds(void *arg)
{
  SELECT_LEX *sel= (SELECT_LEX *) arg;
  uint m= equal_items.elements;
  sel->cond_count+= m*(m-1);
  return 0;
}