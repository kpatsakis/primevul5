sort_keyuse(KEYUSE *a,KEYUSE *b)
{
  int res;
  if (a->table->tablenr != b->table->tablenr)
    return (int) (a->table->tablenr - b->table->tablenr);
  if (a->key != b->key)
    return (int) (a->key - b->key);
  if (a->key == MAX_KEY && b->key == MAX_KEY && 
      a->used_tables != b->used_tables)
    return (int) ((ulong) a->used_tables - (ulong) b->used_tables);
  if (a->keypart != b->keypart)
    return (int) (a->keypart - b->keypart);
  // Place const values before other ones
  if ((res= MY_TEST((a->used_tables & ~OUTER_REF_TABLE_BIT)) -
       MY_TEST((b->used_tables & ~OUTER_REF_TABLE_BIT))))
    return res;
  /* Place rows that are not 'OPTIMIZE_REF_OR_NULL' first */
  return (int) ((a->optimize & KEY_OPTIMIZE_REF_OR_NULL) -
		(b->optimize & KEY_OPTIMIZE_REF_OR_NULL));
}