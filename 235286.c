  bool value_depends_on_sql_mode_const_item()
  {
    /*
      Currently we use value_depends_on_sql_mode() only for virtual
      column expressions. They should not contain any expensive items.
      If we ever get a crash on the assert below, it means
      check_vcol_func_processor() is badly implemented for this item.
    */
    DBUG_ASSERT(!is_expensive());
    /*
      It should return const_item() actually.
      But for some reasons Item_field::const_item() returns true
      at value_depends_on_sql_mode() call time.
      This should be checked and fixed.
    */
    return basic_const_item();
  }