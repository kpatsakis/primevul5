bool LEX::is_partition_management() const
{
  return (sql_command == SQLCOM_ALTER_TABLE &&
          (alter_info.flags ==  Alter_info::ALTER_ADD_PARTITION ||
           alter_info.flags ==  Alter_info::ALTER_REORGANIZE_PARTITION));
}