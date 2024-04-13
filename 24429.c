TABLE_LIST *st_select_lex_node::add_table_to_list(THD *thd, Table_ident *table,
						  LEX_STRING *alias,
						  ulong table_join_options,
						  thr_lock_type flags,
                                                  enum_mdl_type mdl_type,
						  List<Index_hint> *hints,
                                                  List<String> *partition_names,
                                                  LEX_STRING *option)
{
  return 0;
}