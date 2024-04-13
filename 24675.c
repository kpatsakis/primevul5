int cmp_item_temporal::compare(cmp_item *ci)
{
  cmp_item_temporal *l_cmp= (cmp_item_temporal *)ci;
  return (value < l_cmp->value) ? -1 : ((value == l_cmp->value) ? 0 : 1);
}