test_if_equality_guarantees_uniqueness(Item *l, Item *r)
{
  return (r->const_item() || !(r->used_tables() & ~OUTER_REF_TABLE_BIT)) &&
    item_cmp_type(l, r) == l->cmp_type() &&
    (l->cmp_type() != STRING_RESULT ||
     l->collation.collation == r->collation.collation);
}