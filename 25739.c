multi_update::multi_update(THD *thd_arg, TABLE_LIST *table_list,
                           List<TABLE_LIST> *leaves_list,
			   List<Item> *field_list, List<Item> *value_list,
			   enum enum_duplicates handle_duplicates_arg,
                           bool ignore_arg):
   select_result_interceptor(thd_arg),
   all_tables(table_list), leaves(leaves_list), update_tables(0),
   tmp_tables(0), updated(0), found(0), fields(field_list),
   values(value_list), table_count(0), copy_field(0),
   handle_duplicates(handle_duplicates_arg), do_update(1), trans_safe(1),
   transactional_tables(0), ignore(ignore_arg), error_handled(0), prepared(0),
   updated_sys_ver(0)
{
}