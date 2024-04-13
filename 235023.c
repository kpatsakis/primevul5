  Sql_mode_dependency value_depends_on_sql_mode() const
  {
    return Item_args::value_depends_on_sql_mode_bit_or().soft_to_hard();
  }