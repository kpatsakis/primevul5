inline void TABLE::mark_virtual_column_deps(Field *field)
{
  DBUG_ASSERT(field->vcol_info);
  DBUG_ASSERT(field->vcol_info->expr);
  field->vcol_info->expr->walk(&Item::register_field_in_read_map, 1, 0);
}