inline bool Virtual_column_info::is_equal(const Virtual_column_info* vcol) const
{
  return field_type == vcol->get_real_type()
      && stored_in_db == vcol->is_stored()
      && expr->eq(vcol->expr, true);
}