  Item_ident_for_show(THD *thd, Field *par_field, const char *db_arg,
                      const char *table_name_arg):
    Item(thd), field(par_field), db_name(db_arg), table_name(table_name_arg)
  {}