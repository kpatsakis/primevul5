bool Predicant_to_list_comparator::alloc_comparators(THD *thd, uint nargs)
{
  size_t nbytes= sizeof(Predicant_to_value_comparator) * nargs;
  if (!(m_comparators= (Predicant_to_value_comparator *) thd->alloc(nbytes)))
    return true;
  memset(m_comparators, 0, nbytes);
  return false;
}