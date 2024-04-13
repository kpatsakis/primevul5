bool ha_maria::check_if_incompatible_data(HA_CREATE_INFO *create_info,
                                          uint table_changes)
{
  DBUG_ENTER("check_if_incompatible_data");
  uint options= table->s->db_options_in_use;
  enum ha_choice page_checksum= table->s->page_checksum;

  if (page_checksum == HA_CHOICE_UNDEF)
    page_checksum= file->s->options & HA_OPTION_PAGE_CHECKSUM ? HA_CHOICE_YES
                                                              : HA_CHOICE_NO;

  if (create_info->auto_increment_value != stats.auto_increment_value ||
      create_info->data_file_name != data_file_name ||
      create_info->index_file_name != index_file_name ||
      create_info->page_checksum != page_checksum ||
      create_info->transactional != table->s->transactional ||
      (maria_row_type(create_info) != data_file_type &&
       create_info->row_type != ROW_TYPE_DEFAULT) ||
      table_changes == IS_EQUAL_NO ||
      (table_changes & IS_EQUAL_PACK_LENGTH)) // Not implemented yet
    DBUG_RETURN(COMPATIBLE_DATA_NO);

  if ((options & (HA_OPTION_CHECKSUM |
                  HA_OPTION_DELAY_KEY_WRITE)) !=
      (create_info->table_options & (HA_OPTION_CHECKSUM |
                              HA_OPTION_DELAY_KEY_WRITE)))
    DBUG_RETURN(COMPATIBLE_DATA_NO);
  DBUG_RETURN(COMPATIBLE_DATA_YES);
}