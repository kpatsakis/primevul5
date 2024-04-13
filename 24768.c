inline bool Virtual_column_info::is_equal(const Virtual_column_info* vcol) const
{
  return type_handler()  == vcol->type_handler()
      && stored_in_db == vcol->is_stored()
      && expr->eq(vcol->expr, true);
}