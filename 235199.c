  void print_for_table_def(String *str)
  {
    print_parenthesised(str,
                     (enum_query_type)(QT_ITEM_ORIGINAL_FUNC_NULLIF |
                                       QT_ITEM_IDENT_SKIP_DB_NAMES |
                                       QT_ITEM_IDENT_SKIP_TABLE_NAMES |
                                       QT_NO_DATA_EXPANSION |
                                       QT_TO_SYSTEM_CHARSET),
                     LOWEST_PRECEDENCE);
  }